/**************************************************
 *
 * RAIDvss - Simulador Muito Simples de RAID 1
 *
 * so2013/2014
 *
 **************************************************/

#ifndef _VSS_H
#define _VSS_H

#define DEFCONFFILE "config.vss"

/**
 * Definição do tipo sector_t que é utilizado na identificação dos
 * sectores do disco
 */
typedef unsigned int sector_t;

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


#endif
