
#include "menu_base.h"
#include "agar_helper.h"

MenuStack MenuBase::menu_stack;

MenuBase::MenuBase(){
	win = AG_WindowNew(AG_WINDOW_PLAIN);
	AG_WindowMaximize(win);

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
}
void MenuBase::hide(){
	AG_WindowHide(win);
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
void MenuBase::handlerBack(){
	menu_stack.remove();
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
