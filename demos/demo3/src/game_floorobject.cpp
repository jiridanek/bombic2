
#include <iostream>
#include "SDL_lib.h"
#include "constants.h"
// #include "game.h"
#include "game_mapobjects.h"
#include "game_floorobject.h"

/**
 * @param anim animace objektu
 * @param x souřadnice levého rohu v pixelech
 * @param y souřadnice pravého rohu v pixelech
 */
Floorobject::Floorobject(const Animation & anim, Uint16 x, Uint16 y):
	StaticMO(x, y), anim_(anim) {}

Floorobject::Floorobject(const Floorobject & floorobject, Uint16 x, Uint16 y):
	StaticMO(x, y), anim_(floorobject.anim_) {}

/**
 * @param window surface okna pro vykreslení
 */
void Floorobject::draw(SDL_Surface* window, const SDL_Rect & rect){
	anim_.draw(window, x_ +rect.x, y_ +rect.y);
}

void Floorobject::update(){
	anim_.update();
}
