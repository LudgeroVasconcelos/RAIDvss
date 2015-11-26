#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "ipc.h"


/***************************************
#	Sistemas Operativos 2013/2014		
#							
#	Grupo:			so003
#	Diogo Pinto		Nº 43299
#	Ludgero de Vasconcelos	Nº 43259
#	Pedro Cerca		Nº 44030
#							
****************************************/


/**
 * Função auxiliar que, dada uma das constantes de semáforos e memória
 * partilhada (definidas acima) e o nome da configuração retorna em ipcname uma
 * string com o nome a atribuir ao mecanismo de memória partilhada. O formato
 * do nome é deixado ao critério dos grupos. A variável upcname terá que
 * ser criada com espaço suficiente para conter o nome definido.
 */
void make_ipc_name(char* tipo,char* confname,char* ipcname)
{
	strcpy(ipcname,tipo);
	strcat(ipcname,"_");
	strcat(ipcname,confname);
}

/**
 * Inicializa a estrutura ipc, que permite ao processo comunicar pela zona
 * de memória partilhada. A inicialização consiste:
 * - na criação (se necessário), a associação e inicialização (se
 * criada) da zona de memória partilhada: função init_reqpool
 * - na criação (se necessário) e associação aos semáforos que a
 * controlam: função init_sems
 * A identificação dos semáforos e da zona de memória partilhada deve
 * combinar a constante respetiva (definida acima) e o nome da configuração
 * Retorna -1 em caso de erro e imprime para o stderr uma mensagem
 * explicativa. 
 */
int init_ipc(char* progname,char* confname,ipc_t* ipc)
{
	int ret;
	ret = init_reqpool(ipc,confname);
	if(ret == -1) 
	{ 
		perror("init_ipc");
 		return -1; 
	}
	ret = init_sems(ipc,confname);
	if(ret == -1) 
	{ 
		perror("init_ipc");
		return -1;
	}
}

/**
 * Associa o processo à zona de memória partilhada, criando-a se
 * necessário. Se for também criada então é inicializada através da
 * invocação à função init_pool
 * Retorna -1 em caso de erro ou 1 caso contrário
 */
int init_reqpool(ipc_t* ipc,char* confname)
{
	int criado = 0;
	char ipcname[251];
	make_ipc_name(POOLNAME,confname,ipcname);
	
	ipc -> shmfd = shm_open(ipcname, O_RDWR, S_IRUSR|S_IWUSR);
	if (ipc -> shmfd == -1){

		if(errno == ENOENT){
			ipc -> shmfd = shm_open(ipcname, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);
			if (ipc -> shmfd == -1) return -1;
			if(ftruncate(ipc -> shmfd, sizeof(reqpool_t)) == -1) return -1;
			criado = 1;
		}
		else 
			return -1;
	}
	ipc -> reqpool = mmap(0, sizeof(reqpool_t), PROT_READ|PROT_WRITE,MAP_SHARED, ipc -> shmfd, 0);
	if(ipc -> reqpool == MAP_FAILED) {perror("mmap failed"); return -1; }

	if(criado == 1)
		init_pool(ipc-> reqpool);
	
	return 1;
}

/**
 * Inicializa todos os campos da zona de memória partilhada. Invoca a
 * função clean_request para cada pedido da zona da fila. O array
 * disks é inicializado com o valor -1
 */
void init_pool(reqpool_t* reqpool)
{
	int i=0;
	for(i; i < POOLSIZE; ++i)
		clean_request(&reqpool -> queue[i]);

	reqpool -> disks[0] = -1;
	reqpool -> disks[1] = -1;
}

/**
 * Cria os semáforos necessários à sincronização entre os processos
 * de acordo com o modelo produtor/consumidor. Retorna -1 em caso de
 * erro.
 */
