/**************************************************
 *
 * RAIDvss - Simulador Muito Simples de RAID 1
 *
 * so2013/2014
 *
 **************************************************/

#ifndef IPC_H
#define IPC_H

#include <semaphore.h>
#include <sys/types.h>
#include <unistd.h>

#include "request.h"

#define POOLSIZE 50

/**
 * Constantes que definem os nomes dos mecanismos de IPC. Os grupos
 * são livres de alterar estas constantes por forma a que não
 * colidam com os nomes utilizados por outros grupos (requer
 * recompilação e portanto acesso ao código fonte).
 */
#define POOLNAME "/P"
#define SMUTEXINAME "/muti"
#define SMUTEXONAME "/muto"
#define SEMPTYNAME "/empt"
#define SFULLNAME "/full"

/**
 * Estrutura mantida na zona de memória partilhada. Existe apenas
 * uma destas estruturas, partilhada por todos os processos.
 */
typedef struct {
  pid_t disks[2];             /* PID dos discos associados */
  request_t queue[POOLSIZE];  /* pool de pedidos */
  policy_t policy;            /* política de atendimento de pedidos */
  unsigned short int in,out;  /* Cabeça e cauda da lista circular (se modo FCFS) */
} reqpool_t;

/**
 * Estrutura mantida por cada processo e que regula o acesso à zona de
 * memória partilhada.
 */
typedef struct {
  reqpool_t* reqpool;         /* apontador para a zona de memória partilhada*/
  int    shmfd;               /* shared memory file descriptor */
  sem_t* s_mutexi;            /* mutex para controlo de acesso à variável 
				 in (ver produtor/consumidor) */
  sem_t* s_mutexo;            /* mutex para controlo de acesso à variável
				 out (ver produtor/consumidor) */
  sem_t* s_empty;             /* semáforo empty (ver produtor/consumidor) */
  sem_t* s_full;              /* semáforo full  (ver produtor/consumidor) */
} ipc_t;

/**
 * Função auxiliar que, dada uma das constantes de semáforos e memória
 * partilhada (definidas acima) e o nome da configuração retorna em ipcname uma
 * string com o nome a atribuir ao mecanismo de memória partilhada. O formato
 * do nome é deixado ao critério dos grupos. A variável upcname terá que
 * ser criada com espaço suficiente para conter o nome definido.
 */
void make_ipc_name(char* tipo,char* confname,char* ipcname);

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
int init_ipc(char* progname,char* confname,ipc_t* ipc);

/**
 * Associa o processo à zona de memória partilhada, criando-a se
 * necessário. Se for também criada então é inicializada através da
 * invocação à função init_pool
 * Retorna -1 em caso de erro ou 1 caso contrário
 */

int init_reqpool(ipc_t* ipc,char* confname); 

/**
 * Inicializa todos os campos da zona de memória partilhada. Invoca a
 * função clean_request para cada pedido da zona da fila. O array
 * disks é inicializado com o valor -1
 */

void init_pool(reqpool_t* reqpool);

/**
 * Cria os semáforos necessários à sincronização entre os processos
 * de acordo com o modelo produtor/consumidor. Retorna -1 em caso de
 * erro.
 */

int init_sems(ipc_t* ipc,char* confname);


/**
 * Liberta os recursos associados à comunicação entre processos
 * invocando as funções de terminação dos semáforos e da memória
 * partilhada, passando a NULL reqpool e os apontadores para os
 * semáforos e a -1 shmfd. Retorna -1 em caso de erro. 
 */

int end_ipc(ipc_t* ipc);

/**
 * Funções auxiliares invocadas por end_ipc
 */
int end_reqpool(ipc_t* ipc);
int end_sems(ipc_t* ipc);

/**
 * Elimina do sistema operativo os semáforos e memória partilhada 
 * necessários à comunicação entre processos. Retorna -1 em caso de 
 * erro.
 */
int clean_ipc(char* confname);

/**
 * Põe req na fila de pedidos a resolver. Implementa os mecanismos
 * de sincronização. Utiliza copy_request para colocar o pedido
 * req na fila de pedidos. 
 * Idealmente coloca o pedido no fim da fila circular, indicado pelo
 * campo in da estrutura, excepto se essa posição estiver ocupada
 * (pode acontecer se os discos estiverem a usar LOOK). Nesse caso
 * procura uma qualquer posição livre. Deixa sempre o apontador de fim
 * da fila a apontar para a posição seguinte aquela em que escreveu
 * mesmo que a posição esteja ocupada (não esquecer que é uma lista 
 * circular).
 * Retorna -1 em caso de erro, 1 em caso de sucesso e 0 se a espera
 * nos semáforos foi interrompida pela recepção de um qualquer sinal.
 * A interrupção deve deixar a fila e os semáforos no estado original.
 * Se a interrupção for recebida fora do bloqueio dos semáforos,
 * a função deve carregar o problema e retornar 1.
 */
int put_request(ipc_t* ipc,request_t* req);

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
int take_request(ipc_t* ipc,request_t* req,int(*selector)(request_t*,int));


/**
 * Regista a função func que deverá ser invocada quando for recebido o
 * sinal passado como argumento, sem bloquear outros sinais. Retorna o
 * valor retornado por sigaction.
 */
int set_sig_handler(int sig,void (*func)(int));

/**
 * Suspende a entrega do sinal passado como argumento. Retorna -1 em
 * caso de erro e imprime para o stderr uma mensagem explicativa.
 */
int block_sig(int sig);

/**
 * Desbloqueia a entrega do sinal passado como argumento. Retorna -1 em
 * caso de erro e imprime para o stderr uma mensagem explicativa.
 */
int resume_sig(int sig);

/**
 * Envia um USR1 aos discos, que será interpretada pelos discos como
 * uma deslocação imediata e instantânia para a posição de park
 * (sector 0).
 */
void park_disks(pid_t disks[]);

/**
 * Gera um segundo processo, completando assim a criação dos discos
 * para o simulador. Preenche os identificadores dos processos no
 * array recebido como argumento e retorna o índice no vector do
 * identificador do processo respetivo. Em caso de erro imprime uma
 * mensagem para o stderr e termina.
 * Exemplo: se o pid 12456 foi escrito em disks[0], o processo com o
 * pid 12456 retorna 0.
 */
int setup_disks(pid_t disks[]);

#endif
