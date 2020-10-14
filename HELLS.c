
#include <stdio.h>      // libreria estandar
#include <stdlib.h>     // para usar exit y funciones de la libreria standard
#include <string.h>
#include <pthread.h>    // para usar threads
#include <semaphore.h>  // para usar semaforos
#include <unistd.h>


#define LIMITE 50
//Declaro los semáforos y las variables globales.
int flag;
sem_t S;
sem_t P;
sem_t H;

//creo estructura de semaforos 
struct semaforos {
    	sem_t sem_mezclar;
	sem_t sem_salEquipo;
	sem_t sem_cortarLT;         //este semáforo habilita armarHamburguesa()
	sem_t sem_armarMedallones;
	sem_t sem_planchaEquipo;    
	sem_t sem_hornear;          //este semáforo habilita armarHamburguesa()
	sem_t sem_armarHamburguesa; //este semáforo habilita armarHamburguesa()
};

//creo los pasos con los ingredientes
struct paso {
   char accion [LIMITE];
   char ingredientes[4][LIMITE];
};


//creo los parametros de los hilos 
struct parametro {
 	int equipo_param;
  	struct semaforos semaforos_param;
 	struct paso pasos_param[8];
};

void* ganador(void *data){

	FILE *archivoSalida;
	archivoSalida= fopen("salida.txt","a"); //vuelvo a abrir el archivo "salida.txt" para guardar el ganador.
	char numEquipo[2];
	char linea[100];
	struct parametro *mydata =data;
	snprintf(numEquipo,sizeof(numEquipo),"%d", mydata->equipo_param ); //casteo el número de equipo para poder gurdarlo como string.
	printf("\t¡¡¡¡¡¡GANÓ EL EQUIPO: %d !!!!!! \n",mydata->equipo_param); //imprimo por consola
	strcat(linea,"\t¡¡¡¡¡¡GANÓ EL EQUIPO: ");
	strcat(linea,numEquipo);
	strcat(linea,"!!!!!!!\n");
	fputs(linea,archivoSalida); // concateno todo en "linea" y la guardo en el archivo
	fclose(archivoSalida);
}

void* imprimirAccion(void *data, char *accionIn) {
	FILE*archivoSalida;
	archivoSalida = fopen ("salida.txt", "a"); //abro el archivo para poder guardar la salida del programa.
	char numEquipo[2];
	char linea[100];
	struct parametro *mydata =data;
	//calculo la longitud del array de pasos 
	int sizeArray = (int)( sizeof(mydata->pasos_param) / sizeof(mydata->pasos_param[0]));
	//indice para recorrer array de pasos 
	int i;
	for(i = 0; i < sizeArray; i ++){
		//pregunto si la accion del array es igual a la pasada por parametro (si es igual la funcion strcmp devuelve cero)
		if(strcmp(mydata->pasos_param[i].accion, accionIn) == 0){
		printf("\tEquipo %d - accion %s \n " , mydata->equipo_param, mydata->pasos_param[i].accion);
		snprintf(numEquipo, sizeof(numEquipo), "%d", mydata->equipo_param); //casteo de int a String
		strcat(linea, "\tEquipo: ");
		strcat(linea, numEquipo);
		strcat(linea," - accion: ");
		strcat(linea, mydata->pasos_param[i].accion);
		strcat(linea, "\n");
		fputs(linea, archivoSalida); //concateno todo en "linea" para poder guardarlo en el archivo.

		//calculo la longitud del array de ingredientes
		int sizeArrayIngredientes = (int)( sizeof(mydata->pasos_param[i].ingredientes) / sizeof(mydata->pasos_param[i].ingredientes[0]) );
		//indice para recorrer array de ingredientes
		int h;
		printf("\tEquipo %d -----------ingredientes : ----------\n",mydata->equipo_param); 
		strcpy(linea, ""); // "limpio" la variable para poder reutilizarla
		strcat(linea,"\tEquipo: ");
		strcat(linea,numEquipo);
		strcat(linea," ------------ingredientes :-----------\n");
		fputs(linea,archivoSalida);
			for(h = 0; h < sizeArrayIngredientes; h++) {
				//consulto si la posicion tiene valor porque no se cuantos ingredientes tengo por accion 
				if(strlen(mydata->pasos_param[i].ingredientes[h]) != 0) {
							printf("\tEquipo %d ingrediente  %d : %s \n",mydata->equipo_param,h,mydata->pasos_param[i].ingredientes[h]);
							strcpy(linea,"");// "limpio" la variable
							strcat(linea,"\tEquipo ");
							strcat(linea,numEquipo);
							strcat(linea,"  ingrediente: ");
							strcat(linea, mydata->pasos_param[i].ingredientes[h]);
							strcat(linea,"\n");
							fputs(linea,archivoSalida);
				}
			}
		}
	}
	fclose(archivoSalida);
}

