#include "sprite.h"
#include "SDL.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netinet/in.h>
#include <unistd.h> 	/* close() */

#include <signal.h>
#include <sys/wait.h>
#include <sys/shm.h> 	// Contiene las definiciones de memoria compartida.
#include <sys/ipc.h>	// ftok
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <errno.h>
 
#include <pthread.h>

#define BACKLOG 5
#define ALTO_PANT	630
#define ANCHO_PANT 	900
#define COL 100

typedef struct{
	char linea[100];
	int jugadores_inscriptos;	
	int partidos_faltantes;
	int clientes_conectados;
	int partidos_jugados;
	int tabla_posiciones[100][5];
} t_resumen;

typedef struct{	
	int x,y;	
	int nf;
	int nv;
	int act;
	int est;  
	int nidos;
	char nombre_rival[9];
	int contar;
	char ocurrido[16];
} t_mensaje;

typedef struct {
	int puerto;
	char ip_servidor[16];
	
	int timeoutservidor;
	int tiempo_de_juego;
	char tecla_subir;
	char tecla_derecha;
	char tecla_izquierda;
	char tecla_bajar;
	
} t_config;

int cargar_configuracion(t_config*);

void esperar_hijo(int sig);
void comienzo_de_juego(int sig);
void finalizar_programa(int sig);
void* mostrar_inscripcion_torneo(void*);
void* comunicacion_cliente(void*);
int procesar_mensaje(int,t_mensaje*,int,t_mensaje*);

void Armado_Partido_Fecha(int[][100], int);
void Inicializo_Matriz_Segun_Cant_Partidos(int[][100], int);
void Mostrar_Matriz(int [][100], int);
void Ordenador_Posiciones(int [][5]);
void mostrar_posiciones(SDL);
void mostrar_info_torneo(int);


typedef struct{
	int numero;
	char nombre[9];
}lista_jugadores;

lista_jugadores lista_jug[100];

int MARCA=1;
int socket_escucha, socket_comunicacion[100];
int cant_jugadores = 0;
int salir=0;
int mostrar=0;
	
t_resumen* resumen;

pid_t pid_visual;
int estado_visual;

