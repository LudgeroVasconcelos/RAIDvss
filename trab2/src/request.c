#include <stdlib.h>
#include <string.h>

#include "request.h"

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
 * Copia um pedido da origem para o destino 
 */
void copy_request(request_t* origem,request_t* destino){
	destino -> status = origem -> status;
	destino -> sector = origem -> sector;
}

/**
 * Limpa um pedido 
 */
void clean_request(request_t* req){
	req -> status = empty;
}

/**
 * Inicializa e preenche a estrutura pedido 
 */
void fill_request(request_t* req,sector_t sector){
	req -> status = waiting;
	req -> sector = sector;
}
