
#include "menu_base.h"
#include "agar_helper.h"

MenuStack MenuBase::menu_stack;
MenuBase::items_t MenuBase::items;

MenuBase::MenuBase(){
	win = AG_WindowNew(AG_WINDOW_PLAIN);
	AG_WindowMaximize(win);

	AG_SetEvent(win, "window-keydown", handlerItems, 0);

	AG_WindowSetPadding( win,
		(AGWIDGET(win)->w - MENU_ITEM_WIDTH) /2,
		(AGWIDGET(win)->w - MENU_ITEM_WIDTH) /2,
		MENU_OFFSET, 0);

	menu_stack.insert(this);
}

MenuBase::~MenuBase(){
	AG_ViewDetach(win);
}

void MenuBase::show(){
	AG_WindowShow(win);
	items = items_;
}
void MenuBase::hide(){
	AG_WindowHide(win);
	items.clear();
}

void MenuBase::createHeading(const char * text){
	AG_Label * label;

	label = AG_LabelNew(win, AG_LABEL_HFILL, text);

	AG_LabelJustify(label, AG_TEXT_CENTER);
	AG_LabelValign(label, AG_TEXT_MIDDLE);

	AG_SeparatorNewHoriz(win);
}

AG_Box * MenuBase::createItem(const char * text){
	AG_Box * box;
	box = AG_BoxNewVert(win, AG_BOX_FRAME | AG_BOX_HFILL);

	AGWIDGET(box)->flags |= AG_WIDGET_FOCUSABLE|AG_WIDGET_UNFOCUSED_MOTION;
	AG_SetEvent(box, "window-mousemotion", handlerMousemotion, 0);
	AG_SetEvent(box, "window-keydown", handlerItems, 0);

	items_.push_back(box);
	items.push_back(box);

	if(text){
		AG_Label * label;
		label = AG_LabelNewString(box, AG_LABEL_HFILL, text);

		AG_LabelJustify(label, AG_TEXT_CENTER);
		AG_LabelValign(label, AG_TEXT_MIDDLE);
	}

	return box;
}

void MenuBase::handlerBack(AG_Event * ev){
	menu_stack.remove();
}

void MenuBase::handlerItems(AG_Event * event){
	Uint16 active = activeItem(), activate = 0;

	switch(AG_SDLKEY(1)){
		case SDLK_RETURN:
		case SDLK_KP_ENTER:
		case SDLK_SPACE:
			AG_PostEvent(AG_SELF(), AG_SELF(), "window-mousebuttondown", 0);
			return;
		case SDLK_ESCAPE:
			handlerBack();
			return;
		case SDLK_UP:
			if(active>0) activate = active-1;
			else activate = items.size()-1;
			break;
		case SDLK_DOWN:
			activate = active+1;
			break;
		default:
			return;
	}

	if(active < items.size())
		unsetFocus(AGWIDGET(items[active]));
	if(activate >= items.size())
		activate = 0;
	if(!items.empty())
		setFocus(AGWIDGET(items[activate]));
}

Uint16 MenuBase::activeItem(){
	Uint16 i;
	for(i = 0 ; i<items.size() ; ++i){
		if(AG_WidgetFocused(items[i]))
			break;
	}
	return i;
}

void MenuBase::handlerIntItem(AG_Event * event){
	int * val = static_cast<int *>(AG_PTR(1));
	int min = AG_INT(2), max = AG_INT(3);
	SDLKey key = AG_SDLKEY(4);
	switch(key){
		case SDLK_RIGHT:
		case SDLK_PLUS:
		case SDLK_KP_PLUS:
			if(*val < max) ++(*val);
			break;
		case SDLK_LEFT:
		case SDLK_MINUS:
		case SDLK_KP_MINUS:
			if(*val > min) --(*val);
			break;
		case SDLK_RETURN:
		case SDLK_KP_ENTER:
		case SDLK_SPACE:
			++(*val);
			if(*val > max) *val = min;
			break;
		case SDLK_BACKSPACE:
			--(*val);
			if(*val < min) *val = max;
			break;
		default:
			if(key>SDLK_0 && key<SDLK_9
			&& key-SDLK_0>=min && key-SDLK_0<=max)
				*val = key-SDLK_0;
			else if(key>SDLK_KP0 && key<SDLK_KP9
			&& key-SDLK_KP0>=min && key-SDLK_KP0<=max)
				*val = key-SDLK_KP0;
			break;
	}
}

void MenuBase::clearStack(){
	menu_stack.clear();
}

void MenuStack::insert(MenuBase * menu){
	if(!stack_.empty())
		stack_.top()->hide();
	stack_.push(menu);
	stack_.top()->show();
}

void MenuStack::remove(){
	delete stack_.top();
	stack_.pop();
	if(!stack_.empty())
		stack_.top()->show();
}

void MenuStack::clear(){
	while(!stack_.empty())
		remove();
}
