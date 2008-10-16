
#include <iostream>
#include "SDL_lib.h"
#include "constants.h"
#include "game.h"
#include "game_mapobjects.h"
#include "game_flame.h"

/**
 * @param anim animace
 * @param x souřadnice levého rohu políčka v pixelech
 * @param y souřadnice horního rohu v pixelech
 * @param period délka života plamene v sekundách
 */
Flame::Flame(const Animation & anim, Uint16 x, Uint16 y, Uint16 period):
	DynamicMO(x, y),
	anim_(anim), access_counter_(0), period_(period*1000) {}

/** @details
 * Zvýší počet přístupů, kontroluje s dobou periody.
 * @return Vrací TRUE pokud se má objekt zahodit.
 */
bool Flame::move(){
	++access_counter_;
	return access_counter_*MOVE_PERIOD > period_;
}

/**
 * @param window surface okna pro vykreslení
 */
void Flame::draw(SDL_Surface *window){
	anim_.draw(window, x_, y_);
}

void Flame::update(){
	anim_.update();
}
