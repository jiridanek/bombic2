
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
	SDL_Surface *window;
	window_init(&window, 640, 480, "Bombic2 - Demo 1");

	// iterace dokud neni vyvolano zavreni okna
	while(!get_event_isquit(SDLK_ESCAPE)) {

		// vyprazdneni celeho okna
		clear_surface(0,0, colors[ZLUTA], window, window);

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
	return 0;
}
