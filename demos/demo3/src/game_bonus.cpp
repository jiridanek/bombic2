
#include <iostream>
#include "SDL_lib.h"
#include "constants.h"
#include "game.h"
#include "game_mapobjects.h"
#include "game_bonus.h"


/**
 * @param anim animace normálního stavu
 * @param x souřadnice levého rohu v pixelech
 * @param y souřadnice horního rohu v pixelech
 */
Bonus::Bonus(const Animation & anim, Uint16 x, Uint16 y):
	DynamicMO(x,y), anim_(anim), visible_(false) {}

Bonus::Bonus(const Bonus & bonus, Uint16 x, Uint16 y):
	DynamicMO(x,y), anim_(bonus.anim_), visible_(false) {}

/** @details
 * Posune frame animace.
 */
void Bonus::move(){
	if(!visible_) return;
	anim_.update();
}

/**
 * @param window surface okna pro vykreslení
 */
void Bonus::draw(SDL_Surface *window){
	if(!visible_) return;
	anim_.draw(window, x_, y_);
}


