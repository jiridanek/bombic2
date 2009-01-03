
#include <iostream>
#include <string>
#include <ctime>    // For time()
#include <cstdlib>  // For srand() and rand()
#include "agar_helper.h"
#include "tixml_helper.h"
#include "constants.h"
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

		string font_path, font_name(FONT_NAME), agar_theme(AGAR_THEME);
		if(!locate_file("", font_name, font_name))
			throw font_name+ ": No such file or directory";
		string::size_type pos = font_name.rfind('/');
		font_path= font_name.substr(0, pos);
		font_name = font_name.substr(pos+1);
		if(!locate_file("", agar_theme, agar_theme))
			agar_theme = "";
		agar_init(g_window, font_path.c_str(), font_name.c_str(),
			FONT_SIZE, agar_theme.empty() ? 0 : agar_theme.c_str());

		font_path+="/"+font_name;
		Fonts font(font_path.c_str());
		g_font = &font;

		Config config;
		if(config.fullscreen())
			SDL_WM_ToggleFullScreen(g_window);

		GameIntro gameIntro;
		g_gameIntro = &gameIntro;

		MenuMain::create();
		AG_EventLoop();
		MenuBase::clearStack();
	}
	catch(int val){
		return_val = val;
	}
	catch(const string & err){
		return_val = 10;
		cerr << err << endl;
	}
	catch(const TiXmlException & ex){
		return_val = 1;
		cerr << ex.what() << endl;
	}

	AG_Destroy();
	return return_val;
}

