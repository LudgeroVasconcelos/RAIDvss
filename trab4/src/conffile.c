#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "raidvss.h"
#include "conffile.h"
#include "conffileutils.h"

/*
 * Carrega a configuração solicitada para a estrutura conf_t passada
 * como argumento. 
 * Retorno: em caso de erro, a estrutura conf não estará válida
 */
confresult_t get_conf(char* conffile,char* confname,conf_t* conf){
	int conf_found = 0;	// indica se configuração e discos foram encontrados
	int d1_found = 0;
	int d2_found = 0;

	conffileline_t confline;
	char line[255];

	FILE *file = fopen(conffile, "r");
	if(file == NULL){
		if(errno == ENOENT)
			return filenotfound;

		return fileerror;
	}
	
	// procurar configuração
	while(fgets(line, 255, file) != NULL){
		
		parseconfline(line, &confline);

		if(confline.stype == confl && strcmp(confname, confline.data.conf.name) == 0){
			// configuração encontrada, falta saber o nº de setores dos discos
			conf_found = 1;
			fseek(file, 0, SEEK_SET);

			diskconf_t disk[2];
			disk[0] = confline.data.conf.disk[0];
			disk[1] = confline.data.conf.disk[1];

			conffileline_t confline2;

			// procurar discos
			while(fgets(line, 255, file) != NULL){
				
				parseconfline(line, &confline2);

				// um dos discos foi encontrado
				if(confline2.stype == diskl &&
				strcmp(disk[0].name, confline2.data.disk.name) == 0){

					d1_found = 1;	
					disk[0].sectors = confline2.data.disk.sectors;
					confline.data.conf.disk[0].sectors = disk[0].sectors;
				}
				
				// um dos discos foi encontrado
				if(confline2.stype == diskl &&
				strcmp(disk[1].name, confline2.data.disk.name) == 0){

					d2_found = 1;		
					disk[1].sectors = confline2.data.disk.sectors;
					confline.data.conf.disk[1].sectors = disk[1].sectors;
				}
				
				// os dois discos foram encontrados
				if(d1_found && d2_found){

					// verifica se o número de setores dos discos é diferente
					if(disk[0].sectors != disk[1].sectors)
						return fileerror;

					copy_conf(conf, &confline.data.conf);
					return ok;
				}
			}

			// um dos discos nao foi encontrado
			if(!d1_found || !d2_found)
				return disknotfound;
		}
	}

	// configuração nao foi encontrada
	if(!conf_found)
		return confnotfound;
	else
		return fileerror;
}