int init_sems(ipc_t* ipc,char* confname)
{
	char ipcname[251];

	make_ipc_name(SMUTEXINAME,confname,ipcname);

	ipc -> s_mutexi = sem_open(ipcname, O_CREAT, 0xFFFFFFFF, 1);
	if(ipc -> s_mutexi == SEM_FAILED) return -1;

	make_ipc_name(SMUTEXONAME,confname,ipcname);	
	ipc -> s_mutexo = sem_open(ipcname, O_CREAT, 0xFFFFFFFF, 1);
	if(ipc -> s_mutexo == SEM_FAILED) return -1;
	
	make_ipc_name(SEMPTYNAME,confname,ipcname);	
	ipc -> s_empty = sem_open(ipcname, O_CREAT, 0xFFFFFFFF, POOLSIZE);
	if(ipc -> s_empty == SEM_FAILED) return -1;

	make_ipc_name(SFULLNAME,confname,ipcname);
	ipc -> s_full = sem_open(ipcname, O_CREAT, 0xFFFFFFFF, 0);
	if(ipc -> s_full == SEM_FAILED) return -1;
}


/**
 * Liberta os recursos associados à comunicação entre processos
 * invocando as funções de terminação dos semáforos e da memória
 * partilhada, passando a NULL reqpool e os apontadores para os
 * semáforos e a -1 shmfd. Retorna -1 em caso de erro. 
 */
int end_ipc(ipc_t* ipc)
{
	ipc -> shmfd = -1;
	end_reqpool(ipc);
	end_sems(ipc);
}
int end_reqpool(ipc_t* ipc)
{
	if(munmap(ipc -> reqpool , sizeof(reqpool_t)) == -1) return -1;
	ipc -> reqpool = NULL;
}
int end_sems(ipc_t* ipc)
{
	if(sem_close(ipc -> s_mutexi) == -1) return -1;
	if(sem_close(ipc -> s_mutexo) == -1) return -1;
	if(sem_close(ipc -> s_empty)  == -1) return -1;
	if(sem_close(ipc -> s_full)   == -1) return -1;
}

/**
 * Elimina do sistema os semáforos e memória partilhada necessários à
 * comunicação entre processos. Retorna -1 em caso de erro.
 */
int clean_ipc(char* confname)
{
	char ipcname[251];
	
	make_ipc_name(SMUTEXINAME,confname,ipcname);
	if(sem_unlink(ipcname) == -1) return -1;

	make_ipc_name(SMUTEXONAME,confname,ipcname);
	if(sem_unlink(ipcname) == -1) return -1;

	make_ipc_name(SEMPTYNAME,confname,ipcname);
	if(sem_unlink(ipcname) == -1) return -1;

	make_ipc_name(SFULLNAME,confname,ipcname);
	if(sem_unlink(ipcname) == -1) return -1;

	make_ipc_name(POOLNAME,confname,ipcname);
	if(shm_unlink(ipcname) == -1) return -1;
}

/**
 * Põe req na fila de pedidos a resolver. Implementa os mecanismos
 * de sincronização. Utiliza copy_request para colocar o pedido
 * req na fila de pedidos. 
 * Idealmente coloca o pedido no fim da fila circular, indicado pelo
 * campo in da estrutura, excepto se essa posição estiver ocupada
 * (pode acontecer se os discos estiverem a usar LOOK). Nesse caso
 * procura uma qualquer posição livre. Deixa sempre o apontador de fim
 * da fila a apontar para a posição seguinte daquela em que escreveu
 * (não esquecer que é uma lista circular).
 * Retorna -1 em caso de erro e 1 em caso de sucesso. A função deve
 * tolerar a interrupção por sinais sem com isso deixar de colocar o
 * problema.
 */
int put_request(ipc_t* ipc,request_t* req)
{
	int index = ipc -> reqpool -> in;

	if (sem_wait (ipc -> s_empty)  == -1){

		if(errno == EINTR) 
			return 0;

		return -1;
	}

	if (sem_wait (ipc -> s_mutexi) == -1){

		if(errno == EINTR){

			if (sem_post (ipc -> s_empty) == -1)
				return -1;
			
			return 0;
		}
		return -1;
	}
	block_sig(SIGINT);

	while(ipc -> reqpool -> queue[index].status != empty)
		index = (index + 1) % POOLSIZE;

	copy_request(req, &ipc -> reqpool -> queue[index]);

	index = (index + 1) % POOLSIZE;
	ipc -> reqpool -> in = index;
	
	if (sem_post (ipc -> s_mutexi) == -1) return -1;
	if (sem_post (ipc -> s_full)   == -1) return -1;
	resume_sig(SIGINT);

	return 1;
}

