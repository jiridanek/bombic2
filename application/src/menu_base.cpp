
#include "menu_base.h"
#include "agar_helper.h"
#include "language.h"

MenuStack MenuBase::menu_stack;
MenuBase::items_t * MenuBase::p_items = 0;

MenuBase::MenuBase(){
	win = AG_WindowNew(AG_WINDOW_PLAIN);
	AG_WindowMaximize(win);
	AG_WindowSetGeometryAligned(win, AG_WINDOW_MC,
		MENU_ITEM_WIDTH, AGWIDGET(win)->h);

	win->flags = AG_WINDOW_PLAIN | AG_WINDOW_VMAXIMIZE | AG_WINDOW_NOMOVE;

	AG_WindowSetPadding(win, 0, 0, MENU_OFFSET, 0);

	AG_SetEvent(win, "window-keyup", handlerItems, 0);

	menu_stack.insert(this);
}

MenuBase::~MenuBase(){
	AG_ViewDetach(win);
}

void MenuBase::show(){
	AG_WindowSetGeometryAligned(win, AG_WINDOW_MC,
		AGWIDGET(win)->w, AGWIDGET(win)->h);
	AG_WindowShow(win);
	p_items = &items_;
}
void MenuBase::hide(){
	AG_WindowHide(win);
	p_items = 0;
}

void MenuBase::createHeading(const char * text){
	createLabelJustify(AGWIDGET(win), text);
	AG_SeparatorNewHoriz(win);
}

AG_Box * MenuBase::createItem(const char * text){
	AG_Box * box;
	box = AG_BoxNewVert(win, AG_BOX_FRAME | AG_BOX_HFILL);

	setFocusOnMotion( AGWIDGET(box) );
	AG_SetEvent(box, "window-keyup", handlerItems, 0);

	items_.push_back(box);
	if(items_.size()==1)
		setFocus(AGWIDGET(box));

	if(text)
		createLabelJustify(AGWIDGET(box), text);

	return box;
}

AG_Box * MenuBase::createItemHoriz(const char * text){
	AG_Box * box;
	box = createItem(0);

	box = AG_BoxNewHoriz(box, AG_BOX_HOMOGENOUS | AG_BOX_HFILL);
	AG_BoxSetPadding(box, 0);
	AG_LabelNewString(box, 0, text);
	return box;
}

AG_Checkbox * MenuBase::createCheckboxItem(const char * text, int * value){
	AG_Box * box;
	box = createItemHoriz(text);
	AG_AddEvent(items_.back(), "window-mousebuttondown", handlerBoolItem,
		"%p", value);

	box = AG_BoxNewHoriz(box, AG_BOX_HOMOGENOUS | AG_BOX_HFILL);
	AG_BoxSetPadding(box, 0);
	AG_SpacerNewVert(box);
	return AG_CheckboxNewInt(box, 0, "  ", value);
}

AG_FileDlg * MenuBase::createFileDlg(bool load, const char * path,
			const char * menu, const char * submenu){
	AG_FileDlg * file_dlg =
		AG_FileDlgNew(win, load ? AG_FILEDLG_LOAD : AG_FILEDLG_SAVE);
	// popisky
	AG_ObjectDelete(file_dlg->lbCwd);
	file_dlg->lbCwd = AG_LabelNewPolled(file_dlg, AG_LABEL_HFILL, "%s: %s",
		LANG_MENU_TRY(menu, submenu, LANG_DIRECTORY), &file_dlg->cwd[0]);
	AG_TextboxSetLabel(file_dlg->tbFile, "%s: ",
		LANG_MENU_TRY(menu, submenu, LANG_FILE));
	AG_TextboxSetLabel(file_dlg->comTypes->tbox, "%s: ",
		LANG_MENU_TRY(menu, submenu, LANG_TYPE));
	AG_ButtonText(file_dlg->btnOk, "%s",
		LANG_MENU_TRY(menu, submenu, LANG_OK) );
	AG_ButtonText(file_dlg->btnCancel, "%s",
		LANG_MENU_TRY(menu, submenu, LANG_CANCEL) );
	AG_Expand(file_dlg);
	// akce
	AG_FileDlgCancelAction(file_dlg, handlerBack, 0);
	AG_AddEvent(file_dlg->tlDirs, "window-keyup", handlerItems, 0);
	AG_AddEvent(file_dlg->tlFiles, "window-keyup", handlerItems, 0);
	AG_AddEvent(file_dlg->btnOk, "window-keyup", handlerItems, 0);
	AG_AddEvent(file_dlg->btnCancel, "window-keyup", handlerItems, 0);
	// inicialni cesta
	if(path && *path)
		AG_FileDlgSetDirectory(file_dlg, path);
	return file_dlg;
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
			else activate = p_items->size()-1;
			break;
		case SDLK_DOWN:
			activate = active+1;
			break;
		default:
			return;
	}

	if(active < p_items->size())
		unsetFocus(AGWIDGET( p_items->at(active) ));
	if(activate >= p_items->size())
		activate = 0;
	if(!p_items->empty())
		setFocus(AGWIDGET( p_items->at(activate) ));
}

Uint16 MenuBase::activeItem(){
	Uint16 i;
	for(i = 0 ; i<p_items->size() ; ++i){
		if(AG_WidgetFocused( p_items->at(i) ))
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
			if(key>=SDLK_0 && key<=SDLK_9
			&& key-SDLK_0>=min && key-SDLK_0<=max)
				*val = key-SDLK_0;
			else if(key>=SDLK_KP0 && key<=SDLK_KP9
			&& key-SDLK_KP0>=min && key-SDLK_KP0<=max)
				*val = key-SDLK_KP0;
			break;
	}
}

void MenuBase::handlerBoolItem(AG_Event * event){
	int * val = static_cast<int *>(AG_PTR(1));
	*val = ! *val;
}

void MenuBase::clearStack(){
	menu_stack.clear();
}

/************* MenuStack ***************/

void MenuStack::insert(MenuBase * menu){
	if(!stack_.empty())
		stack_.top()->hide();
	stack_.push(menu);
	stack_.top()->show();
}

void MenuStack::remove(bool quit_on_empty){
	delete stack_.top();
	stack_.pop();
	if(!stack_.empty())
		stack_.top()->show();
	else if(quit_on_empty)
		AG_Quit();
}

void MenuStack::clear(){
	while(!stack_.empty())
		remove();
}
