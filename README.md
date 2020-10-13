# Trabajo Práctico 
## Threads y Semáforos ( Hell's burguers) 

El objetivo principal del trabajo práctico es poner en práctica los conceptos aprendidos a lo largo de la materia, 
como lo son: 

* los threads
* los semáforos
* la sección crítica
* la condición de carrera 
* otros más.

Para lograr esto se nos pidió simular una competencia de 3 equipos que realizan una hamburguesa, 
el primero en realizar la receta completa gana. 

## El pseudocódigo:

El primer paso para realizar cualquier proyecto es pensar cómo hacerlo, qué funciones se van a necesitar, qué variables y
en este caso, cuántos semáforos y de qué tipo serán. Para ello implementé lo siguiente: 

![](/declaraciones.jpg)

En el primer recuadro declaré las funciones que se necesitan para realizar la receta y otras necesarias para el funcionamiento del programa. Las funciones en color turquesa no necesitan de ningún semáforo para poder realizarse, es decir no tienen un orden particular.
En cambio las funciones **mezclar**, **salar**, **armar**, **cocinar** y **armarHamburguesa**, necesitan ejecutarse en orden y dependen de que la función anterior las habilite.
Por otro lado hay que recalcar que las funciones **salar**, **hornear** y **cocinar** son de acceso exclusivo y por eso necesitan de un semáforo especial tipo mutex para que los equipos puedan acceder de a uno.
Las funciones remarcadas en amarillo, son funciones que no forman parte de la receta, sin embargo son muy importantes y posteriormente explicaré su contenido.

En el segundo recuadro se pueden observar los semáforos que utilicé, estos semáforos se aseguran de que la receta se ejecute en el orden indicado.

En el tercer recuadro se mencionan los semáforos compartidos que son los que regulan el acceso a la sección crítica. Como sabemos existe un solo salero, un solo horno y una sola plancha para los 3 equipos, es por esto que las funciones **salar**, **honear** y **cocinar** tienen que estar reguladas.   

Por último, en variables globales utilicé un flag (bandera) que regula y controla quien es el ganador.

A continuación observaremos el pseudocódigo completo:

![](/pseudocodigo.jpg)

Otras funciones que forman parte del programa son: 

* **ejecutarReceta()**
* **imprimirAccion()**
* **ganador()**

![](/otrasFunciones.jpg)

En **ejecutarReceta()**, cada equipo crea 8 hilos que ejecutan las distintas acciones. Estas acciones son leídas desde un archivo .txt llamado "receta.txt",
que contiene cada accion a realizar con sus respectivos ingredientes.
Mediante un **_for_** y un **_fgets_** obtenemos las líneas del archivo, esas lineas son separadas por la funcion **_strtok_** y pasadas al **_struct parámetro_**.

```
//Recorro el archivo con la receta y distribuyo los parámetros.
	FILE * archivo;
	archivo = fopen("receta.txt","r");
	int indiceIngredientes= 0;
	char separador[]=",";
	int largo=9;
	char linea[100];
	for (int i=0;i<largo;i++){
		fgets(linea,100,(FILE*) archivo);
		char *palabra= strtok(linea ,separador);	 
		strcpy(pthread_data->pasos_param[i].accion,palabra);
		while(palabra != NULL){
			if (indiceIngredientes==0){
				palabra=strtok(NULL,separador);
				indiceIngredientes++;
			}
			else{ 
				strcpy(pthread_data->pasos_param[i].ingredientes[indiceIngredientes-1],palabra); 
				palabra=strtok(NULL,separador);
				indiceIngredientes++;
			}
		}	
		indiceIngredientes=0;
	}
```

La función **imprimirAccion()** además de imprimir por consola las acciones, los ingredientes y los equipos que las estan ejecutando, guarda esa salida  en un archivo.txt llamado "salida.txt". Cada equipo, accion, ingrediente es concatenado a una línea y guardado en el archivo. Las funciones que se utilizan son **_fputs()_** y **_strcat()_**.

La última función es **ganador()** y ésta es encargada de imprimir por consola y guardar en el archivo txt el ganador de la competencia.
Es llamada desde la función **armarHamburguesa()** y funciona con una bandera declarada en el main (**int flag**), la cual comienza encendida y cuando llega el primer equipo se apaga.

## Prueba de escritorio 

![](/prueba.jpg)