/**
 * Carrega um pedido, retirando-o da fila de pedidos da memória
 * partilhada colocando-o em req.
 * A seleção do pedido é feita pela função select_request a quem esta
 * função passa a pool de pedidos. 
 * Implementa os mecanismos de sincronização.
 * Se a política for FCFS, avança o apontador de início da fila 
 * Retorna -1 em caso de erro, 1 em caso de sucesso e 0 se a espera
 * nos semáforos foi interrompida pela recepção de um qualquer sinal.
 * A interrupção deve deixar a fila e os semáforos no estado original.
 * Se a interrupção for recebida fora do bloqueio dos semáforos,
 * a função deve carregar o problema e retornar 1.
 */
int take_request(ipc_t* ipc,request_t* req,int(*selector)(request_t*,int))
{
	policy_t politica = ipc -> reqpool -> policy;

	if (sem_wait (ipc -> s_full) == -1)
	{	
		if(errno == EINTR)
			return 0;

		return -1;
	}

	if (sem_wait (ipc -> s_mutexo) == -1)
	{ 
		if(errno == EINTR) 
		{ 
			if (sem_post (ipc -> s_full) == -1)
				return -1;

			return 0; 
		}
		return -1; 
	}

	block_sig(SIGINT);
	int index = selector(ipc-> reqpool -> queue, ipc -> reqpool -> out);

	copy_request(&ipc -> reqpool -> queue[index], req);
	clean_request(&ipc -> reqpool -> queue[index]);

	if(politica == FCFS)
		ipc -> reqpool -> out = (ipc -> reqpool -> out + 1) % POOLSIZE;
	
	if (sem_post (ipc -> s_mutexo) == -1) return -1;
	if (sem_post (ipc -> s_empty)  == -1) return -1;
	resume_sig(SIGINT);

	return 1;
}

/**
 * Regista a função func que deverá ser invocada quando for recebido o
 * sinal passado como argumento, sem bloquear outros sinais. Retorna o
 * valor retornado por sigaction.
 */
int set_sig_handler(int sig,void (*func)(int))
{
	struct sigaction sa; 
	sa.sa_handler = func;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);

	return sigaction (sig, &sa, NULL);
}

/**
 * Suspende a entrega do sinal passado como argumento. Retorna -1 em
 * caso de erro e imprime para o stderr uma mensagem explicativa.
 */
int block_sig(int sig)
{
	sigset_t new;
	sigemptyset(&new);
	sigaddset(&new,sig);

	if(sigprocmask(SIG_BLOCK,&new,NULL) == -1)
	{
		perror("block_sig");
		return -1; 
	}
}

/**
 * Desbloqueia a entrega do sinal passado como argumento. Retorna -1 em
 * caso de erro e imprime para o stderr uma mensagem explicativa.
 */
int resume_sig(int sig)
{
	sigset_t new;
	sigemptyset(&new);
	sigaddset(&new,sig);

	if(sigprocmask(SIG_UNBLOCK,&new,NULL) == -1)
	{
		perror("resume_sig");
		return -1; 
	}
}

/**
 * Envia um USR1 aos discos, que será interpretada pelos discos como
 * uma deslocação imediata e instantânia para a posição de park
 * (sector 0).
 */
void park_disks(pid_t disks[])
{
	kill(disks[0],SIGUSR1);
	kill(disks[1],SIGUSR1);
}

/**
 * Gera um segundo processo, completando assim a criação dos discos
 * para o simulador. Preenche os identificadores dos processos no
 * array recebido como argumento e retorna o índice no vector do
 * identificador do processo respetivo. Em caso de erro imprime uma
 * mensagem para o stderr e termina.
 * Exemplo: se o pid 12456 foi escrito em disks[0], o processo com o
 * pid 12456 retorna 0.
 */
int setup_disks(pid_t disks[])
{
	int pid;
	if((pid = fork()) == -1) 
	{
		perror("setup_disks\n");
		exit(1);	
	}

	if(pid == 0) // processo filho
	{
		disks[0] = getpid();
		return 0;
	}

	else // processo pai
	{
		disks[1] = getpid();
		return 1;
	}

}
