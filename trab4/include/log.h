/**************************************************
 *
 * RAIDvss - Simulador Muito Simples de RAID 1
 *
 * so2013/2014
 *
 **************************************************/

#ifndef _LOG_H
#define _LOG_H

/* Tipos das mensagens de erro. Indica-se entre parêntesis o tipo e 
   significado dos argumentos extra (caso existam) */
typedef enum {
  DS,  // Inicialização do disco (direction_t: direction,int disksize,policy_t policy)
  ER,  // Espera pedido (sector_t csector,direction_t direction)
  OR,  // Obtido pedido (sector_t sector)
  RS,  // Sector lido (sector_t nº sector,direction_t direction)
  P,   // Parked
} logtype_t;

/**
 * Função que escreve a mensagem para o ficheiro de log. Aceita um 
 * número variável de argumentos, dependendo do tipo de mensagem de log.
 */
void vsslog(char* confname,char* diskname,logtype_t t,...);

#endif
