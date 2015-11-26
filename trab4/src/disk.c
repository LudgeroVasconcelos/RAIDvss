#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>

#include "raidvss.h"
#include "diskutils.h"
#include "conffile.h"
#include "conffileutils.h"
#include "ipc.h"
#include "disk.h"
#include "log.h"

/* Atraso em microsegundos para o disco se deslocar um sector */

#define DELAYSECTOR 10000

sector_t csector,seekdest;
direction_t cdir;
int disksize;

char* confname;
char diskname[255];


void estaciona_disco(){
	csector = 0;
	cdir = in;

	vsslog(confname,diskname, P);
	vsslog(confname,diskname, ER, csector, cdir);

	// volta a armar o sinal
	signal(SIGUSR1, estaciona_disco);
}


void incrementa_sector(){
		csector = cdir == in ? csector+1 : csector-1;
		signal(SIGALRM, incrementa_sector);
}

/**
 * Espera que o sector dest seja atingido ativando e esperando pelo
 * alarme correspendente.
 */
void disk_seek(sector_t dest){

	if(dest > csector && cdir == out)
		cdir = in;
	else if(dest < csector && cdir == in)
		cdir = out;

	// define temporizador
	struct itimerval val;
	struct itimerval old;
	signal(SIGALRM, incrementa_sector);

	val.it_interval.tv_sec = 0;
	val.it_interval.tv_usec = DELAYSECTOR;
	val.it_value.tv_sec = 0;
	val.it_value.tv_usec = DELAYSECTOR;

	setitimer(ITIMER_REAL, &val, &old);

	seekdest = dest;
	while(csector != seekdest)
		pause();

	setitimer(ITIMER_REAL, &old, NULL);
}


int main( int argc, char *argv[] ){
	char *progname = argv[0];
	char conffile[255];

	if( argc > 1 ){

		if(strcmp(argv[1], "-h") == 0){
			print_help(progname);
			exit(0);
		}
		else if(get_conf_name( argv, conffile ) == 0){
			print_help(progname);
			exit(1);
		}
	}
	else{
		print_help(progname);
		exit(1);
	}

	csector = 0;
	cdir = in;
	confname = argv[argc - 1];

	conf_t conf;
	confresult_t confresult = get_conf(conffile, confname, &conf);
	// verificação de erros de get_conf
	if(confresult != ok){
		if(confresult == filenotfound)
			fprintf(stderr, "ficheiro não encontrado\n");
		else if(confresult == confnotfound)
			fprintf(stderr, "configuração não encontrada\n");
		else if(confresult == disknotfound)
			fprintf(stderr, "disco não encontrado\n");
		else
			fprintf(stderr, "erro no ficheiro\n");

		exit(1);
	}

	// tamanho do disco
	struct stat st;
	if(stat(conf.imagefile, &st) == -1){
		perror("tamanho do disco");
		exit(1);
	}
	disksize = st.st_size;

	// verificar tamanho do ficheiro de imagem com número de
	// setores dos discos. Só é necessário verificar com um
	// disco porque a função get_conf verifica o tamanho dos discos.
	if(disksize != conf.disk[0].sectors){
		fprintf(stderr, "Não conseguio carregar o disco\n");
		exit(1);
	}

	// carregar para memória o ficheiro contendo imagem do disco
	int fd = open(conf.imagefile, O_RDONLY);
	if(fd == -1){
		perror(conf.imagefile);
		exit(1);
	}
	secdata_t disk[disksize];
	if(read (fd, disk, disksize) == -1){
		perror("ler ficheiro img");
		exit(1);
	}
	
	reqpool_t reqpool;
	ipc_t ipc;
	ipc.reqpool = &reqpool;

	// inicializar e associar o processo aos mecanismos de comunicação
	if(init_ipc(progname, confname, &ipc) == -1)
		exit(1);

	// criar o processo filho
	int indice = setup_disks(ipc.reqpool->disks);
	strcpy(diskname, conf.disk[indice].name);

	vsslog(confname,diskname, DS, cdir, disksize, ipc.reqpool->policy);
	vsslog(confname,diskname, ER, csector, cdir);
		
	// arma o sinal SIGUSR1
	signal(SIGUSR1, estaciona_disco);

	// atender pedidos
	request_t req;
	while(1){

		if(ipc.reqpool->policy == FCFS){
			if(take_request(&ipc, &req, select_request_FCFS) != 1){
				perror("carregar pedido");
				exit(1);
			}
		}else{
			if(take_request(&ipc, &req, select_request_LOOK) != 1){
				perror("carregar pedido");
				exit(1);
			}
		}
		vsslog(confname,diskname, OR, req.sector);

		secdata_t sect;
		if(move_and_read(&req, &sect,disk) == 0){
			perror("ler pedido");
			exit(1);
		}
		put_sector(csector, &sect);

		vsslog(confname,diskname, RS, csector, cdir);
		vsslog(confname,diskname, ER, csector, cdir);
	}

}
