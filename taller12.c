#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#define MAX 1000000

int numero_lineas(char *ruta, int *tam_lineas){
	if(ruta != NULL){
		FILE* ar = fopen(ruta, "r");
		int lineas =0;
		int tam_linea = 0;
		while(!feof(ar)){
			tam_linea++;
			char c = getc(ar);
			if(c == '\n'){
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

void * funcion_hilo(void *arg){
	
	estructura *argumentos = (estructura *)arg;		

	

	

}

typedef struct archivo_parc{
	char *ruta;
	char **palabras;
	int *num_palabras;
	int inicio;
	int fin;
} estructura;

int main (int argc, char *argv[]){

	char *ruta = argv[1];
	int num_hilos = atoi(argv[2]);
	int cant_palabras = argc - 3;
	char *palabras[100];
	int i;
	for(i = 0; i<cant_palabras; i++){
		palabras[i] = argv[i+3];
		
	}

	
	int num_palabras[100];
	int tam_lineas[MAX];

	int num_lineas = numero_lineas(ruta, tam_lineas);

	pthread_t hilos[num_hilos];

	int razon = num_lineas/num_hilos;

	int j, h;
	for (j=0;j<num_hilos;j++){
			estructura *s_hilo = malloc(sizeof(estructura));
			s_hilo -> ruta = ruta;
			s_hilo -> inicio=v;
			v += razon;
			s_hilo -> fin=v-1; 
			s_hilo -> palabras =  palabras;
			s_hilo -> num_palabras = num_palabras;
			int status=pthread_create(&hilos[j],NULL,funcion_hilo,(void*)s_hilo);
			if(status<0){
				fprintf(stderr,"Error al crear el hilo");
	}
		
}

	
	return(0);
}


