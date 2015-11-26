/**************************************************
 *
 * RAIDvss - Simulador Muito Simples de RAID 1
 *
 * so2013/2014
 *
 **************************************************/

#ifndef _CONFFILE_H
#define _CONFFILE_H

/**
 * Estrutura que define os parâmetros de configuração de um disco
 */
typedef struct {
  char  nome[255];
  int   sectores;
} diskconf_t;

/**
 * Estrura que define uma configuração completa
 */
typedef struct {
  char  nome[255];
  diskconf_t disk[2];
  char  fichimagem[255];
} conf_t;

/**
 * Valores possíveis de retorno da função get_conf
 */
typedef enum {
  ok,
  filenotfound,
  confnotfound,
  disknotfound
} confresult_t;

/*
 * Carrega a configuração solicitada para a estrutura conf_t passada
 * como argumento. 
 * Retorno: em caso de erro, a estrutura conf não estará válida
 */
confresult_t get_conf(char* conffile,char* confname,conf_t* conf);

#endif
