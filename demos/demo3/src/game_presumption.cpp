
#include <iostream>
#include "SDL_lib.h"
#include "constants.h"
// #include "game.h"
#include "game_mapobjects.h"
#include "game_presumption.h"

/**
 * @param anim animace
 * @param x souřadnice levého rohu políčka v pixelech
 * @param y souřadnice horního rohu v pixelech
 */
Presumption::Presumption(Animation * anim, Uint16 x, Uint16 y,
				Uint16 to_flame):
	DynamicMO(x, y), anim_(anim), periods_to_flame_(to_flame) {}

/** @details
 * Sníží počet period do výbuchu.
 * @return Vrací FALSE.
 */
bool Presumption::move(){
	if(periods_to_flame_)
		--periods_to_flame_;
	return false;
}

/**
 * @param window surface okna pro vykreslení
 */
void Presumption::draw(SDL_Surface *window, const SDL_Rect & rect){
	anim_->draw(window, x_ +rect.x, y_ +rect.y);
}

/** @details
 * Náhodně zvolí frame animace.
 */
void Presumption::update(){
	anim_->random();
}
