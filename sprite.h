#ifndef SPRITE_H_
#define SPRITE_H_
#define TRUE 1
#define FALSE 0

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

/////////////////////////  FRAME  //////////////////////////
// representa un frame independiente de un sprite.
class Frame {
	public:
		SDL_Surface *img;
		void cargar(char *path);
		void liberar();
};
////////////////////////////////////////////////////////////



/////////////////////////  SPRITE  /////////////////////////
// está formada por un array de frames;
class Sprite {
	private:
		int posx,posy;
		int estado; 	//frame actual
		int nframes;   	//cantidad de frames del sprite
		int cont;
		
	public:
		Frame *sprite;
		Sprite(int nf);
		Sprite();
		void finalizar();
		void agregar_frame(Frame f);
		void seleccionar_frame(int nf);
		int frames() {return cont;}
		void setx(int x) {posx=x;}
		void sety(int y) {posy=y;}
		void set_estado(int e) {estado=e;}
		void addx(int c) {posx+=c;}
		void addy(int c) {posy+=c;}
		int getx() {return posx;}
		int gety() {return posy;}
		int getw() {return sprite[estado].img->w;}
		int geth() {return sprite[estado].img->h;}
		int get_estado() {return estado;}
		void dibujar(SDL_Surface *superficie);
		int colision(Sprite jug,int ajustex,int ajustey);
		int colision_agua(Sprite jug,int ajuste);
};
////////////////////////////////////////////////////////////



////////////////////////  JUGADORES  ///////////////////////
class Jugadores {
	private:
		int x,y;	//posicion
		int nframe;
		int nvidas;
		int activo;
		int estado;  //0=reposo,1=movimiento,2=atropellado;3=ahogado
		int nidos;
	public:
		Jugadores();
		Jugadores(int nf);
		void setx(int x) {this->x=x;}
		void sety(int y) {this->y=y;}
		void set_nframe(int nf) {nframe=nf;}
		void set_nvidas(int nv) {nvidas=nv;}
		void set_activo(int a) {activo=a;}
		void set_estado(int e) {estado=e;}
		void set_nidos(int n) {nidos=n;}
		int getx() {return x;}
		int gety() {return y;}
		int get_nframe() {return nframe;}
		int get_nvidas() {return nvidas;}
		int get_activo() {return activo;}
		int get_estado() {return estado;}
		int get_nidos() {return nidos;}
		void sumar_nidos() {nidos++;}
		void restar_vida() {nvidas--;}
};
//////////////////////////////////////////////////////////



////////////////////////  ENEMIGOS  ////////////////////////
class Enemigos {
	private:
		int x,y;	//posicion
		int vel;	//velocidad del sprite
		int nframe; 
	public:
		Enemigos();
		Enemigos(int nf);
		void setx(int x) {this->x=x;}
		void sety(int y) {this->y=y;}
		void set_vel(int v) {vel=v;}
		void set_nframe(int nf) {nframe=nf;}
		int getx() {return x;}
		int gety() {return y;}
		int get_vel() {return vel;}
		int get_nframe() {return nframe;}
		int colision(Jugadores jug);
};
////////////////////////////////////////////////////////////



/////////////////////////  AMIGOS  /////////////////////////
class Amigos {
	private:
		int x,y;	//posicion
		int vel;	//velocidad del sprite
		int tipo;  	//tipo de amigo (1=tortuga comun,2=tortuga sumergible,3=tronco)
		int nframe; 
	public:
		Amigos();
		void setx(int x) {this->x=x;}
		void sety(int y) {this->y=y;}
		void set_vel(int v) {vel=v;}
		void set_tipo(int t) {tipo=t;}
		void set_nframe(int nf) {nframe=nf;}
		int getx() {return x;}
		int gety() {return y;}
		int get_vel() {return vel;}
		int get_tipo() { return tipo;}
		int get_nframe() {return nframe;}
};
////////////////////////////////////////////////////////////
#endif