int main(void) {	
	int id_resumen;
	key_t claveMemoria;
	
	pthread_t thread_id[2];
	pthread_create(&thread_id[0],NULL,mostrar_inscripcion_torneo,(void*)lista_jug);
	
	claveMemoria =ftok("/",5000);
	id_resumen=shmget(claveMemoria,sizeof(t_resumen), IPC_CREAT | 0600);
	
	resumen=(t_resumen*)shmat(id_resumen,NULL,0);
	
	t_config configuracion;
	
	/** Cargo todos los parametros del archivo de configuracion */
	cargar_configuracion(&configuracion);
	
	//signal(SIGCHLD, esperar_hijo);			/* manejos de zombies */
	signal(SIGALRM, comienzo_de_juego);		
    signal(SIGINT, finalizar_programa);		/* manejo ctrl+c */
				
	/** Creo un socket de escucha. */
	if((socket_escucha = socket(AF_INET, SOCK_STREAM, 0))==-1) {
		strcpy(resumen->linea,"Error al crear el socket");
		sleep(3);
		exit(1);
	} 

	/** Creo una estructura para indicar por que red y puerto voy a escuchar los pedidos. */
	struct sockaddr_in listen_address;		//Dirreccion de escucha.
	struct sockaddr_in caller_address;		//Dirreccion de escucha.
	
	listen_address.sin_family 		= AF_INET;  
	listen_address.sin_port 		= htons(configuracion.puerto);  			// Número de puerto donde recibirá paquetes el programa  
	listen_address.sin_addr.s_addr  = inet_addr (configuracion.ip_servidor);	// IP por la que recibirá paquetes el programa  
	
	/** Asociar direccion ip y puerto al socket */
	if((bind(socket_escucha, (struct sockaddr *)&listen_address, sizeof(struct sockaddr)))==-1) {
		strcpy(resumen->linea,"Error al asociar direccion ip y puerto al socket");
		sleep(3);
		exit(EXIT_FAILURE);
	}
	
	/** Habilitar el socket para recibir conexiones en forma pasiva. */
	if((listen(socket_escucha,BACKLOG))==-1) {
		strcpy(resumen->linea,"Error al inicializar el servidor");
		sleep(3);
		exit(EXIT_FAILURE);
	} 		
	strcpy(resumen->linea,"Servidor Inicializado");
	
	/** Se programa un alarma para terminar tiempo de conexion del servidor con el cliente.*/
	alarm(configuracion.timeoutservidor);
	
	int size_caller_address;
	
	/** Esta parte del codigo se encarga de encolar a todas las conexiones en socket_comunicacion[] dentro de un tiempo. */
	while(MARCA){
		size_caller_address=sizeof(struct sockaddr);
		
		/** Se crea una conexion con el cliente.*/
		if((socket_comunicacion[cant_jugadores]=accept(socket_escucha,(struct sockaddr *)&caller_address,(socklen_t *)&size_caller_address))==-1) {
			continue;
		} else {
			recv(socket_comunicacion[cant_jugadores],&lista_jug[cant_jugadores].nombre,sizeof(char)*9,0);
			cant_jugadores++;	
		}
	}
	
	close(socket_escucha);
	int cant_fechas=0;
		
	///CREO LOS SERVIDORES DE PARTIDA.
	if(cant_jugadores%2!=0){
		cant_jugadores--;
		send(socket_comunicacion[cant_jugadores],&cant_fechas,sizeof(int),0);
	}
	
	int cant_partidos=cant_jugadores/2;	//ejemplo: 12/2=6 	cantidad de partidos por fecha.
	cant_fechas=cant_jugadores-1;	//ejemplo: 12-1=11 	Cantidad de fechas.
	int fecha, partido;
	int matriz_versus[2][100];
	
	int i, rival1, rival2;
	
	/** Envio a todos los clientes la cant_jugadores inscriptos en el torneo */
	for(int k=0;k<cant_jugadores;k++)
		send(socket_comunicacion[k],&cant_fechas,sizeof(int),0);
		
	/** inicializo la memoria compartida */
	resumen->clientes_conectados=cant_jugadores;
	resumen->jugadores_inscriptos=cant_jugadores;
	resumen->partidos_faltantes=cant_fechas*cant_partidos;
	resumen->partidos_jugados=0;	

	for(i=0;i<cant_jugadores;i++) {
		resumen->tabla_posiciones[i][0]=i; //Identifico en la primera columna quien es el jugador.
		resumen->tabla_posiciones[i][1]=0; //Indicializo la tabla de posiciones
		resumen->tabla_posiciones[i][2]=0;
		resumen->tabla_posiciones[i][3]=0;
		resumen->tabla_posiciones[i][4]=1;
	}
	pthread_join(thread_id[0], NULL);
	
	Inicializo_Matriz_Segun_Cant_Partidos(matriz_versus,cant_partidos); //ejemplo: m[2][12/2=6]=[0  1  2  3  4  5 ][ 6  7  8  9  10  11]
	      
    int vector_versus1[2];
	
	for (fecha = 0; fecha < cant_fechas; fecha++) {
		/** Acá se rearma como van a ser los partidos. Se determina quien juega con quien. */
		 if(fecha!=0)
			Armado_Partido_Fecha(matriz_versus,cant_partidos);
		
		strcpy(resumen->linea,"Inicio Fecha");
		
		switch (pid_visual=fork()){
			case -1://error
				printf("\nError: no se pudo crear visualizador del torneo\n");
				exit(EXIT_FAILURE);
				break;
			case 0:
				mostrar_info_torneo(cant_partidos);
				exit(0);
				break;
		}
		
		/** Una vez determinado como que jugadores jugaran en cada partido. 
		* Creo sus servidores de partidas correspondientes. */
		for(partido=0; partido<cant_partidos; partido++) {
			
			rival1=matriz_versus[0][partido];	/** Averiguo quien es el jugador uno del partido. */
			rival2=matriz_versus[1][partido];	/** Averiguo quien es el jugador dos del partido. */
		
			strcpy(resumen->linea,"Fecha en juego");
			//printf("Partido %d: %s vs %s\n",partido+1, lista_jug[rival1].nombre,lista_jug[rival2].nombre);
			
			switch (fork()) {
				case -1://error
					printf("\nError: no se pudo crear proceso\n");
					exit(EXIT_FAILURE);
					break;

				//*Servidores de partida.****************************************************************
				case 0:
					vector_versus1[0]=rival1;
					vector_versus1[1]=rival2;
					
					if(resumen->tabla_posiciones[rival1][4]==0 && resumen->tabla_posiciones[rival2][4]==0) {
						resumen->partidos_jugados++;
						resumen->partidos_faltantes--;
						exit(EXIT_SUCCESS);
					}	
					if(resumen->tabla_posiciones[rival1][4]==0){
						char aux[9]={"ganaste1"};
						send(socket_comunicacion[rival2], &aux, sizeof(char)*9, 0);
						resumen->tabla_posiciones[rival2][1]++;
						resumen->partidos_jugados++;
						resumen->partidos_faltantes--;
						exit(EXIT_SUCCESS);
					}
					
					if(resumen->tabla_posiciones[rival2][4]==0){	
						char aux[9]={"ganaste1"};
						send(socket_comunicacion[rival1], &aux, sizeof(char)*9, 0);
						resumen->tabla_posiciones[rival1][1]++;
						resumen->partidos_jugados++;
						resumen->partidos_faltantes--;
						exit(EXIT_SUCCESS);
					}
														
					pthread_create(&thread_id[1],NULL,comunicacion_cliente,(void*)vector_versus1);
					pthread_join(thread_id[1], NULL);
					
					for(i=0;i<cant_jugadores;i++)
						close(socket_comunicacion[i]);
					
					/*suma partido jugado*/
					resumen->tabla_posiciones[rival1][1]++;
					resumen->tabla_posiciones[rival2][1]++;
					resumen->partidos_jugados++;
					resumen->partidos_faltantes--;

					exit(0);
					break;
				}	
			}
			/** El servidor una vez creado todos los servidores de partidas 
			*espera que todos terminen correctamente.*/
			for (i = 0; i < cant_partidos+1; i++){	
				wait(NULL);
		}					
		Ordenador_Posiciones(resumen->tabla_posiciones);
	}

	for(i=0;i<cant_jugadores;i++)
		close(socket_comunicacion[i]);

	/** Cierro el socket de escucha. */
	close(socket_escucha);

    shmdt(resumen);
	sleep(5);
    
    return 0;
}

