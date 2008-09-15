
#include <iostream>
#include <string>
#include <ctime>    // For time()
#include <cstdlib>  // For srand() and rand()

using namespace std;

#include "SDL_lib.h"
#include "game.h"

Fonts g_font("fonts/verdana.ttf");

int main(int argc, char ** argv){
	srand(time(0));
	// konstrukce zarizujici spravny pocet obrazku za sekundu
	Uint8 fps=8;
	Uint32 fps_last= SDL_fps(0,fps);
	// stav klavesnice
	Uint8 *keystate = SDL_GetKeyState(0);

	// vytvoreni hlavniho okna
	SDL_Surface *window=0, *background=0, *pause=0;

	int return_val=0;
	try {
		window_init(&window, 800, 600, "Bombic2 - Demo 2");

		// vytvorit pozadi
		background = create_surface(640, 480, Color::navy);
		// text napisu
		pause = get_text(g_font[15], "PAUSE", Color::white);

		Game game(1, "map_forest_debug");

		// iterace dokud neni vyvolano zavreni okna
		while(!get_event_isquit(SDLK_ESCAPE)) {
			// obnoveni stavu klavesnice
			SDL_PumpEvents();// obnoveni stavu klavesnice
			// pauza
			if(keystate[SDLK_SPACE]){
				draw_surface(window->h/2, window->w/2, pause, window);
				SDL_Flip(window);
				while(!wait_event_isquit(SDLK_SPACE)) ;
				SDL_Delay(500);
			}

			game.play();
			// vyprazdneni celeho okna
			draw_surface(0,0, background, window);
			game.draw(window);

			// cekani - chceme presny pocet obrazku za sekundu
			fps_last= SDL_fps(fps_last,fps);
			// zobrazeni na obrazovku
			SDL_Flip(window);
		}
	}
	catch(int val){
		return_val = val;
	}
	if(window) SDL_FreeSurface(window);
	if(background) SDL_FreeSurface(background);
	if(pause) SDL_FreeSurface(pause);
	return return_val;
}

