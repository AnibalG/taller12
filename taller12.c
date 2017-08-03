#include <stdio.h>
#include <pthread.h>
#include <String.h>

int numero_lineas(char *ruta, int *tam_lineas){
	if(ruta != NULL){
		FILE* ar = fopen(ruta, "r");
		int lineas =0;
		int tam_linea;
		while(!feof(ar)){
			tam_linea++;
			char c = getc(ar);
			if(c == "\n"){
				if(tam_lineas != NULL){
					tam_lineas[lineas] = tam_linea;
				}
			lineas++;
			tam_linea = 0;
			}
		}
		fclose(ar);
		return lineas;
	}
	return -1;
}


