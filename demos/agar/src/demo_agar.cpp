
#include <iostream>
#include <string>
#include "SDL_lib.h"
#include "stl_helper.h"
#include "agar_helper.h"
#include "config.h"
#include "menu_base.h"
#include "menu_main.h"

using namespace std;


SDL_Surface * g_window=0;

int main(int argc, char *argv[]) {

	int return_val=0;
	try {
		window_init(&g_window, 1024, 768, "Bombic2 - Agar demo");
		agar_init(g_window, 15, argc>1 ? argv[1] : 0);
		Config config;

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
	AG_Destroy();
	if(g_window) SDL_FreeSurface(g_window);
	return return_val;
}

