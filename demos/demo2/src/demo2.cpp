
#include <iostream>
#include <string>
#include <ctime>    // For time()
#include <cstdlib>  // For srand() and rand()

using namespace std;

#include "SDL_lib.h"
#include "game.h"


int main(int argc, char ** argv){
	srand(time(0));
	// konstrukce zarizujici spravny pocet obrazku za sekundu
	Uint8 fps=64;
	Uint32 fps_last= SDL_fps(0,fps);
	// stav klavesnice
	Uint8 *keystate = SDL_GetKeyState(0);

	Game game(1, "map_forest");

	// vytvoreni hlavniho okna
	SDL_Surface *window, *background, *bgrect;
	window_init(&window, 800, 600, "Bombic2 - Demo 2");

	// vytvorit obdelnickovane pozadi
	background = create_surface(640, 480, Colors::navy());

	// iterace dokud neni vyvolano zavreni okna
	while(!get_event_isquit(SDLK_ESCAPE)) {

		game.play();
		// vyprazdneni celeho okna
		draw_surface(0,0, background, window);
		game.draw(window);

		// obnoveni stavu klavesnice
		SDL_PumpEvents();// obnoveni stavu klavesnice

		// cekani - chceme presny pocet obrazku za sekundu
		fps_last= SDL_fps(fps_last,fps);
		// zobrazeni na obrazovku
		SDL_Flip(window);
	}

	SDL_FreeSurface(window);
	SDL_FreeSurface(background);
	return 0;
}

