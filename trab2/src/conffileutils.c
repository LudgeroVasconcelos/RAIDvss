#include<stdio.h>
#include<string.h>
 
#include "conffileutils.h"
#include "raidvss.h"

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
 * Simplifica a análise da linha. Retorna um apontador para o primeiro
 * caracter de s que não seja um espaço ou tab. Termina a string no
 * caracter '#' (se existir) ou em '\n'
 */
char* chop(char* s)
{
	// Procura o caracter onde termina a string
	int i = 0;
	while( (s[i] != '\0') && (s[i] != '\n') && (s[i] != '#') )
	{
		i++;
	}
	s[i] = '\0';
	
	// Procura o primeiro caracter diferente de espaco ou tab
	int d = 0;
	while( (s[d] == ' ') || (s[d] == '\t') )
	{
		d++;
	}
	return (s + d);
}

/**
 * Preenche uma estrutura conffileline_t com o conteúdo da linha
 * recebida como argumento. stype será empty se a linha estiver vazia
 * ou for de comentário e error se a linha não respeitar a sintaxe do
 * ficheiro de configuração. Caso contrário ou uma das estruturas
 * diskconf_t ou conf_t será preenchida com os valores lidos na linha.
 * No caso de conf_t, apenas o nome dos discos será preenchido (não o
 * número de setores. Utiliza a função chop para simplificar a avaliação
 */ 
void parseconfline(char* line,conffileline_t* confline)
{
	char* cursor = chop(line);

	if(*cursor == '\0') 
	{
		confline->stype = emptyl;
		return;
	}
	
	char *token;
	token = strtok(cursor, " ");
	int nArg = 0;
	
	if(strcmp(token,"raidvss") == 0) 
	{
		nArg = 4;
	}	
	else if(strcmp(token,"disc") == 0) 
	{
		nArg = 2;
	}
	else
	{
	 confline->stype = errorf;
	 return;                   
	}

 	token = strtok(NULL," ");
	char arg[nArg][255];
		

	// conta argumentos e preenche array de argumentos
	int count=0;
	while (token != NULL) 
	{
		strcpy(arg[count],token);
		token = strtok(NULL," ");
         	count++;
	}


	if (nArg != count) 
	{
		confline->stype = errorf;
		return;
	}
	
	if (*cursor == 'd')
	{
		char *p;
		strtod(arg[nArg-1],&p);    // testa se 2 arg e numerico

		if(*p != '\0') {
			confline->stype = errorf;
			return;
		}

		confline->stype = diskl;
		strcpy(confline->data.disk.name , arg[0]);
		confline->data.disk.sectors = atoi(arg[1]);

	}
	else if (*cursor == 'r')
	{
		confline->stype = confl;
	 	strcpy(confline->data.conf.name         , arg[0]);
	 	strcpy(confline->data.conf.disk[0].name , arg[1]);
	 	strcpy(confline->data.conf.disk[1].name , arg[2]);
	 	strcpy(confline->data.conf.imagefile    , arg[3]);
	}	

}

/**
 * Recebe a lista de argumentos e copia para conffile o nome do
 * ficheiro de configuração.
 * Retorna 1 se foi utilizado o nome de ficheiro por omissão ou 2 se
 * existia o parâmetro -f e por isso foi usado o nome
 * especificado. Retorna 0 se existe um parâmetro -f mas é inválido
 * (último argumento) 
 */
int get_conf_name(char* argv[], char* conffile)
{
	int i = 0 , f_flag = 0;
	
	while(argv[i] != NULL){

		if(strcmp(argv[i],"-f") == 0)
		{			
			f_flag = 1;
			if(argv[i + 1] != NULL)
			{
				strcpy(conffile,argv[i + 1]); 
				return 2;
			}
		}
		
		i++;
	}

	strcpy(conffile,DEFCONFFILE); 
	return f_flag ? 0 : 1;
}

/**
 * Copia a configuração src para dest
 */
void copy_conf(conf_t* dest,conf_t* src)
{
	strcpy(dest->name , src->name);
	
	strcpy(dest->disk[0].name , src->disk[0].name);
	strcpy(dest->disk[1].name , src->disk[1].name);
	
	dest->disk[0].sectors = src->disk[0].sectors;
	dest->disk[1].sectors = src->disk[1].sectors;

	strcpy(dest->imagefile , src->imagefile);
}

