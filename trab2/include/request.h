/**************************************************
 *
 * RAIDvss - Simulador Muito Simples de RAID 1
 *
 * so2013/2014
 *
 **************************************************/

#ifndef _REQUEST_H
#define _REQUEST_H

#include "raidvss.h"

/**
 * Estrutura que descreve o estado de um pedido
 */
typedef enum {
  empty,
  waiting,
} reqstat_t;

/**
 * Descrição de um pedido
 */
typedef struct {
  reqstat_t status;
  sector_t  sector;
} request_t;


/**
 * Copia um pedido da origem para o destino 
 */
void copy_request(request_t* origem,request_t* destino);

/**
 * Limpa um pedido 
 */
void clean_request(request_t*);

/**
 * Inicializa e preenche a estrutura pedido 
 */
void fill_request(request_t* req,sector_t sector);

#endif
