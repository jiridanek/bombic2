
#include <iostream>
#include <ctime>    // For time()
#include <cstdlib>  // For srand() and rand()

using namespace std;

#include "agar_helper.h"
#include "SDL_lib.h"
#include "stl_helper.h"
#include "config.h"
#include "game_intro.h"

Fonts * g_font;
GameIntro g_gameIntro;
SDL_Surface *g_window=0;

int main(int argc, char ** argv){
	srand(time(0));

	int return_val=0;
	try {
		Uint16 w= 1024, h=768, players=1;
		switch(argc){
			case 1:
				break;
			case 2:
				string2x(argv[1], players);
				break;
			case 3:
				string2x(argv[1], w);
				string2x(argv[2], h);
				break;
			case 4:
			default:
				string2x(argv[1], players);
				string2x(argv[2], w);
				string2x(argv[3], h);
				break;
		}
		window_init(&g_window, w, h, "Bombic2 - Demo 3");
		agar_init(g_window, 15);
		Fonts font("fonts/verdana.ttf");
		g_font = &font;
		Config config;

		g_gameIntro.new_game(0, players);
		g_gameIntro.show_screen();

	}
	catch(int val){
		return_val = val;
	}
	catch(const string & err){
		return_val = 10;
		cerr << err << endl;
	}
	if(g_window) SDL_FreeSurface(g_window);
	return return_val;
}

