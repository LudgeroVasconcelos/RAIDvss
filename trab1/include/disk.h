/**************************************************
 *
 * RAIDvss - Simulador Muito Simples de RAID 1
 *
 * so2013/2014
 *
 **************************************************/

#ifndef _DISK_H
#define _DISK_H

#include "raidvss.h"

/**
 * Políticas possíveis de seleção de pedido.
 * FCFS (First Come First Served): trata os pedidos por ordem de
 * chegada e portanto utilizando a variável out da pool.
 * LOOK: Selecionado o pedido mais próximo na direção de movimento do
 * disco (preferencialmente) ou o pedido mais próximo na direção
 * inversa (caso não exista um na mesma direção.
 */
typedef enum {
  FCFS,
  LOOK
} policy_t;

/**
 * Variáveis globais que descrevem o estado do disco num determinado
 * instante.
 * csector: setor atual
 * cdir: direção atual
 * disksize: dimensão do disco (para verificar validade dos pedidos)
 */

extern sector_t csector;
extern direction_t cdir;
extern int disksize;

/**
 * Espera que o sector dest seja atingido ativando e esperando pelo
 * alarme correspendente.
 */
void disk_seek(sector_t dest);

#endif
