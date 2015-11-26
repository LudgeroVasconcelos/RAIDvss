#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include "raidvss.h"
#include "request.h"
#include "conffileutils.h"
#include "conffile.h"
#include "ipc.h"


void showHelp(char *progname){

	printf("Utilização:\n"
	"%s -h : Esta informação de ajuda\n"
	"%s -c configuracao : Limpa os mecanismos de IPC para a configuração indicada\n"
	"%s [-f ficheiro] configuracao policy: Executa o controlador com a configuração e na política indicada\n"
	"-f ficheiro: Utiliza o ficheiro de configuração indicado. Por omissão: config.vss\n"
	"politica: Um dos valores LOOK ou FCFS\n", progname, progname, progname);
}


int main( int argc, char *argv[] ){
	char *progname = argv[0];
	char conffile[255];
	char *confname;
	char *politica;

	if(argc > 1){

		if(strcmp(argv[1], "-h") == 0){
			showHelp(progname);
			exit(0);
		}
		else if(strcmp(argv[1], "-c") == 0){
			// eliminar mecanismos de comunicação entre processos
			if(argc > 2)
				clean_ipc(argv[2]);
			else
				showHelp(progname);

			exit(1);
		}
		else if(get_conf_name(argv, conffile) == 0){
			showHelp(progname);
			exit(1);
		}
	}
	else{
		showHelp(progname);
		exit(1);
	}
	politica = argv[argc - 1];
	confname = argv[argc - 2];

	// verificação da política
	policy_t policy;
	if(strcmp(politica, "LOOK") == 0)
		policy = LOOK;
	else if(strcmp(politica, "FCFS") == 0)
		policy = FCFS;
	else{
		showHelp(progname);
		exit(1);
	}

	// verificação da configuração
	conf_t conf;
	confresult_t confresult = get_conf(conffile, confname, &conf);

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
	
	reqpool_t reqpool;
	ipc_t ipc;
	ipc.reqpool = &reqpool;

	//inicializar e associar o processo aos mecanismos de comunicação
	init_ipc(progname, confname, &ipc);
		
	ipc.reqpool->policy = policy;

	printf("%% ");
	// ler os pedidos
	char c;
	while((c = getchar()) != EOF){

		switch( c ){
			case 'r' : 
			{
				sector_t sector;
				
				int n = scanf("%u", &sector);

				if(n == 0){
					printf("Bronco\n");
					break;
				}

				request_t req;
				fill_request( &req, sector );
				put_request( &ipc, &req );

				break;
			}

			case 'p' : park_disks(ipc.reqpool->disks); break;

			default : printf("Bronco\n");
		}
		while(getchar() != '\n');
		printf("%% ");
	}
	
	return 0;
}