void* comunicacion_cliente(void* vector) {
	t_mensaje mensaje1, mensaje2;
	
	int* vector_versus = (int*)vector;
	
	int rival1 = vector_versus[0];
	int rival2 = vector_versus[1];
	
	send(socket_comunicacion[rival1], &lista_jug[rival2].nombre, sizeof(char)*9, 0);
	send(socket_comunicacion[rival2], &lista_jug[rival1].nombre, sizeof(char)*9, 0);

		
	while(salir==0) {
		recv(socket_comunicacion[rival1], &mensaje1, sizeof(t_mensaje), 0);		
		recv(socket_comunicacion[rival2], &mensaje2, sizeof(t_mensaje), 0);
		
		salir=procesar_mensaje(rival1,&mensaje1,rival2,&mensaje2);
		
		send(socket_comunicacion[rival1], &mensaje2, sizeof(t_mensaje), 0);
		send(socket_comunicacion[rival2], &mensaje1, sizeof(t_mensaje), 0);
		
	}
	return NULL;
}

void* mostrar_inscripcion_torneo(void*jug) {
	char aux[50];
	SDL SDL;

	SDL.iniciar(ALTO_PANT,ANCHO_PANT,(char*)"Servidor Frogger");
	SDL.cargar_fondo((char*)"imagenes/servidor.png");
	SDL.cargar_fuente_txt((char*)"fuentes/FreeSansBold.ttf",18);
	SDL.set_fcolor(0,0,0);	
	SDL.refrescar_pantalla();
	
	while(MARCA){
		SDL.dibujar_pantalla();
		SDL.imprimir_texto(resumen->linea,150,260);
		for(int i=0;i<cant_jugadores;i++) {
			strcpy(aux,"El servidor recibio una conexion: ");
			strcat(aux,lista_jug[i].nombre);
			SDL.imprimir_texto(aux,175,290+(i*20));
		}
		SDL.actualizar_pantalla();
		sleep(1);
	}
	
	SDL.refrescar_pantalla();
	SDL.imprimir_texto((char*)"Fin de la inscripcion",150,260);
	SDL.actualizar_pantalla();	
	sleep(2);

	if(cant_jugadores>0) {
		SDL.imprimir_texto((char*)"Jugadores inscriptos",150,290);
		SDL.imprimir_texto((char*)"__________________",150,295);
		for(int i=0;i<cant_jugadores;i++) {
			SDL.imprimir_texto(lista_jug[i].nombre,175,320+(i*20));
			SDL.actualizar_pantalla();
		}
		sleep(2);
		SDL.imprimir_texto((char*)"Comienzo del juego",150,320+((cant_jugadores+1)*20));
		SDL.actualizar_pantalla();
	}
	else {
		SDL.imprimir_texto((char*)"No hubo jugadores inscriptos",150,290);
		SDL.actualizar_pantalla();
		sleep(2);
		SDL.imprimir_texto((char*)"El servidor se cerrara",150,310);
		SDL.actualizar_pantalla();
	}
	sleep(2);
	SDL.finalizar();
	return NULL;
}

