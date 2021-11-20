#include "sprite.h"

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

/////////////////////////  FRAME  //////////////////////////
void Frame::cargar(char path[]) {
	img=IMG_Load(path);			//carga el gráfico del frame
	
	// Asignamos el color transparencia (TURQUESA)
	SDL_SetColorKey(img,SDL_SRCCOLORKEY|SDL_RLEACCEL, SDL_MapRGB(img->format,0,222,222));
	
	img=SDL_DisplayFormat(img);
}

void Frame::liberar(){
	SDL_FreeSurface(img); 		//libera los recursos de la superficie.
}
/////////////////////////////////////////////////////////////



/////////////////////////  SPRITE  /////////////////////////
Sprite::Sprite() {
	int nf=5;
	sprite=new Frame[nf];
	nframes=nf;
	cont=0;
}

Sprite::Sprite(int nf) {
	sprite=new Frame[nf];
	nframes=nf;
	cont=0;
}

void Sprite::finalizar() {
	int i;
	for (i=0 ; i<=nframes-1 ; i++)
		sprite[i].liberar();
}

void Sprite::agregar_frame(Frame f) {
	if (cont<nframes){
		sprite[cont]=f;
		cont++;
	}
}

//selecciona el frame actual
void Sprite::seleccionar_frame(int nf) {
if (nf<=nframes) 
	estado=nf;
}

void Sprite::dibujar(SDL_Surface *superficie) {
	SDL_Rect dest;
	
	dest.x=posx;
	dest.y=posy;
	SDL_BlitSurface(sprite[estado].img,NULL,superficie,&dest);
}

int Sprite::colision(Sprite jug,int ajustex,int ajustey) {
	int w1,h1,w2,h2,x1,y1,x2,y2;
	
	w1=getw();			//ancho 
	h1=geth(); 			//altura 
	x1=getx()-ajustex;	//pos. x
	y1=gety()-ajustey;	//pos. y del Enemigo
	
	w2=jug.getw();	//ancho 
	h2=jug.geth();	//alto 
	x2=jug.getx();	//pos. x
	y2=jug.gety();	//pos. y del Jugador

	if (((x1+w1)>x2)&&((y1+h1)>y2)&&((x2+w2)>x1)&&((y2+h2)>y1)) 
		return TRUE;
	else 
		return FALSE;
}

int Sprite::colision_agua(Sprite jug,int ajuste) {
	int w1,h1,w2,h2,x1,y1,x2,y2;
	
	w1=getw()-(2*ajuste);//ancho 
	h1=geth(); 			//altura 
	x1=getx()+ajuste;	//pos. x
	y1=gety();			//pos. y del Enemigo
	
	w2=jug.getw();	//ancho 
	h2=jug.geth();	//alto 
	x2=jug.getx();	//pos. x
	y2=jug.gety();	//pos. y del Jugador

	if (((x1+w1)>x2)&&((y1+h1)>y2)&&((x2+w2)>x1)&&((y2+h2)>y1)) 
		return TRUE;
	else 
		return FALSE;
}
/////////////////////////////////////////////////////////////



////////////////////////  JUGADORES  ////////////////////////
Jugadores::Jugadores() {
	int nf=0;				// num_frame actual
	nframe=nf;
}
/////////////////////////////////////////////////////////////



////////////////////////  ENEMIGOS  ////////////////////////
Enemigos::Enemigos() {
	int nf=0;				// num_frame actual
	nframe=nf;
}
/////////////////////////////////////////////////////////////



/////////////////////////  AMIGOS  /////////////////////////
Amigos::Amigos() {
	int nf=0;				// num_frame actual
	nframe=nf;
}
/////////////////////////////////////////////////////////////
