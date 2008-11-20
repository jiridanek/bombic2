
#include <iostream>
#include <string>
#include <agar/core.h>
#include <agar/gui.h>
#include "SDL_lib.h"
#include "stl_helper.h"

void mousemotion(AG_Event *event) {
	AG_Widget *w = static_cast<AG_Widget *>(AG_SELF());
	int x = AG_INT(1);
	int y = AG_INT(2);

	if (AG_WidgetSensitive(w, w->rView.x1+x, w->rView.y1+y)) {
		if ((w->flags & AG_WIDGET_FOCUSED) == 0) {
			AG_WidgetFocus(w);
			AG_WindowFocus(AG_WidgetParentWindow(w));
		}
	} else {
		if (w->flags & AG_WIDGET_FOCUSED) {
			AG_WidgetUnfocus(w);
		}
	}
}

void CreateItem(AG_Fixed * parent, AG_Rect * rect, const char * text){
	AG_Fixed * box;
	AG_Label * label;

	box = AG_FixedNew(parent, AG_FIXED_BOX);
	AGWIDGET(box)->flags |= AG_WIDGET_FOCUSABLE|AG_WIDGET_UNFOCUSED_MOTION;
	AG_FixedMove(parent, box, rect->x, rect->y);
	AG_FixedSize(parent, box, rect->w, rect->h);

	label = AG_LabelNew(box, 0, text);
	AG_Expand(label);
	AG_LabelJustify(label, AG_TEXT_CENTER);
	AG_LabelValign(label, AG_TEXT_MIDDLE);

	AG_SetEvent(box, "window-mousemotion", mousemotion, 0);

}

void CreateWindow() {
	AG_Window *win;
	AG_Fixed *fixed;
	AG_Rect rect;

	win = AG_WindowNew(AG_WINDOW_PLAIN);
	AG_WindowMaximize(win);


	fixed = AG_FixedNew(win, AG_FIXED_FRAME);
	AG_Expand(fixed);

	int text_w, text_h, items, i;
	std::string text_s("item ");
	AG_TextSize( (text_s+"100").c_str(), &text_w, &text_h);

	rect.w = text_w+text_h*4;
	rect.h = text_h*2;
	rect.x = (AGWIDGET(win)->w-rect.w) /2;

	items = AGWIDGET(win)->h / rect.h -2;
	for(rect.y = rect.h, i=0 ; i < items  ; rect.y+=rect.h, ++i){
		CreateItem(fixed, &rect, (text_s+x2string(i)).c_str());
	}

	AG_WindowShow(win);
}

int main(int argc, char *argv[]) {
	SDL_Surface *window;

	window_init(&window, 1024, 768, "Bombic2 - Agar demo");

	/*
	 * Agar Initialization.
	 */
	if (AG_InitCore("agar-bombic2-demo", 0) == -1) {
		std::cerr << AG_GetError() << std::endl;
		return (1);
	}
	AG_SetInt(agConfig, "font.size", 20);

	/* Initialize Agar-GUI. */
	if (AG_InitVideoSDL(window, 0) == -1) {
		std::cerr << AG_GetError() << std::endl;
		return (-1);
	}

	/* Load colors */
	if(argc > 1 && AG_ColorsLoad(argv[1]) == -1){
		std::cerr << AG_GetError() << std::endl;
		return (-1);
	}

	/* Bind some useful accelerator keys. */
	AG_BindGlobalKey(SDLK_ESCAPE, KMOD_NONE, AG_Quit);

	/* Create our test window. */
	CreateWindow();

	AG_EventLoop();
	AG_Destroy();
	return (0);
}

