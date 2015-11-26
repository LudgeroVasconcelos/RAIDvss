/**************************************************
 *
 * RAIDvss - Simulador Muito Simples de RAID 1
 *
 * so2013/2014
 *
 **************************************************/

#ifndef _CONFFILEUTILS_H
#define _CONFFILEUTILS_H

/**
 * Retorna um apontador para o primeiro caracter de s que não seja um espaço ou tab.
 */
char* chop(char* s);

/**
 * Recebe a lista de argumentos e copia para conffile o nome do
 * ficheiro de configuração.
 * Retorna 1 se foi utilizado o nome de ficheiro por omissão ou 2 se
 * existia o parâmetro -f e por isso foi usado o nome
 * especificado. Retorna 0 se existe um parâmetro -f mas é inválido
 * (último argumento) 
 */
int get_conf_name(char* argv[], char* conffile);

#endif