void* cortarLT(void *data) { 
	//creo el nombre de la accion 
	 char *accion = "cortarLechugaTomate";
        //creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero).
        struct parametro *mydata = data;
        //llamo a la funcion imprimir le paso el struct y la accion de la funcion
        imprimirAccion(mydata,accion);
        //uso sleep para simular que que pasa tiempo
        usleep( 2000000 );
        //enciendo el semáforo para luego poder armar la Hamburguesa
	sem_post(&mydata->semaforos_param.sem_cortarLT);

    pthread_exit(NULL);
}


void* hornear(void *data) {
	//Pregunto si puedo usarlo
	sem_wait(&H);
        //creo el nombre de la accion de la funcion
        char *accion = "hornear";
        //creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero).
        struct parametro *mydata = data;
        //llamo a la funcion imprimir le paso el struct y la accion de la funcion
        imprimirAccion(mydata,accion);
        //uso sleep para simular que que pasa tiempo
        usleep( 8000000 );
        //doy la señal a la siguiente accion y habilito el horno.
	sem_post(&H);
        sem_post(&mydata->semaforos_param.sem_hornear);
    pthread_exit(NULL);
}


void* cortar(void *data) {
        //creo el nombre de la accion de la funcion
        char *accion = "cortar";
        //creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero).
        struct parametro *mydata = data;
        //llamo a la funcion imprimir le paso el struct y la accion de la funcion
        imprimirAccion(mydata,accion);
        //uso sleep para simular que que pasa tiempo
        usleep( 3000000 );
        //doy la señal a la siguiente accion 
        sem_post(&mydata->semaforos_param.sem_mezclar);

    pthread_exit(NULL);
}

void* mezclar(void *data) {

        //creo el nombre de la accion de la funcion
        char *accion = "mezclar";
        //creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero).
        struct parametro *mydata = data;
	//Pregunto si puedo mezclar
	sem_wait(&mydata->semaforos_param.sem_mezclar);
        //llamo a la funcion imprimir le paso el struct y la accion de la funcion
        imprimirAccion(mydata,accion);
        //uso sleep para simular que que pasa tiempo
        usleep( 2000000 );
        //doy la señal a la siguiente accion 
	sem_post(&mydata->semaforos_param.sem_salEquipo);
	    	

    pthread_exit(NULL);
}

void* salar(void *data) {
	//pregunto si puedo usar el salero
        sem_wait(&S); 
	//creo el nombre de la accion de la funcion
        char *accion = "salar";
        //creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero).
        struct parametro *mydata = data;
	//pregunto si  puedo realizar la acción
	sem_wait(&mydata->semaforos_param.sem_salEquipo);
	
        //llamo a la funcion imprimir le paso el struct y la accion de la funcion
        imprimirAccion(mydata,accion);
        //uso sleep para simular que que pasa tiempo
        usleep( 2000000 );
        //doy la señal a la siguiente accion  y habilito el salero
        sem_post(&S);
	sem_post(&mydata->semaforos_param.sem_armarMedallones);

    pthread_exit(NULL);
}