void mostrar_info_torneo(int cant_partidos){
	SDL SDL2;
	int p=resumen->partidos_faltantes;

	SDL2.iniciar(ALTO_PANT,ANCHO_PANT,(char*)"Servidor Frogger");
	SDL2.cargar_fondo((char*)"imagenes/servidor.png");
	SDL2.cargar_fuente_txt((char*)"fuentes/FreeSansBold.ttf",18);
	SDL2.set_fcolor(0,0,0);	
	SDL2.refrescar_pantalla();
	
	while(p-resumen->partidos_faltantes!=cant_partidos){
		SDL2.dibujar_pantalla();
		SDL2.imprimir_texto(resumen->linea,150,250);
		SDL2.imprimir_texto((char*)"Partidos jugados:",150,275);
		SDL2.imprimir_numero(resumen->partidos_jugados,325,275);
		SDL2.imprimir_texto((char*)"Partidos faltantes:",350,275);
		SDL2.imprimir_numero(resumen->partidos_faltantes,535,275);
		mostrar_posiciones(SDL2);
		SDL2.actualizar_pantalla();
		sleep(2);
	}
	
	if(resumen->partidos_faltantes==0) {
		SDL2.dibujar_pantalla();
		SDL2.imprimir_texto((char*)"Final del torneo",150,250);
		Ordenador_Posiciones(resumen->tabla_posiciones);
		mostrar_posiciones(SDL2);
		SDL2.actualizar_pantalla();
		sleep(10);
	}
	SDL2.finalizar();
}

int procesar_mensaje(int jug1,t_mensaje* m1,int jug2, t_mensaje* m2){
	if (strcmp(m1->ocurrido, "fintiempo") == 0 || strcmp(m2->ocurrido, "fintiempo")==0) { 
		resumen->tabla_posiciones[jug1][3]++;
		resumen->tabla_posiciones[jug2][3]++;
		strcpy(m1->ocurrido,"perdiste");
		strcpy(m2->ocurrido,"perdiste");
		return 1;			
	}
	if (strcmp(m1->ocurrido, "abandone") == 0) { 
		resumen->tabla_posiciones[jug1][4]=0;
		resumen->tabla_posiciones[jug1][3]++;
		resumen->tabla_posiciones[jug2][2]++;
		strcpy(m1->ocurrido,"ganaste");
		strcpy(m2->ocurrido,"perdiste");
		return 1;			
	}
	if (strcmp(m2->ocurrido, "abandone") == 0) { 
		resumen->tabla_posiciones[jug2][4]=0;
		resumen->tabla_posiciones[jug2][3]++;
		resumen->tabla_posiciones[jug1][2]++;
		strcpy(m2->ocurrido,"ganaste");
		strcpy(m1->ocurrido,"perdiste");
		return 1;			
	}
	if (m1->nv == 0) {
		resumen->tabla_posiciones[jug1][3]++;
		resumen->tabla_posiciones[jug2][2]++;
		strcpy(m1->ocurrido,"ganaste");
		strcpy(m2->ocurrido,"perdiste");
		return 1;			
	}
	if (m2->nv == 0) {
		resumen->tabla_posiciones[jug2][3]++;
		resumen->tabla_posiciones[jug1][2]++;
		strcpy(m2->ocurrido,"ganaste");
		strcpy(m1->ocurrido,"perdiste");
		return 1;			
	}
	if (m1->nidos == 4){ 
		resumen->tabla_posiciones[jug1][2]++;
		resumen->tabla_posiciones[jug2][3]++;
		strcpy(m1->ocurrido,"perdiste");
		strcpy(m2->ocurrido,"ganaste");
		return 1;			
	}
	if (m2->nidos == 4){ 
		resumen->tabla_posiciones[jug2][2]++;
		resumen->tabla_posiciones[jug1][3]++;
		strcpy(m2->ocurrido,"perdiste");
		strcpy(m1->ocurrido,"ganaste");
		return 1;			
	}
	return 0;
}

void Armado_Partido_Fecha(int m[][100], int cant_partidos) {
    int i, j, aux1, aux2;

    aux1=m[0][1];
    
    for(i=2;i<cant_partidos;i++)
        m[0][i-1]=m[0][i];
   

    aux2=m[1][--i];

    for(j=i;j>0;j--)
        m[1][j]=m[1][j-1];

    m[1][0]=aux1;
    m[0][i]=aux2;

}

void Inicializo_Matriz_Segun_Cant_Partidos(int matriz_versus[][100], int cant_partidos) {
	int i, j, cont=0;
	
	for(i=0;i<2;i++)
		for(j=0;j<cant_partidos;j++)
			matriz_versus[i][j]=cont++;
}
 
