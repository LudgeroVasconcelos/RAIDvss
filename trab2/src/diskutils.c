#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "diskutils.h"
#include "ipc.h"
#include "disk.h"

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
 * Escolhe o próximo pedido a tratar pelo disco na politica LOOK
 * Invocado por select_request
 * Retorna o indice do pedido selecionado
 */
int select_request_LOOK(request_t* pool,int out){
		
	int min = disksize + 1;		// guarda menor sector maior que csector (dir in)
	int max = -1;			// guarda maior sector menor que csector (dir out)
	int index_min, index_max;	// indices desses sectores na lista
	
	index_min = index_max = -1;

	// percorre a lista toda e guarda os indices dos sectores a devolver,
	// independentemente se a direcao é in ou out
	int i;
	for(i = 0; i < POOLSIZE; ++i){
		if(pool[i].status == waiting){

			int sector = pool[i].sector;
			if(sector > csector && sector < min){	// in
				min = sector;
				index_min = i;		
			}
			else if(sector < csector && sector > max){	// out
				max = sector;
				index_max = i;
			}
		}
	}
	// se nao encontrou nenhum setor, é porque o setor é o mesmo (csector)
	if(index_min == -1 && index_max == -1)
		return out;

	if(index_min == -1)
		return index_max;
	else if(index_max = -1)
		return index_min;
	return cdir == in ? index_min : index_max;
}

/**
 * Escolhe o próximo pedido a tratar pelo disco na politica FCFS
 * Invocado por select_request
 * Retorna o indice do pedido selecionado
 */
int select_request_FCFS(request_t* pool,int out){

	return out;
}

/**
 * Copia o sector corrente (dado pela variável global csector) do
 * disco (representado por disk) para o argumento sect.
 * Se o sector corrente exceder a dimensão do disco, retorna 0 e
 * posiciona o disco no sector 0. Caso contrário retorna 1
 */
int disk_read_sector(secdata_t* sect,void* disk){

	if (csector >= disksize){
		// sector corrente excedeu a dimensão do disco
		csector = 0;
		return 0;
	}

	secdata_t *p = disk;
	*sect = p[csector];
	return 1;
}


/**
 * Copia para sect o sector solicitado em request. Invoca em sequência
 * as funções disk_seek e disk_read_sector para o fazer.
 * Retorna 0 em caso de erro e 1 caso contrário.
 */
int move_and_read(request_t* request,secdata_t* sect,void* disk){

	disk_seek(request->sector);

	return disk_read_sector(sect, disk);
}

/**
 * Imprime informação de utilização para o stderr
 */
void print_help(char* progname){

	printf("%s [-f <ficheiro configuracao>] configuracao disco\n", progname);
	printf("%s -h\n", progname);
}

/**
 * Imprime o conteúdo do sector para o stdout no formato <nº do sector>:<conteúdo>
 */
void put_sector(sector_t sector,secdata_t* secdata){

	printf("%u:%c\n", sector, *secdata);
}