void* armar(void *data) {
        //creo el nombre de la accion de la funcion
        char *accion = "armarMedallones";
        //creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro 
	struct parametro *mydata = data;
	//pregunto si puedo realizar la accion
	sem_wait(&mydata->semaforos_param.sem_armarMedallones);
        //llamo a la funcion imprimir le paso el struct y la accion de la funcion
        imprimirAccion(mydata,accion);
        //uso sleep para simular que que pasa tiempo
        usleep( 3000000 );
        //doy la señal a la siguiente accion 
	sem_post(&mydata->semaforos_param.sem_planchaEquipo);

    pthread_exit(NULL);
}

void* cocinar(void *data) {
	//pregunto si puedo usar la plancha
	sem_wait(&P);
        //creo el nombre de la accion de la funcion
        char *accion = "cocinar";
        //creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero).
        struct parametro *mydata = data;
	//pregunto si puedo realizar la accion
	sem_wait(&mydata->semaforos_param.sem_planchaEquipo);
        //llamo a la funcion imprimir le paso el struct y la accion de la funcion
        imprimirAccion(mydata,accion);
        //uso sleep para simular que que pasa tiempo
        usleep( 5000000 );
        //doy la señal a la siguiente accion y habilito la plancha
	sem_post(&P);
	sem_post(&mydata->semaforos_param.sem_armarHamburguesa);

    pthread_exit(NULL);
}

void* armarHamburguesa(void *data) {
        //creo el nombre de la accion de la funcion
        char *accion = "armarHamburguesa";
        //creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero).
        struct parametro *mydata = data;
	//Pregunto si se cortó la lechuga y el tomate, si se cocinaron los medallones y si se hornearon los panes.
	sem_wait(&mydata->semaforos_param.sem_hornear);
	sem_wait(&mydata->semaforos_param.sem_cortarLT);
	sem_wait(&mydata->semaforos_param.sem_armarHamburguesa);
        //llamo a la funcion imprimir le paso el struct y la accion de la funcion
        imprimirAccion(mydata,accion);
        //uso sleep para simular que que pasa tiempo
        usleep( 5000000 );
	//el primero que llega, llama a la funcion ganador y deshabilita la bandera.
	if(flag == 1){
		ganador(mydata);
		flag = 0;
	}

  	pthread_exit(NULL);
}

