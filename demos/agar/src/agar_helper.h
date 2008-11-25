/** @file agar_helper.h
 * Soubor obsahující pomocné funkce ke knihovně agar-gui.
 */

#ifndef AGAR_HELPER_H
#define AGAR_HELPER_H

#include <agar/core.h>
#include <agar/gui.h>
#include "SDL.h"

void agar_init(SDL_Surface * window, Uint16 fontsize, const char * colors=0);

void setFocus(AG_Widget *w);

void unsetFocus(AG_Widget *w);

void handlerMousemotion(AG_Event *event);

#endif
