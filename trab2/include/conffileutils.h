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
 * Estrutura que define os parâmetros de configuração de um disco
 */
typedef struct {
  char  name[255];
  int   sectors;
} diskconf_t;

/**
 * Estrura que define uma configuração completa
 */
typedef struct {
  char  name[255];
  diskconf_t disk[2];
  char  imagefile[255];
} conf_t;

/**
 * Estrutura retornada pela função parseconfline e que é preenchida de
 * acordo com a linha lida 
 */
typedef struct {
  enum {diskl,confl,emptyl,errorf} stype;
  union {
    diskconf_t disk;
    conf_t     conf;
  } data;
} conffileline_t;

/**
 * Simplifica a análise da linha. Retorna um apontador para o primeiro
 * caracter de s que não seja um espaço ou tab. Termina a string no
 * caracter '#' (se existir) ou em '\n'
 */
char* chop(char* s);

/**
 * Preenche uma estrutura conffileline_t com o conteúdo da linha
 * recebida como argumento. stype será empty se a linha estiver vazia
 * ou for de comentário e error se a linha não respeitar a sintaxe do
 * ficheiro de configuração. Caso contrário ou uma das estruturas
 * diskconf_t ou conf_t será preenchida com os valores lidos na linha.
 * No caso de conf_t, apenas o nome dos discos será preenchido (não o
 * número de setores. Utiliza a função chop para simplificar a avaliação
 */ 
void parseconfline(char* line,conffileline_t* confline);

/**
 * Recebe a lista de argumentos e copia para conffile o nome do
 * ficheiro de configuração.
 * Retorna 1 se foi utilizado o nome de ficheiro por omissão ou 2 se
 * existia o parâmetro -f e por isso foi usado o nome
 * especificado. Retorna 0 se existe um parâmetro -f mas é inválido
 * (último argumento) 
 */
int get_conf_name(char* argv[], char* conffile);

/**
 * Copia a configuração src para dest
 */
void copy_conf(conf_t* dest,conf_t* src);

#endif
