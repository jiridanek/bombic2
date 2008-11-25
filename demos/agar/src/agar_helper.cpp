
#include <iostream>
#include "agar_helper.h"

void agar_init(SDL_Surface * window, Uint16 fontsize, const char * colors){
	/* Initialize Agar-core. */
	if(AG_InitCore("agar-bombic2", 0) == -1) {
		std::cerr << AG_GetError() << std::endl;
		throw 1;
	}
	AG_SetInt(agConfig, "font.size", fontsize);

	/* Initialize Agar-GUI. */
	if(AG_InitVideoSDL(window, 0) == -1) {
		std::cerr << AG_GetError() << std::endl;
		throw 1;
	}

	/* Load colors */
	if(colors && AG_ColorsLoad(colors) == -1){
		std::cerr << AG_GetError() << std::endl;
		throw 1;
	}
}

void setFocus(AG_Widget *w){
	if ((w->flags & AG_WIDGET_FOCUSED) == 0) {
		AG_WidgetFocus(w);
		AG_WindowFocus(AG_WidgetParentWindow(w));
	}
}

void unsetFocus(AG_Widget *w){
	if (w->flags & AG_WIDGET_FOCUSED)
		AG_WidgetUnfocus(w);
}

void handlerMousemotion(AG_Event *event) {
	AG_Widget *w = AGWIDGET(AG_SELF());
	int x = AG_INT(1);
	int y = AG_INT(2);

	if( AG_WidgetSensitive(w, w->rView.x1+x, w->rView.y1+y) )
		setFocus(w);
	else
		unsetFocus(w);
}
