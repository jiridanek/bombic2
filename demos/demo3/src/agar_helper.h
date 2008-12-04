/** @file agar_helper.h
 * Soubor obsahující pomocné funkce ke knihovně agar-gui.
 */

#ifndef AGAR_HELPER_H
#define AGAR_HELPER_H

#include <string>
#include <agar/core.h>
#include <agar/gui.h>
#include "SDL.h"

void agar_init(SDL_Surface * window, Uint16 fontsize, const char * colors=0);

AG_Label * createLabelJustify(AG_Widget * parent, const char * text);

void setFocus(AG_Widget *w);

void unsetFocus(AG_Widget *w);

void setFocusOnMotion(AG_Widget *w);

void handlerMousemotion(AG_Event *event);


#define SEARCH_HOME ".bombic"
#define SEARCH_PATHS { ".", 0 }
#define SEARCH_DEPTH 5

bool is_dir(const std::string & path);
bool is_file(const std::string & path);

bool locate_file(const std::string & hint_path,
		const std::string & name, std::string & res);

void get_home_path(std::string & path);

#endif
