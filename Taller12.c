#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>

void Funcion_Error(int nHilos, FILE *fp,int numLns);
int numero_lineas(char *ruta, int *tam_lineas);
void * countWords(void * datosNecesarios);
void * printEstado(void * arg);

struct structuraHilo{
  FILE *fp;
  int InicioLeer;
  int lineaIn;
  int *tam_lineas;
  int lineaFin;

};


#define MAXnumLNS 10000000
#define numWordsToFind 100

int numero_Caracteres = 0;
int numero_Palabras = 0;

char** palabras;
int num_palabras[numWordsToFind];//arreglo

pthread_mutex_t mutex;

int main(int argc, char** argv)
{

  if( argc<4 ){
    Funcion_Error(1,(void*)1,0);
    return -1;
  }
else{


    int nHilos = atoi(argv[2]);
    char *ruta = argv[1];

    FILE *fp = fopen(ruta,"r");
    if(fp==NULL || nHilos<=0)
    {
  
      Funcion_Error(nHilos,fp,0);
      return -1;
    }



    numero_Palabras = argc - 3;

    palabras=(char**)malloc(sizeof(char*)*numero_Palabras);
    for(int i = 0; i<numero_Palabras; i++){ 
      palabras[i]=argv[i+3];
      num_palabras[i]=0;   	
    }                       
      
 
	int *tam_lineas = (int*)malloc(sizeof(int)*MAXnumLNS);
	int numeroDeLineas = numero_lineas(ruta,tam_lineas);
	if(numeroDeLineas>MAXnumLNS){      
	  Funcion_Error(1,(void*)1,1);
	  return -1;
	}


    int numCharDivisible = numero_Caracteres;
    while((numCharDivisible % nHilos)!=0)
      numCharDivisible -= 1;
    int charxHilo = numCharDivisible/nHilos;


    pthread_t * idHilos = (pthread_t *)malloc(sizeof(pthread_t) * nHilos);


    if (pthread_mutex_init(&mutex, NULL) != 0){
        printf("\nInicializacion de mutex fallida\n");
        return -1;
    }
//Inicio de los parametros, Es necesario tener un contador de todo

    int lineaInicio = 0;
    int lnItr=0;
    int inicioLeer=0;
    int caracteres=0;
    int hilosCreados=0;

    for (int xHilo=0; xHilo<nHilos && caracteres<numero_Caracteres ; xHilo++){	

      struct structuraHilo *params = (struct structuraHilo*)malloc(sizeof(struct structuraHilo));

      int charHiloActual = 0;       
      do{
        charHiloActual += tam_lineas[lnItr];
        caracteres += tam_lineas[lnItr];  
        lnItr++;
      }
	while( (charHiloActual+tam_lineas[lnItr]) < charxHilo );
      params->fp=fp;
      params->InicioLeer=inicioLeer;
      params->lineaIn=lineaInicio;
      if ( xHilo == nHilos-1 )
        params->lineaFin = numeroDeLineas-1;
      else
      	params->lineaFin = lnItr-1;
      params->tam_lineas=tam_lineas;

    
      if ((pthread_create(&(idHilos[xHilo]), NULL, countWords, (void *)params)<0)){
        printf("Algo salio mal con la creacion del hilo\n");
        return -1;
      }

  
      lineaInicio=lnItr;
      inicioLeer=caracteres;
  
      hilosCreados++;
      if(lnItr == (numeroDeLineas -1) || lineaInicio == (numeroDeLineas-1))
      	break;
    }

    pthread_t idHiloImpresion;
    if (pthread_create(&idHiloImpresion, NULL, printEstado, NULL)<0)
    {
      printf("Algo salio mal con la creacion del hilo de impresion\n");
      return -1;
    }

 
    for(int i = 0; i<hilosCreados; i++){
      int status = pthread_join(idHilos[i], NULL);
      if(status < 0){
	      fprintf(stderr, "Error al esperar por el hilo %i\n",i);
		    exit(-1);
	    }
    }

//Cerrar FP
    fclose(fp);



  	for(int i = 0; i<numero_Palabras; i++)
    {
      printf("palabra%i: %s  aparece %i veces\n",i+1,palabras[i],num_palabras[i]);
    }

  }

  
  return 0;
}

//Manejador de Errores
void Funcion_Error(int nHilos, FILE *fp, int numLns){
  if(numLns==0){
    printf("Error en los parametros introducidos\n");
    if(nHilos<=0)
      printf(" Ingresar un numero de hilos para el programa\n");
    
  }
  else
  	printf("\nError");
  	
}

int numero_lineas(char *ruta, int *tam_lineas){
  if(ruta != NULL) {
	FILE* ar = fopen(ruta,"r");
	int lineas = 0;
	int tam_linea_actual = 0;
	while( !feof(ar) && (lineas<MAXnumLNS) ){
	  tam_linea_actual++;
	  numero_Caracteres++;
	  char c = getc(ar);
	  if(c == '\n'){
	    if(tam_lineas != NULL){
	      tam_lineas[lineas] = tam_linea_actual;
	    }
	    lineas++;
	    tam_linea_actual=0;
	  }
	}
    fclose(ar);
    return lineas;
  }
  return -1;
}

void * printEstado(void * arg)
{
  while(1)
  {
    pthread_mutex_lock(&mutex);
    for(int i = 0; i<numero_Palabras; i++)
    {
        printf("palabra%i: %s  aparece %i veces\n",i+1,palabras[i],num_palabras[i]);
    }
    printf("\n");
    pthread_mutex_unlock(&mutex);
    sleep(1);
  }
}
//Funcion para contar palabras
void * countWords(void * datosStruct)
{
  struct structuraHilo *r = (struct structuraHilo*)datosStruct;

  for(int i = r->lineaIn; i <= r->lineaFin; i++)
  { 
    int tam = (r->tam_lineas[i]);
    char *buff;
    buff = (char *)malloc(sizeof(char)*tam);
    
    pthread_mutex_lock(&mutex);
    fseek(r->fp,r->InicioLeer,SEEK_SET);
    fgets(buff,tam,r->fp);
    pthread_mutex_unlock(&mutex);    

    char *token;
//Caracteres no validos
    while((token = strtok_r(buff," \n\e\t\\\?\f\'\"\v\b\r!^~><·$%&/,()=º¡¢£¤¥¦§¨©ª«®¯|@#~½¬-_ç`+*[]{}ḉç¿,.!?:;",&buff)))	
    {
      for(int j=0; j<numero_Palabras;j++)
      {     
        int comp = strcmp(token,palabras[j]);
        if(  comp == 0 )
        {
          pthread_mutex_lock(&mutex);  
            (num_palabras[j])++;
          pthread_mutex_unlock(&mutex); 
        }
      }
    }
    (r->InicioLeer) += tam;
  }
  return (void *)0;
}
