#include "sprite.h"
#include "SDL.h"

#include <unistd.h>			//usleep

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <pthread.h>
#include <signal.h>

////////////////////////  MACROS  //////////////////////////
#define ANCHO_PANT  630
#define ALTO_PANT	630
#define LIM_INF		504
#define FRAME 		42 	// frames de 42x42 pixeles 

#define CANT_JUG	2
#define CANT_VIDAS	3
#define CANT_NIDOS	7
#define CANT_ENEMIG	8
#define CANT_AMIGOS	16

/////////////////////////  MAPA  ///////////////////////////
// Mapa de tiles (42x42 píxeles) -> pantalla (15x15 tiles)
int mapa[15][15] = {{4,4,4,4,4,4,4,4,4,4,4,4,4,4,4},
					{6,3,5,3,5,3,5,3,5,3,5,3,5,3,7},
					{2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
					{2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
					{2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
					{2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
					{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
					{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
					{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
					{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
					{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
					{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
					{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
					{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
					{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}};
					
Frame tile;
Frame tile1;
Frame tile2;
Frame tile3;
Frame tile4;
Frame tile5;
Frame tile6;
Frame tile7;
Frame tile8;
Frame tile9;
Sprite suelo[10];
////////////////////////////////////////////////////////////



//////////////// CREACION DE SPRITES+FRAMES ////////////////
Frame fmenu;
Frame fvidas;
Frame fvidas2;
Frame fbarra;

Frame frana1_1;
Frame frana1_2;
Frame frana1_3;
Frame frana1_4;
Frame frana1_5;
Frame frana1_6;
Frame frana1_7;
Frame frana1_8;
Frame frana2_1;
Frame frana2_2;
Frame frana2_3;
Frame frana2_4;
Frame frana2_5;
Frame frana2_6;
Frame frana2_7;
Frame frana2_8;

Frame fauto1;
Frame fauto1_2;
Frame fauto2;
Frame fauto3;
Frame fcamion;

Frame ftortugas1;
Frame ftortugas2;
Frame ftronco;

Frame fahogado1;
Frame fahogado2;
Frame fahogado3;
Frame fatropellado1;
Frame fatropellado2;
Frame fmuerte1;
Frame fmuerte2;

Sprite menu(1);
Sprite vidas(2);
Sprite barra(1);

Sprite rana1(8);
Sprite rana2(8);

Sprite auto1(2);
Sprite auto2(1);
Sprite auto3(1);
Sprite camion(1);

Sprite tortugas(2);
Sprite tronco(1);

Sprite ahogado[CANT_JUG];
Sprite atropellado[CANT_JUG];

Jugadores jugador[CANT_JUG];
Enemigos enemigo[CANT_ENEMIG];
Amigos amigo[CANT_AMIGOS];

//////////////////////  V.GLOBALES  ////////////////////////
char jug1[9]=" ";
char jug2[9]=" ";

int abandono=0;
int ini_jug1,ini_jug2;
int menu_ini=2;
int salir;
int ciclos;				//para los movimientos de los sprites
int tiempo_restante;
int cant_fechas;
int caller_socket;

int TIEMPO=0;
int BAJAR=SDLK_DOWN;
int SUBIR=SDLK_UP;
int DERECHA=SDLK_RIGHT;
int IZQUIERDA=SDLK_LEFT;

pthread_mutex_t sem1 = PTHREAD_MUTEX_INITIALIZER; 
pthread_mutex_t sem2 = PTHREAD_MUTEX_INITIALIZER; 

SDL SDL;

///////////////  INICIO + CONFIGURACIONES  /////////////////
void iniciar_frames() {
	//TILES
	tile.cargar((char*)"imagenes/piso.png");
	tile1.cargar((char*)"imagenes/calle.png");	
	tile2.cargar((char*)"imagenes/agua.png");	
	tile3.cargar((char*)"imagenes/nido.png");
	tile4.cargar((char*)"imagenes/pasto.png");
	tile5.cargar((char*)"imagenes/flores.png");
	tile6.cargar((char*)"imagenes/flores2.png");
	tile7.cargar((char*)"imagenes/flores3.png");	
	tile8.cargar((char*)"imagenes/nido_ocupado.png");	
	tile9.cargar((char*)"imagenes/nido_ocupado2.png");
	
	//MENU
	fmenu.cargar((char*)"imagenes/inicio.jpg");
	//INFO
	fvidas.cargar((char*)"imagenes/vidas.png");
	fvidas2.cargar((char*)"imagenes/vidas2.png");
	fbarra.cargar((char*)"imagenes/barra_tiempo.png");
	
	//JUGADORES
	frana1_1.cargar((char*)"imagenes/rana1_1.png");
	frana1_2.cargar((char*)"imagenes/rana1_2.png");
	frana1_3.cargar((char*)"imagenes/rana1_3.png");
	frana1_4.cargar((char*)"imagenes/rana1_4.png");
	frana1_5.cargar((char*)"imagenes/rana1_5.png");
	frana1_6.cargar((char*)"imagenes/rana1_6.png");
	frana1_7.cargar((char*)"imagenes/rana1_7.png");
	frana1_8.cargar((char*)"imagenes/rana1_8.png");
	frana2_1.cargar((char*)"imagenes/rana2_1.png");
	frana2_2.cargar((char*)"imagenes/rana2_2.png");
	frana2_3.cargar((char*)"imagenes/rana2_3.png");
	frana2_4.cargar((char*)"imagenes/rana2_4.png");
	frana2_5.cargar((char*)"imagenes/rana2_5.png");
	frana2_6.cargar((char*)"imagenes/rana2_6.png");
	frana2_7.cargar((char*)"imagenes/rana2_7.png");
	frana2_8.cargar((char*)"imagenes/rana2_8.png");
	
	//ENEMIGOS
	fauto1.cargar((char*)"imagenes/auto1.png");
	fauto1_2.cargar((char*)"imagenes/auto1_2.png");
	fauto2.cargar((char*)"imagenes/auto2.png");
	fauto3.cargar((char*)"imagenes/auto3.png");
	fcamion.cargar((char*)"imagenes/camion.png");
	
	//AMIGOS
	ftortugas1.cargar((char*)"imagenes/tortugas1.png");
	ftortugas2.cargar((char*)"imagenes/tortugas2.png");
	ftronco.cargar((char*)"imagenes/tronco.png");
	
	//ANIMACIONES
	fahogado1.cargar((char*)"imagenes/ahogado1.png");
	fahogado2.cargar((char*)"imagenes/ahogado2.png");
	fahogado3.cargar((char*)"imagenes/ahogado3.png");
	fatropellado1.cargar((char*)"imagenes/atropellado1.png");
	fatropellado2.cargar((char*)"imagenes/atropellado2.png");
	fmuerte1.cargar((char*)"imagenes/muerte1.png");
	fmuerte2.cargar((char*)"imagenes/muerte2.png");
}

void iniciar_sprites() {
	//TILES	
	suelo[0].agregar_frame(tile);
	suelo[1].agregar_frame(tile1);
	suelo[2].agregar_frame(tile2);
	suelo[3].agregar_frame(tile3);
	suelo[4].agregar_frame(tile4);
	suelo[5].agregar_frame(tile5);
	suelo[6].agregar_frame(tile6);
	suelo[7].agregar_frame(tile7);
	suelo[8].agregar_frame(tile8);
	suelo[9].agregar_frame(tile9);
	
	//MENU
	menu.agregar_frame(fmenu);
	//INFO
	vidas.agregar_frame(fvidas);
	vidas.agregar_frame(fvidas2);
	barra.agregar_frame(fbarra);
	
	//JUGADORES
	rana1.agregar_frame(frana1_1);
	rana1.agregar_frame(frana1_2);
	rana1.agregar_frame(frana1_3);
	rana1.agregar_frame(frana1_4);
	rana1.agregar_frame(frana1_5);
	rana1.agregar_frame(frana1_6);
	rana1.agregar_frame(frana1_7);
	rana1.agregar_frame(frana1_8);
	rana2.agregar_frame(frana2_1);
	rana2.agregar_frame(frana2_2);
	rana2.agregar_frame(frana2_3);
	rana2.agregar_frame(frana2_4);
	rana2.agregar_frame(frana2_5);
	rana2.agregar_frame(frana2_6);
	rana2.agregar_frame(frana2_7);
	rana2.agregar_frame(frana2_8);
	
	//ENEMIGOS
	auto1.agregar_frame(fauto1);
	auto1.agregar_frame(fauto1_2);
	auto2.agregar_frame(fauto2);
	auto3.agregar_frame(fauto3);
	camion.agregar_frame(fcamion);
	
	//AMIGOS
	tortugas.agregar_frame(ftortugas1);
	tortugas.agregar_frame(ftortugas2);
	tronco.agregar_frame(ftronco);
	
	//ANIMACIONES
	for(int i=0; i<CANT_JUG;i++) {
		ahogado[i].agregar_frame(fahogado1);
		ahogado[i].agregar_frame(fahogado2);
		ahogado[i].agregar_frame(fahogado3);
		ahogado[i].agregar_frame(fmuerte1);
		ahogado[i].agregar_frame(fmuerte2);
		atropellado[i].agregar_frame(fatropellado1);
		atropellado[i].agregar_frame(fatropellado2);
		atropellado[i].agregar_frame(fmuerte1);
		atropellado[i].agregar_frame(fmuerte2);
	}
}

void inicio_configuraciones() {
	ciclos=0;
	tiempo_restante=TIEMPO;
	
	jugador[0].set_nvidas(CANT_VIDAS);	
	jugador[0].set_nidos(0);
	
	ahogado[0].set_estado(0);
	ahogado[1].set_estado(0);
	atropellado[0].set_estado(0);
	atropellado[1].set_estado(0);
}

void inicio_jugador(int n,int x, int y) {
	jugador[n].setx(x);
	jugador[n].sety(y);
	jugador[n].set_nframe(0);		//frame inicial
	jugador[n].set_activo(1);		//activado
	jugador[n].set_estado(0);
}

void inicio_jugadores() {
	//inicio de posicion
	if(strcasecmp(jug1,jug2)<0){
		ini_jug1=FRAME*4;
		ini_jug2=FRAME*10;
		inicio_jugador(0,ini_jug1,LIM_INF-FRAME+5);
		inicio_jugador(1,ini_jug2,LIM_INF-FRAME+5);
	}
	else{
		ini_jug1=FRAME*10;
		ini_jug2=FRAME*4;
		inicio_jugador(0,ini_jug1,LIM_INF-FRAME+5);
		inicio_jugador(1,ini_jug2,LIM_INF-FRAME+5);
	}
}

void inicio_enemigos() {
	int fila=7;				//comienza a cargar desde la fila 9 del mapa
	
	//inicio de posiciones
	for(int i=0;i<CANT_ENEMIG;i++) {
		if(i%2==0) {
			enemigo[i].setx(ANCHO_PANT);
			enemigo[i].sety(FRAME*fila);
		}
		else {
			enemigo[i].setx(ANCHO_PANT-(FRAME*6));
			enemigo[i].sety(FRAME*fila++);
		}
	}
		
	//incio de velocidades
	enemigo[0].set_vel(2);
	enemigo[1].set_vel(2);
	enemigo[2].set_vel(10);
	enemigo[3].set_vel(10);
	enemigo[4].set_vel(8);
	enemigo[5].set_vel(8);
	enemigo[6].set_vel(5);
	enemigo[7].set_vel(5);
}

void inicio_amigos() {
	int fila=2; 		//comienza a cargar desde la fila 2 del mapa
	int tipo=2;			//cargo primero los troncos
	int pos=5;			//cargar cada cinco frames
	
	//inicio de posiciones
	for(int i=0;i<CANT_AMIGOS;i++){	
		amigo[i].set_tipo(tipo);	
		
		if(tipo==1)						
			amigo[i].setx(FRAME*pos);
		else
			amigo[i].setx(ANCHO_PANT-(FRAME*pos));
		
		amigo[i].sety(FRAME*fila);
		
		pos+=5;
		
		if((i+1)%4==0) {
			if(tipo==2)
				tipo--;
			else
				tipo++;
			fila++;
			pos=5;
		}
	}
		
	//incio de velocidades
	amigo[0].set_vel(6);
	amigo[1].set_vel(6);
	amigo[2].set_vel(6);
	amigo[3].set_vel(6);
	amigo[4].set_vel(3);
	amigo[5].set_vel(3);
	amigo[6].set_vel(3);
	amigo[7].set_vel(3);
	amigo[8].set_vel(4);
	amigo[9].set_vel(4);
	amigo[10].set_vel(4);
	amigo[11].set_vel(4);
	amigo[12].set_vel(2);
	amigo[13].set_vel(2);
	amigo[14].set_vel(2);
	amigo[15].set_vel(2);
}
////////////////////////////////////////////////////////////



//////////////////////   MUERTE   //////////////////////////
void muerte(){
	if(jugador[0].get_nvidas()>0){ 	//tiene vidas
		jugador[0].restar_vida();
		jugador[0].set_activo(0);
	}
}



///////////////////// LLEGO A NIDO  ////////////////////////
void llegada_nido(int num_jug,char* nombre){
	SDL.imprimir_titulo(nombre,FRAME*4,FRAME*5);	
	SDL.actualizar_pantalla();
	
	tiempo_restante=TIEMPO;				//Reinicio el tiempo
			
	inicio_jugadores();					//Reinicio el jugador
}



////////////////////    GRAFICOS   /////////////////////////
void limpiar_mapa() {
	for(int i=1;i<15;i++)
		mapa[1][i++]=3;
}

void dibujar_mapa(int mapa[15][15]){
	int i,j,x,y,t;
	SDL.set_fcolor(255,255,0);

	for (i=0 ; i<15 ; i++) {
		for (j=0 ; j<15 ; j++) {
			t=mapa[i][j];
			// calculo la posición del tile
			x=j*FRAME;
			y=i*FRAME;
			// dibujamos el tile
			suelo[t].setx(x);
			suelo[t].sety(y);
			suelo[t].dibujar(SDL.obtener_pantalla());
			/////////////////////////////////////////////////////
			/////////////////LLEGA AL NIDO///////////////////////
			if(t==3 && suelo[3].colision_agua(rana1,10)==TRUE){
				mapa[i][j]=8; 					//Dibujo el nido_ocupado
				jugador[0].sumar_nidos();
				llegada_nido(0,jug1);	
				sleep(3);
			}
			if(t==3 && suelo[3].colision_agua(rana2,10)==TRUE){
				mapa[i][j]=9; 					//Dibujo el nido_ocupado	
				llegada_nido(1,jug2);	
				sleep(3);
			}
			/////////////////////////////////////////////////////
			/////////////////////////////////////////////////////
		}
	}

}

void dibujar_info(){
	//dibujo las vidas
	for (int i=0 ; i<jugador[0].get_nvidas() ; i++) {
		vidas.seleccionar_frame(0);
		vidas.setx(10+i*22);			//vidas de tam 22x22
		vidas.sety(FRAME*12+5);
		vidas.dibujar(SDL.obtener_pantalla());
	}

	for (int i=0 ; i<jugador[1].get_nvidas() ; i++) {
		vidas.seleccionar_frame(1);
		vidas.setx(ANCHO_PANT-10-(i+1)*22);		//vidas de tam 22x22
		vidas.sety(FRAME*12+5);
		vidas.dibujar(SDL.obtener_pantalla());
	}
	
	//dibujo la barra del tiempo
	for (int i=0 ; i<tiempo_restante ; i++) {
		barra.setx((FRAME*6)+(i*2));	//barra tam 2x22
		barra.sety(FRAME*13);
		barra.dibujar(SDL.obtener_pantalla());
	}
	if(ciclos%20==0)
		tiempo_restante--;
	
	//imprimo el cronometro					
	SDL.set_fcolor(255,255,0);
	SDL.imprimir_numero(tiempo_restante,FRAME*5,FRAME*13-10);	//imprimo

	//imprimo nombre jugador1
	SDL.set_fcolor(0,255,0);
	SDL.imprimir_texto(jug1,10,FRAME*13-15);	
	//imprimo puntaje jugador1
	SDL.imprimir_numero(jugador[0].get_nidos(),10,FRAME*13+15);
	
	//imprimo nombre jugador2
	SDL.set_fcolor(255,0,0);
	SDL.imprimir_texto(jug2,ANCHO_PANT-strlen(jug2)*15,FRAME*13-15);	
	//imprimo puntaje jugador2
	SDL.imprimir_numero(jugador[1].get_nidos(),ANCHO_PANT-30,FRAME*13+15);
}

void dibujar_amigos(){
	int flota=-1;  //NO FLOTA
	
	for(int i=0;i<CANT_AMIGOS;i++){
		if(amigo[i].get_tipo()==1) {
			tortugas.setx(amigo[i].getx());
			tortugas.sety(amigo[i].gety());
			tortugas.seleccionar_frame(amigo[i].get_nframe());
			tortugas.dibujar(SDL.obtener_pantalla());
			
			if(tortugas.colision_agua(rana1,10)==TRUE)
				flota=i; //a salvo sobre el amigo i
		}	
		else {
			tronco.setx(amigo[i].getx());
			tronco.sety(amigo[i].gety());
			tronco.dibujar(SDL.obtener_pantalla());
			
			if(tronco.colision_agua(rana1,10)==TRUE)
				flota=i; //a salvo sobre el amigo i
		}		
	}
	
	//si esta sobre el agua, esta activo, no flota, y no se movio anteriormente-> se ahoga
	if(jugador[0].gety()<(FRAME*6) && jugador[0].get_activo()==1 && flota==-1 && jugador[0].get_estado()==0){
		jugador[0].set_estado(3); //ahogado
		muerte();
	}
	
	//si flota la rana tiene la velocidad del amigo
	if(flota>=0 && jugador[0].get_activo()==1){
		if(amigo[flota].get_tipo()==1)
			jugador[0].setx(jugador[0].getx()-amigo[flota].get_vel());
		if(amigo[flota].get_tipo()==2)
			jugador[0].setx(jugador[0].getx()+amigo[flota].get_vel());
		flota=-1;
	}
}

void dibujar_jugador(){
	// si esta activo dibujo la rana y veo si se mueve
	if(jugador[0].get_activo()==1){
		if(jugador[0].getx()<0)
			jugador[0].setx(0);
		
		if(jugador[0].getx()>=ANCHO_PANT)
			jugador[0].setx(ANCHO_PANT-FRAME);
		
		rana1.setx(jugador[0].getx());
		rana1.sety(jugador[0].gety());
		rana1.seleccionar_frame(jugador[0].get_nframe());
		rana1.dibujar(SDL.obtener_pantalla());
		
		// ¿Nos movimos?
		if(jugador[0].get_nframe()>0 && jugador[0].get_estado()==1 ){
			jugador[0].set_nframe(jugador[0].get_nframe()-1);
			jugador[0].set_estado(0);
		}
	}
}

void dibujar_oponente(){
	if(jugador[1].get_activo()==1) {
		rana2.setx(jugador[1].getx());
		rana2.sety(jugador[1].gety());
		rana2.seleccionar_frame(jugador[1].get_nframe());
		rana2.dibujar(SDL.obtener_pantalla());
	}
}	

void dibujar_enemigos(){
	for(int i=0;i<2;i++) {
		camion.setx(enemigo[i].getx());
		camion.sety(enemigo[i].gety());
		camion.dibujar(SDL.obtener_pantalla());
		
		auto1.setx(enemigo[i+2].getx());
		auto1.sety(enemigo[i+2].gety());
		auto1.dibujar(SDL.obtener_pantalla());
		
		auto2.setx(enemigo[i+4].getx());
		auto2.sety(enemigo[i+4].gety());
		auto2.dibujar(SDL.obtener_pantalla());
		
		auto3.setx(enemigo[i+6].getx());
		auto3.sety(enemigo[i+6].gety());
		auto3.dibujar(SDL.obtener_pantalla());
		
		// ¿Nos piso un auto?
		if (auto1.colision(rana1,0,0) == TRUE || auto2.colision(rana1,0,0) == TRUE ||
			auto3.colision(rana1,0,0) == TRUE || camion.colision(rana1,0,0) == TRUE){
			jugador[0].set_estado(2);
			if(jugador[0].get_activo()==1)
				muerte();
		}	
	}	
}

void dibujar_ahogo(){
	for(int i=0;i<CANT_JUG;i++) {
		if(jugador[i].get_estado()==3){
			ahogado[i].seleccionar_frame(ahogado[i].get_estado());
			ahogado[i].setx(jugador[i].getx());
			ahogado[i].sety(jugador[i].gety());
			ahogado[i].dibujar(SDL.obtener_pantalla());
			if(ciclos%6==0)
				ahogado[i].set_estado(ahogado[i].get_estado()+1);
		}
		if (ahogado[i].get_estado()>=ahogado[i].frames()){ 
			ahogado[i].set_estado(0);
			if(i==0)
				inicio_jugador(0,ini_jug1,LIM_INF-FRAME+5);
			else
				inicio_jugador(1,ini_jug2,LIM_INF-FRAME+5);
			jugador[i].set_estado(0);
		}
	}
}

void dibujar_accidente(){
	for(int i=0;i<CANT_JUG;i++) {
		if(jugador[i].get_estado()==2){
			atropellado[i].seleccionar_frame(atropellado[i].get_estado());
			atropellado[i].setx(jugador[i].getx());
			atropellado[i].sety(jugador[i].gety());
			atropellado[i].dibujar(SDL.obtener_pantalla());
			if(ciclos%6==0)
				atropellado[i].set_estado(atropellado[i].get_estado()+1);
		}
		
		if (atropellado[i].get_estado()>=atropellado[i].frames()){ 
			atropellado[i].set_estado(0);
			if(i==0)
				inicio_jugador(0,ini_jug1,LIM_INF-FRAME+5);
			else
				inicio_jugador(1,ini_jug2,LIM_INF-FRAME+5);
			jugador[i].set_estado(0);
		}
	}
}

// dibuja todo anterior
void dibujar_escena(SDL_Surface *pantalla) {
	
	dibujar_mapa(mapa);
	dibujar_info();	
	dibujar_amigos();
	dibujar_jugador();
	dibujar_oponente();
	dibujar_enemigos();
	dibujar_ahogo();
	dibujar_accidente();
	
	SDL_Flip(pantalla);
}
////////////////////////////////////////////////////////////

//////////////////   MOVIMIENTO SPRITES   //////////////////
void mover_jugador(SDL_Event evento){				
	
	SDL_keysym keysym;
	keysym = evento.key.keysym;
	
	if(jugador[0].get_activo()==1 && jugador[0].get_estado()==0){	
		if(keysym.sym==SUBIR && jugador[0].gety() > FRAME && rana1.colision(rana2,0,FRAME)==FALSE) {
			jugador[0].sety(jugador[0].gety()-FRAME);
			jugador[0].set_nframe(1);
			jugador[0].set_estado(1);
			return;
		}

		if(keysym.sym==BAJAR && jugador[0].gety() < (LIM_INF-FRAME) && rana1.colision(rana2,0,-FRAME)==FALSE) {
			jugador[0].sety(jugador[0].gety()+FRAME);
			jugador[0].set_nframe(3);
			jugador[0].set_estado(1);
			return;
		}
	
		if(keysym.sym==IZQUIERDA && jugador[0].getx() > 0 && rana1.colision(rana2,FRAME,0)==FALSE) {
			jugador[0].setx(jugador[0].getx()-FRAME);
			jugador[0].set_nframe(5);
			jugador[0].set_estado(1);
			return;
		}

		if(keysym.sym==DERECHA && jugador[0].getx() < (ANCHO_PANT) && rana1.colision(rana2,-FRAME,0)==FALSE) {
			jugador[0].setx(jugador[0].getx()+FRAME);
			jugador[0].set_nframe(7);
			jugador[0].set_estado(1);
			return;
		}
	}
	
	return;	
}

void mover_enemigos(){
	int fila=0;
	
	for(int i=0;i<CANT_ENEMIG;i++){
		if(fila%2==0){
			if(enemigo[i].getx() <= (-FRAME)*2)
				enemigo[i].setx(ANCHO_PANT);
			enemigo[i].setx(enemigo[i].getx()-enemigo[i].get_vel());
		}
		else{
			if(enemigo[i].getx() >= ANCHO_PANT+FRAME)
				enemigo[i].setx(-FRAME);
			enemigo[i].setx(enemigo[i].getx()+enemigo[i].get_vel());
		}
		if(i%2==1)
			fila++;
	}
}

void mover_amigos(){	
		
	for(int i=0;i<CANT_AMIGOS;i++) {
		if(amigo[i].get_tipo()==1) {
			if(amigo[i].getx() <= (-FRAME*3))
				amigo[i].setx(ANCHO_PANT);
			amigo[i].setx(amigo[i].getx()-amigo[i].get_vel());
			if(ciclos%10==0) {
				if(amigo[i].get_nframe()==0)
					amigo[i].set_nframe(1);	
				else
					amigo[i].set_nframe(0);
				}
		}
		else{
			if(amigo[i].getx() >= ANCHO_PANT+(FRAME*3))
				amigo[i].setx(-FRAME*3);
			amigo[i].setx(amigo[i].getx()+amigo[i].get_vel());
		}
	}
}

//////////////////   CIERRE DE SPRITES   ///////////////////
void finalizar_sprites() {
	suelo[0].finalizar();
	suelo[1].finalizar();
	suelo[2].finalizar();
	suelo[3].finalizar();
	suelo[4].finalizar();
	suelo[5].finalizar();
	
	menu.finalizar();
	vidas.finalizar();
	barra.finalizar();		
					
	rana1.finalizar();
	rana2.finalizar();

	auto1.finalizar();
	auto2.finalizar();
	auto3.finalizar();
	camion.finalizar();
	
	tortugas.finalizar();
	tronco.finalizar();
	
	//~ ahogado[0].finalizar();
	//~ ahogado[1].finalizar();
	//~ atropellado[0].finalizar();
	//~ atropellado[1].finalizar();
}



//////////////////////  MENU INICIAL ///////////////////////
void menu_inicial() {
	int i=0;
	SDL.set_fcolor(255,255,255);

	while (menu_ini>0) {

		while (SDL.preguntar_evento()) {

			if (SDL.obtener_evento().type == SDL_QUIT) {
				SDL.refrescar_pantalla();	
				SDL.imprimir_texto((char*)"saliendo...",FRAME*11,FRAME*14);
				SDL.actualizar_pantalla();
				sleep(3);
				exit(EXIT_SUCCESS);
			}

			if (SDL.obtener_evento().type == SDL_KEYDOWN ) {
				if (SDL.obtener_evento().key.keysym.sym == SDLK_ESCAPE){
					SDL.refrescar_pantalla();
					SDL.imprimir_texto((char*)"saliendo...",FRAME*11,FRAME*14);
					SDL.actualizar_pantalla();
					sleep(3);	
					exit(EXIT_SUCCESS);
				}
				else {
					if (menu_ini==2)
						menu_ini--;
					else {
						if (SDL.obtener_evento().key.keysym.sym ==SDLK_RETURN && i>=3)
							menu_ini=0;
						if (SDL.obtener_evento().key.keysym.sym ==SDLK_BACKSPACE && i>0)
							jug1[--i]=' ';
						if (i<8 && SDL.obtener_evento().key.keysym.sym >='A' && SDL.obtener_evento().key.keysym.sym <='z')
							jug1[i++]=SDL.obtener_evento().key.keysym.sym;
					}
				}
			}
		}

		menu.dibujar(SDL.obtener_pantalla());

		if(menu_ini==2)
			SDL.imprimir_texto((char*)"presione una tecla "
			"para continuar...",FRAME/2,FRAME*14);	
		if(menu_ini==1) {
			SDL.imprimir_texto((char*)"ingrese nombre:",FRAME/2,FRAME*13);
			SDL.imprimir_texto(jug1,FRAME*7,FRAME*13);	
		}
		SDL.actualizar_pantalla();
		usleep(50000);
	}
	
	SDL.imprimir_texto((char*)"esperando conexion ... ",FRAME/2,FRAME*14);	
	SDL.actualizar_pantalla();	
}


//MENSAJE
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

t_mensaje mensaje;

t_mensaje mensaje_contrario;

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

int cargar_configuracion(t_config* param) {
	FILE* archivo;
	char linea[81];
	char* punt;

	if((archivo=fopen("Global/Config.dat","r"))== NULL)
	{
		printf("Error apertura archivo\n\n");
		exit (EXIT_FAILURE);
	}
	
	while(!feof(archivo))
	{
		fgets(linea,100,archivo);
		punt=linea;
		
		while(*punt!='=')
			punt++;

		*punt='\0';
		if (strcmp(linea,"ip_servidor")==0)
		{
			*punt=' ';
			punt++;
			strcpy(param->ip_servidor,punt);
			param->ip_servidor[strlen(param->ip_servidor)-2]='\0';
		}
		else if (strcmp(linea,"puerto")==0)
		{
			*punt=' ';
			punt++;
			param->puerto=atoi(punt);
		} 
		else if (strcmp(linea,"timeoutservidor")==0)
		{
			*punt=' ';
			punt++;
			param->timeoutservidor=atoi(punt);
		}
		else if (strcmp(linea,"tiempo_de_juego")==0)
		{
			*punt=' ';
			punt++;
			param->tiempo_de_juego=atoi(punt);
		}
		else if (strcmp(linea,"tecla_derecha")==0)
		{
			*punt=' ';
			punt++;
			param->tecla_derecha = *punt;
		}
		else if (strcmp(linea,"tecla_izquierda")==0)
		{
			*punt=' ';
			punt++;
			param->tecla_izquierda = *punt;
		}
		else if (strcmp(linea,"tecla_subir")==0)
		{
			*punt=' ';
			punt++;
			param->tecla_subir = *punt;
		}
		else if (strcmp(linea,"tecla_bajar")==0)
		{
			*punt=' ';
			punt++;
			param->tecla_bajar = *punt;
		}

	}
	fclose (archivo);
	return 0;
}


void cargar_mensaje(){	
	SDL_keysym keysym;
	keysym = SDL.obtener_evento().key.keysym;
	
	mensaje.x=jugador[0].getx();
	mensaje.y=jugador[0].gety();
	mensaje.nf=jugador[0].get_nframe();
	mensaje.nv=jugador[0].get_nvidas();
	mensaje.act=jugador[0].get_activo();
	mensaje.est=jugador[0].get_estado();
	mensaje.nidos=jugador[0].get_nidos();
	
	if(salir==0)
		strcpy(mensaje.ocurrido,"vivo");
	if(ciclos>100 && tiempo_restante==0){
		strcpy(mensaje.ocurrido,"fintiempo");	
	}	
	if(SDL.obtener_evento().type==SDL_QUIT || keysym.sym==SDLK_ESCAPE){
		strcpy(mensaje.ocurrido,"abandone");
		cant_fechas=0;
		abandono=1;
	}
}

void descargar_mensaje(){

	jugador[1].setx(mensaje_contrario.x);
	jugador[1].sety(mensaje_contrario.y);
	jugador[1].set_nframe(mensaje_contrario.nf);
	jugador[1].set_nvidas(mensaje_contrario.nv);
	jugador[1].set_activo(mensaje_contrario.act);
	jugador[1].set_estado(mensaje_contrario.est);
	jugador[1].set_nidos(mensaje_contrario.nidos);
	
	if(strcmp(mensaje_contrario.ocurrido,"ganaste")==0) 
		salir=1;
	
	if(strcmp(mensaje_contrario.ocurrido,"perdiste")==0) 
		salir=-1;
}


//HILOS
void* funcion_comunicacion(void* socket) {
	
	int caller_socket = (int)socket;
	
	while(salir==0) {
		pthread_mutex_lock(&sem1);
		
		cargar_mensaje();
		
		send(caller_socket, &mensaje, sizeof(t_mensaje), 0);
		
		recv(caller_socket, &mensaje_contrario, sizeof(t_mensaje), 0);
			
		descargar_mensaje();
		pthread_mutex_unlock(&sem2);		
	}
	
	return NULL;
}

void* funcion_mover_jugador(void* nulo) {
	while(salir==0){
		while (SDL.preguntar_evento()){ 
			if(SDL.obtener_evento().type == SDL_KEYDOWN)
				mover_jugador(SDL.obtener_evento());
		}
		usleep(50000);	
	}	
	
	return NULL;
}

void* funcion_dibujar_pantalla(void* nulo) {
	inicio_enemigos();
	inicio_amigos();
	inicio_jugadores();
	inicio_configuraciones();
	
	while (salir == 0) {
		
		pthread_mutex_lock(&sem2);	
		dibujar_escena(SDL.obtener_pantalla());

		mover_enemigos();
		mover_amigos();
			
		usleep(50000);			
		ciclos++;
		pthread_mutex_unlock(&sem1);
	}
	
	return NULL;
}

void finalizar_programa(int sig) {
	pthread_mutex_destroy(&sem1);
	pthread_mutex_destroy(&sem2); 	
	close(caller_socket);
	printf("\nCliente desconectado.\n");
	exit(EXIT_SUCCESS);
}

/////////////////////  PRINCIPAL  //////////////////////////
int main(int argc, char *argv[]) {
	t_config configuracion;
	
	signal(SIGINT, finalizar_programa);
	
	//////INICIO SDL/////////////////////////////////////////////////////
	SDL.iniciar(ALTO_PANT,ANCHO_PANT,(char*)"Frogger");
	SDL.cargar_fondo((char*)"imagenes/inicio.jpg");
	SDL.actualizar_pantalla();
	SDL.cargar_fuente_txt((char*)"fuentes/McBoing.ttf",40);
	SDL.cargar_fuente_titulo((char*)"fuentes/McBoing.ttf",100);
	/////////////////////////////////////////////////////////////////////
	
	/** Cargo todos los parametros del archivo de configuracion */
	cargar_configuracion(&configuracion);
	
	TIEMPO=configuracion.tiempo_de_juego;
	
	BAJAR=configuracion.tecla_bajar;
	SUBIR=configuracion.tecla_subir;
	IZQUIERDA=configuracion.tecla_izquierda;
	DERECHA=configuracion.tecla_derecha;
	
	iniciar_frames();	
	iniciar_sprites();
	
	menu_inicial();
	
	/** Creo un socket de escucha. */
	caller_socket = socket(AF_INET, SOCK_STREAM, 0);
	
	/** Creo una estructura para indicar por que red y puerto voy a escuchar los pedidos. */
	struct sockaddr_in listen_address;		//Dirreccion de escucha.
	
	listen_address.sin_family 		= AF_INET;  
	listen_address.sin_port 		= htons(configuracion.puerto);  			// Número de puerto donde recibirá paquetes el programa  
	listen_address.sin_addr.s_addr  = inet_addr (configuracion.ip_servidor);	// IP por la que recibirá paquetes el programa  
	
	/** Realizo la conexion del cliente con el servidor. */
	if(connect(caller_socket, (struct sockaddr *) &listen_address, sizeof(struct sockaddr))==-1)
		perror("Error");
	else {
		sleep(1);
		menu.dibujar(SDL.obtener_pantalla());
		SDL.imprimir_texto((char*)"conectado ",FRAME/2,FRAME*14);	
		SDL.actualizar_pantalla();	
	}

	/** Envio mi nombre al servidor. */
	if(send(caller_socket,&jug1,sizeof(char)*9,0)==-1)
		perror("Error");
	
	/** Recibo la cant_fechas del torneo */
	if((recv(caller_socket,&cant_fechas,sizeof(int),0))==-1)
		perror("Error");
	
	if(cant_fechas==0){
		menu.dibujar(SDL.obtener_pantalla());
		SDL.imprimir_texto((char*)"eliminado del torneo por falta de oponente",FRAME/2,FRAME*14);	
		SDL.actualizar_pantalla();
		sleep(3);	
	}

	pthread_t thread_id[3];
	
	while(cant_fechas--) {	
		salir=0;
		
		SDL.set_fcolor(255,255,255);
		menu.dibujar(SDL.obtener_pantalla());
		SDL.imprimir_texto((char*)"esperando oponente...",FRAME/2,FRAME*14);	
		SDL.actualizar_pantalla();	
		
		memset(jug2,0,9);
		bzero(jug2,sizeof(char)*9);
		/** recibo el nombre del rival */
		if((recv(caller_socket,&jug2,sizeof(char)*9,0))==-1)
			perror("Error1");

		if(strcmp(jug2,"ganaste1")==0) {
			menu.dibujar(SDL.obtener_pantalla());
			SDL.imprimir_texto((char*)"fecha libre",FRAME,FRAME*12);
			SDL.imprimir_texto((char*)"ganaste por abandono del rival",FRAME,FRAME*13);
			SDL.actualizar_pantalla();
			sleep(5);
		}
		else{
			menu.dibujar(SDL.obtener_pantalla());
			SDL.imprimir_texto((char*)"proximo rival",FRAME,FRAME*12);
			SDL.imprimir_texto(jug2,FRAME*6,FRAME*12);	
			SDL.actualizar_pantalla();
			sleep(3);

			pthread_mutex_lock(&sem2);	
			limpiar_mapa();	
			pthread_create( &thread_id[0], NULL, funcion_comunicacion,(void*)caller_socket);
			pthread_create( &thread_id[1], NULL, funcion_mover_jugador,(void*)salir);
			pthread_create( &thread_id[2], NULL, funcion_dibujar_pantalla,(void*)salir);
		
			pthread_join( thread_id[0], NULL);
			pthread_join( thread_id[1], NULL);
			pthread_join( thread_id[2], NULL);
			pthread_mutex_unlock(&sem2);	
			
			dibujar_escena(SDL.obtener_pantalla()); //dibujo el ultimo refresco antes de salir

			SDL.set_fcolor(255,255,0);
			if(salir==-1){
				SDL.imprimir_titulo((char*)"perdiste",FRAME*4,FRAME*5);
				SDL.actualizar_pantalla();
			}
			if(salir==1){
				SDL.imprimir_titulo((char*)"ganaste",FRAME*4,FRAME*5);
				SDL.actualizar_pantalla();
			}
			sleep(5);
		}
	}
	
	if(abandono==0) {
		SDL.set_fcolor(255,255,255);
		menu.dibujar(SDL.obtener_pantalla());
		SDL.imprimir_texto((char*)"final del torneo",FRAME/2,FRAME*14);	
		SDL.actualizar_pantalla();		
		sleep(5);
	}
	
	pthread_mutex_destroy(&sem1);
	pthread_mutex_destroy(&sem2); 	
	close(caller_socket);

	return 0;
}
////////////////////////////////////////////////////////////
