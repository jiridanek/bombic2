
#include <iostream>
#include <ctime>    // For time()
#include <cstdlib>  // For srand() and rand()

using namespace std;

#include "SDL_lib.h"
#include "stl_helper.h"
#include "config.h"
#include "game_intro.h"

Fonts g_font("fonts/verdana.ttf");
GameIntro g_gameIntro;
SDL_Surface *g_window=0;

int main(int argc, char ** argv){
	srand(time(0));

	int return_val=0;
	try {
		Uint16 w= 1024, h=768;
		if(argc>=3){
			string2x(argv[1], w);
			string2x(argv[2], h);
		}
		window_init(&g_window, w, h, "Bombic2 - Demo 3");

		Config config;

		g_gameIntro.new_game(1, 4);
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

