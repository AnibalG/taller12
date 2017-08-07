#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#define MAX 1000000

int cant_palabras;
int running_threads = 0;
int num_palabras[100];
char *palabras[100];
int tam_lineas[MAX];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;	

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

typedef struct archivo_parc{
	char *ruta;
	int bytes;
	int inicio;
	int fin;
} estructura;

void * funcion_hilo(void *arg){
	
	
	estructura *argumentos = (estructura *)arg;		
	FILE* fp = fopen(argumentos ->ruta,"r");
	
	char *saveptr;
	
	char * palabra;
	
	
	
	int n;
	pthread_mutex_lock(&mutex);
	fseek(fp, argumentos -> bytes, SEEK_SET);
	pthread_mutex_unlock(&mutex);
	for(n = argumentos ->inicio; n<=(argumentos ->fin);n++){
		
		char linea[tam_lineas[n]];
		fgets(linea,MAX,fp);

		char *newline = strchr( linea, '\n' );
		if ( newline )
			*newline = 0;	
		
		palabra = strtok_r(linea, ",.!?:; ",&saveptr);
		

		while(palabra != NULL){
			int o;
			
			
			for(o=0; o<cant_palabras;o++){
				char * comp = palabras[o];
				
				if (strcmp(palabra, comp) == 0){
					
					pthread_mutex_lock(&mutex);
					num_palabras[o] += 1;
					pthread_mutex_unlock(&mutex);
				}
			}
			palabra = strtok_r(NULL, " ,.!?:;",&saveptr);
			
		}
		
	}
	
	fclose(fp);
	running_threads--;
	return (void *)0;

}



int main (int argc, char *argv[]){

	char *ruta = argv[1];
	int num_hilos = atoi(argv[2]);
	cant_palabras = argc - 3;
	
	
	int i;
	for(i = 0; i<cant_palabras; i++){
		palabras[i] = argv[i+3];
		
	}

	
	
	

	int num_lineas = numero_lineas(ruta, tam_lineas);
	printf("%d\n",num_lineas);

	pthread_t hilos[num_hilos];

	int razon = num_lineas/num_hilos;

	int j, h, v;
	v=0;
	if (num_lineas % num_hilos == 0){
		for (j=0;j<num_hilos;j++){
			estructura *s_hilo = malloc(sizeof(estructura));
			s_hilo -> ruta = ruta;
			s_hilo -> inicio=v;
			for(int u = 0; u<v; u++){
				s_hilo -> bytes += tam_lineas[u];
			}
			v += razon;
			s_hilo -> fin=v-1; 
			
			running_threads++;
			int status = pthread_create(&hilos[j],NULL,funcion_hilo,(void*)s_hilo);
			
			if(status<0){
				fprintf(stderr,"Error al crear el hilo");
			}
		}
	}

	else{
		for (j=0;j<num_hilos;j++){  
			estructura *s_hilo = malloc(sizeof(estructura));
			s_hilo -> ruta = ruta;
			s_hilo -> inicio=v;
			for(int u = 0; u<v; u++){
					s_hilo -> bytes += tam_lineas[u];
				}
			v += razon;
			if(v>num_lineas){
				s_hilo -> fin=num_lineas-1;
			}else{
				s_hilo -> fin=v; 
			}
			v += 1;
			running_threads++;
			int status=pthread_create(&hilos[j],NULL,funcion_hilo,(void*)s_hilo);
			if(status<0){
				fprintf(stderr,"Error al crear el hilo");
			}
		
		}
}
	while(running_threads>0){
		sleep(1);
		for(int k=0; k<cant_palabras; k++){
			printf("%s: %d\t", palabras[k], num_palabras[k]);
		}
		printf("\n");
		
	}
	
	for (h=0;h<num_hilos;h++){
		
		
		pthread_join(hilos[h],NULL);
		

	}

	for(int k=0; k<cant_palabras; k++){
		printf("%s: %d\n", palabras[k], num_palabras[k]);
	}


	
	return(0);
}


