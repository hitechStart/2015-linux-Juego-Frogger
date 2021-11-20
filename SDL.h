#ifndef SDL_H_
#define SDL_H_
#define TRUE 1
#define FALSE 0

#include "sprite.h"
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

class SDL {
	private:
		Frame img_fondo;
		Sprite fondo;
		SDL_Surface* pantalla;
		TTF_Font* fuente_txt;
		TTF_Font* fuente_titulo;
		SDL_Color fcolor;
		SDL_Event evento;
	public:
		void iniciar(int alto,int ancho,char* nombre);
		void finalizar();
		
		void cargar_fondo(char* imagen);
		void actualizar_pantalla();
		void refrescar_pantalla();
		void dibujar_pantalla();
		SDL_Surface* obtener_pantalla(){return pantalla;}
		
		void cargar_fuente_txt(char* f,int t);
		void cargar_fuente_titulo(char* f,int t);
		void set_fcolor(int r,int g,int b);
		void imprimir_texto(char* texto,int x,int y);
		void imprimir_titulo(char* texto,int x,int y);
		void imprimir_numero(int numero,int x,int y);
	
		int preguntar_evento();
		SDL_Event obtener_evento(){return evento;}
};

#endif
