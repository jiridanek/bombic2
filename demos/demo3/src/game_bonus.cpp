
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

/** @details
 * Jakýsi copycontructor, který navíc k okopírování objektu nastaví souřadnice.
 * @param bonus objekt k okopírování
 * @param x souřadnice levého rohu v pixelech
 * @param y souřadnice horního rohu v pixelech
 */
Bonus::Bonus(const Bonus & bonus, Uint16 x, Uint16 y):
	DynamicMO(x,y), anim_(bonus.anim_), visible_(false) {}

/** @details
 * Posune frame animace.
 */
bool Bonus::move(){
	if(!visible_) return false;
	// TODO hlídat plameny a sežrání
	return false;
}

/**
 * @param window surface okna pro vykreslení
 */
void Bonus::draw(SDL_Surface *window){
	if(visible_)
		anim_.draw(window, x_, y_);
}

void Bonus::update(){
	if(visible_)
		anim_.update();
}
