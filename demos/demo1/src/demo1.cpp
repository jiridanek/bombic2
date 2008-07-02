
#include <iostream>

using namespace std;

#include "SDL_lib.h"
#include "bombic.h"




int main(int argc, char ** argv){
		// konstrukce zarizujici spravny pocet obrazku za sekundu
	Uint8 fps=64;
	Uint32 fps_last= SDL_fps(0,fps);
	// stav klavesnice
	Uint8 *keystate = SDL_GetKeyState(0);

	// vytvori postavu bombice
	Bombic bombic(fps);

	Colors colors;
	// vytvoreni hlavniho okna
	SDL_Surface *window, *background, *bgrect;
	window_init(&window, 640, 480, "Bombic2 - Demo 1");

	// vytvorit obdelnickovane pozadi
	background = create_surface(640, 480, colors[ZELENA]);
	bgrect = create_surface(640/8, 480/8, colors[ZLUTA]);
	for(int i=0; i<8; ++i){
		for(int j=0; j<8; ++j){
			if((j+i)%2)
				draw_surface(j*640/8, i*480/8, bgrect, background);
		}
	}
	SDL_FreeSurface(bgrect);

	// iterace dokud neni vyvolano zavreni okna
	while(!get_event_isquit(SDLK_ESCAPE)) {

		// vyprazdneni celeho okna
// 		clear_surface(0,0, colors[ZLUTA], window, window);
		draw_surface(0,0, background, window);

		// obnoveni stavu klavesnice
		SDL_PumpEvents();// obnoveni stavu klavesnice
		// tah bombicem
		bombic.move(keystate, window);
		bombic.draw(window);
		// cekani - chceme presny pocet obrazku za sekundu
		fps_last= SDL_fps(fps_last,fps);
		// zobrazeni na obrazovku
		SDL_Flip(window);
	}

	SDL_FreeSurface(window);
	SDL_FreeSurface(background);
	return 0;
}
