
#include <iostream>
#include <string>
#include <ctime>    // For time()
#include <cstdlib>  // For srand() and rand()
#include "agar_helper.h"
#include "SDL_lib.h"
#include "config.h"
#include "menu_base.h"
#include "menu_main.h"
#include "game_intro.h"

using namespace std;


SDL_Surface * g_window=0;
Fonts * g_font=0;
GameIntro * g_gameIntro=0;

int main(int argc, char *argv[]) {
	Surface window;
	srand(time(0));

	int return_val=0;
	try {
		window_init(&g_window, 800, 600, "Bombic2");
		window = g_window;

		agar_init(g_window, 15, argc>1 ? argv[1] : 0);

		string fontfile = "fonts/verdana.ttf";
		if(!locate_file("", fontfile, fontfile))
			throw fontfile+ ": No such file or directory";
		Fonts font(fontfile.c_str());
		g_font = &font;

		Config config;

		GameIntro gameIntro;
		g_gameIntro = &gameIntro;

		try {
			MenuMain::create();
			AG_EventLoop();
		}
		catch(int val){
			return_val = val;
		}
		catch(const string & err){
			return_val = 10;
			cerr << err << endl;
		}

		MenuBase::clearStack();
	}
	catch(int val){
		return_val = val;
	}
	catch(const string & err){
		return_val = 10;
		cerr << err << endl;
	}

	AG_Destroy();
	return return_val;
}

