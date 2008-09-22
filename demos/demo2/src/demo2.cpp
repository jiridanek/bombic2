
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
// 	Uint8 fps=10;
// 	Uint32 fps_last= SDL_fps(0,fps);
	// stav klavesnice
	Uint8 *keystate = SDL_GetKeyState(0);

	// vytvoreni hlavniho okna
	SDL_Surface *window=0, *pause=0;

	int return_val=0;
	try {
		window_init(&window, 800, 600, "Bombic2 - Demo 2");

		// text napisu
		pause = get_text(g_font[15], "PAUSE", Color::white);

		Game game(1, "map_forest_debug");

		int last_time = SDL_GetTicks();
		int time_to_use = 0, this_time=last_time;
		// iterace dokud neni vyvolano zavreni okna
		while(!get_event_isquit(SDLK_ESCAPE)) {
			SDL_PumpEvents();// obnoveni stavu klavesnice
			// pauza
			if(keystate[SDLK_SPACE]){
				draw_surface(window->h/2, window->w/2, pause, window);
				SDL_Flip(window);
				while(!wait_event_isquit(SDLK_SPACE)) ;
				SDL_Delay(500);
			}

			game.draw(window);

			// cekani - chceme presny pocet obrazku za sekundu
// 			fps_last= SDL_fps(fps_last,fps);
			// zobrazeni na obrazovku
			SDL_Flip(window);

			this_time = SDL_GetTicks();
			time_to_use += this_time - last_time;
// 			cout << time_to_use << endl;
			for(; time_to_use > 10; time_to_use -= 10){
				game.play();
			}
			last_time = this_time;
		}

	}
	catch(int val){
		return_val = val;
	}
	if(window) SDL_FreeSurface(window);
	if(pause) SDL_FreeSurface(pause);
	return return_val;
}