void* ejecutarReceta(void *i){

	sem_t sem_mezclar;
	sem_t sem_salEquipo;
	sem_t sem_armarMedallones;
	sem_t sem_planchaEquipo;
	sem_t sem_cortarLT;
	sem_t sem_hornear;
	sem_t sem_armarHamburguesa;

	pthread_t p1;
	pthread_t p2;
	pthread_t p3;
	pthread_t p4;
	pthread_t p5;
	pthread_t p6;
	pthread_t p7;
	pthread_t p8;

	int p = *((int *) i);

	printf("Ejecutando equipo %d \n", p);

	struct parametro *pthread_data = malloc(sizeof(struct parametro));

	pthread_data->equipo_param = p;


	pthread_data->semaforos_param.sem_mezclar = sem_mezclar;
	pthread_data->semaforos_param.sem_salEquipo = sem_salEquipo;
	pthread_data->semaforos_param.sem_armarMedallones = sem_armarMedallones;
	pthread_data->semaforos_param.sem_planchaEquipo = sem_planchaEquipo;
	pthread_data->semaforos_param.sem_cortarLT = sem_cortarLT;
	pthread_data->semaforos_param.sem_hornear = sem_hornear;
	pthread_data->semaforos_param.sem_armarHamburguesa = sem_armarHamburguesa;

	//Recorro el archivo con la receta y distribuyo los parámetros.
	FILE * archivo;
	archivo = fopen("receta.txt","r");
	int indiceIngredientes= 0;
	char separador[]=",";
	int largo=9;
	char linea[100];
	for (int i=0;i<largo;i++){ // recorro las acciones que son iguales a la cantidad de lineas de la receta
		fgets(linea,100,(FILE*) archivo); //obtengo cada linea
		char *palabra= strtok(linea ,separador);	 // la divido por ","
		strcpy(pthread_data->pasos_param[i].accion,palabra); //guardo la acción
		while(palabra != NULL){ //recorro las palabras de cada linea
			if (indiceIngredientes==0){
				palabra=strtok(NULL,separador); //la acción no la necesito porque ya la guardé (la salteo)
				indiceIngredientes++;
			}
			else{ 
				strcpy(pthread_data->pasos_param[i].ingredientes[indiceIngredientes-1],palabra); // le paso los ingredientes al struct
 				palabra=strtok(NULL,separador);
				indiceIngredientes++;
			}
		}	
		indiceIngredientes=0; // reinicio la variable de los ingredientes por cada linea
	}	

	sem_init(&(pthread_data->semaforos_param.sem_mezclar),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_salEquipo),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_armarMedallones),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_planchaEquipo),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_cortarLT),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_hornear),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_armarHamburguesa),0,0);

	int rc;

	rc = pthread_create(&p1,NULL,cortar,pthread_data);
	rc = pthread_create(&p2,NULL,mezclar,pthread_data);
	rc = pthread_create(&p3,NULL,salar,pthread_data);
	rc = pthread_create(&p4,NULL,armar,pthread_data);
	rc = pthread_create(&p5,NULL,cocinar,pthread_data);
	rc = pthread_create(&p6,NULL,hornear,pthread_data);
	rc = pthread_create(&p7,NULL,cortarLT,pthread_data);
	rc = pthread_create(&p8,NULL,armarHamburguesa,pthread_data);
	
	pthread_join (p1,NULL);
	pthread_join (p2,NULL);
	pthread_join (p3,NULL);
	pthread_join (p4,NULL);
	pthread_join (p5,NULL);
	pthread_join (p6,NULL);
	pthread_join (p7,NULL);
	pthread_join (p8,NULL);

	sem_destroy(&sem_mezclar);
	sem_destroy(&sem_salEquipo);
	sem_destroy(&sem_armarMedallones);
	sem_destroy(&sem_planchaEquipo);
	sem_destroy(&sem_cortarLT);
	sem_destroy(&sem_armarHamburguesa);

	 pthread_exit(NULL);
}





int main ()
{
	//inicio los semáforos en 1
	sem_init(&S,0,1);
	sem_init(&P,0,1);
	sem_init(&H,0,1);
	//inicio la bandera en 1, para poder definir el ganador
	flag = 1;

        int rc;

        int *equipoNombre1 = malloc(sizeof(*equipoNombre1));
	int *equipoNombre2 = malloc(sizeof(*equipoNombre2));
	int *equipoNombre3 = malloc(sizeof(*equipoNombre3));

        *equipoNombre1 = 1;
	*equipoNombre2 = 2;
	*equipoNombre3 = 3;

        pthread_t equipo1;
	pthread_t equipo2;
	pthread_t equipo3;
	
	//si existe el archivo, lo borro y luego en la función ejecutar receta creo uno nuevo
	FILE *archivoSalida;
	archivoSalida= fopen("salida.txt","a");
	remove("salida.txt");



        rc = pthread_create(&equipo1,NULL,ejecutarReceta,equipoNombre1);
	rc = pthread_create(&equipo2,NULL,ejecutarReceta,equipoNombre2);
	rc = pthread_create(&equipo3,NULL,ejecutarReceta,equipoNombre3);

        pthread_join (equipo1,NULL);
	pthread_join (equipo2,NULL);
	pthread_join (equipo3,NULL);


        pthread_exit(NULL);

	sem_destroy(&S);
	sem_destroy(&P);
	sem_destroy(&H);
}

