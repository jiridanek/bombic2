
#include <iostream>
#include <string>
#include "SDL_lib.h"
#include "stl_helper.h"
#include "agar_helper.h"
#include "menu_base.h"
#include "menu_main.h"

using namespace std;

/*
AG_Box * CreateItem(AG_Widget * parent, const char * text){
	AG_Box * box;
	box = AG_BoxNewVert(parent, AG_BOX_FRAME | AG_BOX_HFILL);

	AGWIDGET(box)->flags |= AG_WIDGET_FOCUSABLE|AG_WIDGET_UNFOCUSED_MOTION;
	AG_SetEvent(box, "window-mousemotion", mousemotion, 0);

	if(text){
		AG_Label * label;
		label = AG_LabelNewString(box, AG_LABEL_HFILL, text);

		AG_LabelJustify(label, AG_TEXT_CENTER);
		AG_LabelValign(label, AG_TEXT_MIDDLE);
	}

	return box;
}

int g_speed=1;

void CreateSpeedSlider(AG_Box * parent){
	AG_SpacerNewHoriz(parent);
	AG_SliderNewIntR(parent, AG_SLIDER_HORIZ, AG_SLIDER_HFILL, &g_speed, 1, 9);
}

void CreateWindow() {
	AG_Window *win;
	AG_Box * box;

	win = AG_WindowNew(AG_WINDOW_PLAIN);
	AG_WindowMaximize(win);

	int text_w, text_h, items, i;
	string text_s("item ");
	AG_TextSize( (text_s+"100").c_str(), &text_w, &text_h);

	text_w+= 2*50;
	text_h*= 2;
	AG_WindowSetPadding(win,
		(AGWIDGET(win)->w-text_w) /2, (AGWIDGET(win)->w-text_w) /2,
		text_h, text_h);

	CreateHeading(AGWIDGET(win), "Menu");

	items = 6;
	for(i=0 ; i < items  ; ++i){
		box = CreateItem(AGWIDGET(win), (text_s+x2string(i)).c_str());
		if(i==2){
			CreateSpeedSlider(box);
		}
	}
	AG_SpacerNewHoriz(win);
	CreateItem(AGWIDGET(win), "Back");

	AG_WindowShow(win);
// 	AG_ViewDetach(win);
}
*/

int main(int argc, char *argv[]) {
	SDL_Surface *window;

	window_init(&window, 1024, 768, "Bombic2 - Agar demo");
	agar_init(window, 15, argc>1 ? argv[1] : 0);

	/* Bind some useful accelerator keys. */
// 	AG_BindGlobalKey(SDLK_ESCAPE, KMOD_NONE, AG_Quit);

	MenuMain::create();

	AG_EventLoop();
	// TODO catch all exceptions

	MenuBase::clearStack();
	AG_Destroy();
	SDL_FreeSurface(window);
	return 0;
}

