#include "SDL.h"
#include "sprite.h"

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

void SDL::iniciar(int alto,int ancho,char* nombre) {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) 
		printf("No se pudo iniciar SDL: %s\n",SDL_GetError());
	atexit(SDL_Quit);

	pantalla = SDL_SetVideoMode(ancho,alto,32,SDL_DOUBLEBUF);
	if (pantalla == NULL)
		printf("No se puede inicializar el modo gráfico:%s \n",SDL_GetError());

	SDL_WM_SetCaption(nombre, nombre); //nombre de la ventana
	
	if (TTF_Init() < 0) 
		printf("No se pudo iniciar SDL_ttf: %s\n",SDL_GetError());
	atexit(TTF_Quit);
}

void SDL::finalizar() {
	SDL_Quit();
}

void SDL::cargar_fondo(char* imagen) {
	img_fondo.cargar(imagen);
	fondo.agregar_frame(img_fondo);
}

void SDL::actualizar_pantalla() {
	SDL_Flip(pantalla);
}

void SDL::refrescar_pantalla() {
	fondo.setx(0);
	fondo.sety(0);
	fondo.seleccionar_frame(0);
	fondo.dibujar(pantalla);
	SDL_Flip(pantalla);
}

void SDL::dibujar_pantalla() {
	fondo.setx(0);
	fondo.sety(0);
	fondo.seleccionar_frame(0);
	fondo.dibujar(pantalla);
}

void SDL::cargar_fuente_txt(char* f, int tam) {
	fuente_txt = TTF_OpenFont(f,tam);
}

void SDL::cargar_fuente_titulo(char* f, int tam) {
	fuente_titulo = TTF_OpenFont(f,tam);
}

void SDL::imprimir_texto(char* texto,int x,int y) {

	SDL_Rect rectangulo;
	SDL_Surface *txt;
	
	txt = TTF_RenderText_Blended(fuente_txt,texto,fcolor);
	
	rectangulo.y=y;
	rectangulo.x=x;
	rectangulo.w=txt->w;
	rectangulo.h=txt->h;

	SDL_BlitSurface(txt,NULL,pantalla,&rectangulo);	//imprime texto
}

void SDL::imprimir_titulo(char* texto,int x,int y) {

	SDL_Rect rectangulo;
	SDL_Surface *txt;
	
	txt = TTF_RenderText_Blended(fuente_titulo,texto,fcolor);
	
	rectangulo.y=y;
	rectangulo.x=x;
	rectangulo.w=txt->w;
	rectangulo.h=txt->h;

	SDL_BlitSurface(txt,NULL,pantalla,&rectangulo);	//imprime texto
}

void SDL::imprimir_numero(int numero,int x,int y) {
	char texto[5];
	
	sprintf(texto,"%d",numero);
	
	imprimir_texto(texto,x,y);
}

void SDL::set_fcolor(int r, int g, int b) {
	fcolor.r=r;
	fcolor.g=g;
	fcolor.b=b;
}

int SDL::preguntar_evento(){
	return SDL_PollEvent(&evento);
}