void Mostrar_Matriz(int matriz_versus[][100], int cant_partidos) {
	int i,j;
	
    for(i=0;i<2;i++)
    {
        for(j=0;j<cant_partidos;j++)
            printf("%d\t",matriz_versus[i][j]);

        printf("\n");
    }	
}

void Ordenador_Posiciones(int m[][5]) {
   int i=1, marca=1, j, aux;

	while( marca == 1 )
    {
        marca = 0;

        for( j=0; j<10-i; j++ )
        {
            if( m[j][2] < m[j+1][2] ) // SI partidos ganados es menor que el siguiente entonces cambiar...
            {
                aux = m[j][0];		//Cambio el orden del jugador
                m[j][0] = m[j+1][0];
                m[j+1][0] = aux;

                aux = m[j][1];		//Cambio de partidos jugados
                m[j][1] = m[j+1][1];
                m[j+1][1] = aux;

                aux = m[j][2];		//Cambio el orden de partidos ganados
                m[j][2] = m[j+1][2];
                m[j+1][2] = aux;				

                aux = m[j][3];		//Cambio el orden de partidos perdidos
                m[j][3] = m[j+1][3];
                m[j+1][3] = aux;
				
				aux = m[j][4];		//Cambio el orden de estado del jugador
                m[j][4] = m[j+1][4];
                m[j+1][4] = aux;
	            
	            marca = 1;
            }
        }
        i++;
    }
}
  
 void mostrar_posiciones(SDL SDL) {
	SDL.imprimir_texto((char*)"Tabla de posiciones",150,300);
	SDL.imprimir_texto((char*)"Jugador    PJ  PG  PP",150,325);
	
	for(int i=0;i<cant_jugadores;i++) {
		SDL.imprimir_texto(lista_jug[resumen->tabla_posiciones[i][0]].nombre,150,350+(i*20));
		SDL.imprimir_numero(resumen->tabla_posiciones[i][1],250,350+(i*20));
		SDL.imprimir_numero(resumen->tabla_posiciones[i][2],285,350+(i*20));
		SDL.imprimir_numero(resumen->tabla_posiciones[i][3],320,350+(i*20));
		if(resumen->tabla_posiciones[i][4]==0)
			SDL.imprimir_texto((char*)"SALIO DEL TORNEO",350,350+(i*20));
	}
} 
    
void esperar_hijo(int sig) {
    int estado;
    while (wait3(&estado, WNOHANG, (struct rusage *)0) >= 0);
}

void comienzo_de_juego(int sig) {
	MARCA=0; 
	close(socket_escucha);
	sleep(8);
}

/* Caida del servidor ctrl+c*/
void finalizar_programa(int sig) {
	for(int i=0;i<cant_jugadores;i++)
		close(socket_comunicacion[i]);
	close(socket_escucha);
	kill(pid_visual,9);
	shmdt(resumen);
	printf("\nServidor desconectado.\n");
	exit(EXIT_SUCCESS);
}

int cargar_configuracion(t_config* param) {
	FILE* archivo;
	char linea[81];
	char* punt;

	if((archivo=fopen("Global/Config.dat","r"))== NULL) {
		printf("Error apertura archivo\n\n");
		exit (EXIT_FAILURE);
	}
	
	while(!feof(archivo)) {
		fgets(linea,100,archivo);
		punt=linea;
		
		while(*punt!='=')
			punt++;

		*punt='\0';
		if (strcmp(linea,"ip_servidor")==0) {
			*punt=' ';
			punt++;
			strcpy(param->ip_servidor,punt);
			param->ip_servidor[strlen(param->ip_servidor)-2]='\0';
		}
		else if (strcmp(linea,"puerto")==0) {
			*punt=' ';
			punt++;
			param->puerto=atoi(punt);
		}
		else if (strcmp(linea,"timeoutservidor")==0) {
			*punt=' ';
			punt++;
			param->timeoutservidor=atoi(punt);
		}
		else if (strcmp(linea,"tecla_derecha")==0) {
			*punt=' ';
			punt++;
			param->tecla_derecha = *punt;
		}
		else if (strcmp(linea,"tecla_izquierda")==0) {
			*punt=' ';
			punt++;
			param->tecla_izquierda = *punt;
		}
		else if (strcmp(linea,"tecla_subir")==0) {
			*punt=' ';
			punt++;
			param->tecla_subir = *punt;
		}
		else if (strcmp(linea,"tecla_bajar")==0) {
			*punt=' ';
			punt++;
			param->tecla_bajar = *punt;
		}
	}
	
	fclose (archivo);
	return 0;
}
