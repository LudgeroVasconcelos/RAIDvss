#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

#include "log.h"
#include "raidvss.h"
#include "diskutils.h"
#include "disk.h"


/**
 * Função que escreve a mensagem para o ficheiro de log. Aceita um 
 * número variável de argumentos, dependendo do tipo de mensagem de log.
 */
void vsslog(char* confname,char* diskname,logtype_t t,...){
	char filename[251];
	strcpy(filename, confname);
	strncat(filename, ".log", 4);

	FILE *flog = fopen(filename, "a");
	if(flog == NULL){
		perror("criar ficheiro log");
		exit(1);
	}

	// tempo no formato pretendido
	time_t tempo;
	time(&tempo);
	struct tm *vartm;
	vartm = localtime(&tempo);
	char data[20];
	strftime(data, 20, "%Y%m%d %H:%M:%S ", vartm);
	fprintf(flog, "%s", data);

	// enum to string
	char* logtype[] = {"DS", "ER", "OR", "RS", "P"};

	// nome do disco e tipo
	fprintf(flog, "%s %s", diskname, logtype[t]);

	//obter os restantes argumentos
	va_list arg;
	va_start(arg, t);

	switch ( t ){
		case DS:
			{
			direction_t dir = va_arg(arg, direction_t);
			int disksize = va_arg(arg, int);
			policy_t poli = va_arg(arg, policy_t);
			char* politica[] = {"FCFS", "LOOK"};

			fprintf(flog, " Início do disco direção: %s, dimensão: %d"
			", política: %s\n", dir==in?"in":"out", disksize, politica[poli]);
			break;
			}

		case ER:
			{
			sector_t sector = va_arg(arg, sector_t);
			direction_t dir = va_arg(arg, direction_t);

			fprintf(flog, " Espera pedido sector actual: %u,"
			" direção: %s\n", sector, dir==in?"in":"out");
			break;
			}

		case OR:
			{
			sector_t sector = va_arg(arg, sector_t);
			fprintf(flog, " Obteve pedido sector: %u\n", sector);
			break;
			}

		case RS:
			{
			sector_t sector = va_arg(arg, sector_t);
			direction_t dir = va_arg(arg, direction_t);
			fprintf(flog, " Leu sector actual: %u, direção:"
			" %s\n", sector, dir==in?"in":"out");
			break;
			}

		case P:
			fprintf(flog, "  Disco inicializado \n");
	}
			
	fclose(flog);
}
