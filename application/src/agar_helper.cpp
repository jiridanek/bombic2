
#include <stdlib.h> // for getenv
#include <iostream>
#include "agar_helper.h"

using namespace std;

void agar_init(SDL_Surface * window, Uint16 fontsize, const char * colors){
	/* Initialize Agar-core. */
	if(AG_InitCore("agar-bombic2", 0) == -1) {
		cerr << AG_GetError() << endl;
		throw 1;
	}
	AG_SetInt(agConfig, "font.size", fontsize);

	/* Initialize Agar-GUI. */
	if(AG_InitVideoSDL(window, 0) == -1) {
		cerr << AG_GetError() << endl;
		throw 1;
	}

	/* Load colors */
	if(colors && AG_ColorsLoad(colors) == -1){
		cerr << AG_GetError() << endl;
		throw 1;
	}

	AG_ResizeDisplay(window->w, window->h);
}

AG_Label * createLabelJustify(AG_Widget * parent, const char * text){
	AG_Label * label;
	label = AG_LabelNewString(parent, AG_LABEL_HFILL, text);

	AG_LabelJustify(label, AG_TEXT_CENTER);
	AG_LabelValign(label, AG_TEXT_MIDDLE);
	return label;
}

void setFocus(AG_Widget *w){
	if( (w->flags & AG_WIDGET_FOCUSED) == 0
	&& AG_WindowIsFocused(AG_WidgetParentWindow(w)) ) {
		AG_WidgetFocus(w);
	}
}

void unsetFocus(AG_Widget *w){
	if (w->flags & AG_WIDGET_FOCUSED)
		AG_WidgetUnfocus(w);
}

void setFocusOnMotion(AG_Widget *w){
	w->flags |= AG_WIDGET_FOCUSABLE|AG_WIDGET_UNFOCUSED_MOTION;
	AG_SetEvent(w, "window-mousemotion", handlerMousemotion, 0);
}

void handlerMousemotion(AG_Event *event) {
	AG_Widget *w = AGWIDGET(AG_SELF());
	int x = AG_INT(1);
	int y = AG_INT(2);

	if( AG_WidgetSensitive(w, w->rView.x1+x, w->rView.y1+y) )
		setFocus(w);
}


/****************** SEARCHING of file ****************/

bool is_dir(const std::string & path){
	AG_FileInfo file_info;
	if(AG_GetFileInfo(path.c_str(), &file_info)==-1)
		return false;
	return file_info.type==AG_FILE_DIRECTORY
		&& file_info.perms & AG_FILE_READABLE;
}

bool is_file(const std::string &path){
	return AG_FileExists( path.c_str() )!=0;
}

static bool locate_in_dir(int depth, const std::string & path,
		const std::string & name, std::string & res){

	if(++depth>SEARCH_DEPTH)
		return false;

	if( is_file(path + "/" + name)){
		res = path+"/"+name;
		return true;
	}

	AG_Dir * dir = AG_OpenDir(path.c_str());
	if(dir == 0)
		return false;
	bool located = false;

	string dir_name;
	for( int i = 0 ; i<dir->nents && !located ; ++i){
		dir_name = dir->ents[i];
		if(dir_name[0]=='.')
			continue;
		dir_name = path + "/" + dir_name;
		if(is_dir(dir_name))
			located = locate_in_dir(depth, dir_name, name, res);
	}
	AG_CloseDir(dir);
	return located;
}

static bool locate_in_path(const std::string & path, const std::string & name,
				std::string &res){
	if(is_dir(path))
		return locate_in_dir(0, path, name, res);

	return false;
}

bool locate_file(const std::string & hint_path, const std::string & name,
		std::string & res){
	if(name.empty()) return false;
	if(name[0]=='/') {
		if(is_file(name)){
			res = name;
			return true;
		} else
			return false;
	}

	if(!hint_path.empty()
	&& locate_in_path(hint_path, name, res))
		return true;

	string home_path;
	get_home_path(home_path);
	if(!home_path.empty()
	&& locate_in_path(home_path, name, res))
		return true;

	const char * paths[] = SEARCH_PATHS;
	for( int i=0 ; paths[i] != 0 ; ++i ){
		if(locate_in_path(paths[i], name, res))
			return true;
	}
	return false;
}

#include <iostream>
void get_home_path(std::string & path) {
	// TODO make something for windows
	const char * home = getenv("HOME");
	if(home == 0)
		path = "";
	else {
		path = home;
		path+= "/";
		path+= SEARCH_HOME;
	}
}
