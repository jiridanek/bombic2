
#include <iostream>
#include "SDL_lib.h"
#include "constants.h"
#include "game.h"
#include "game_mapobjects.h"
#include "game_presumption.h"

/**
 * @param anim animace
 * @param x souřadnice levého rohu políčka v pixelech
 * @param y souřadnice horního rohu v pixelech
 */
Presumption::Presumption(const Animation & anim, Uint16 x, Uint16 y):
	DynamicMO(x, y), anim_(anim) {}

/** @details
 * Zvýší počet přístupů, kontroluje s dobou periody.
 * @return Vrací TRUE pokud se má objekt zahodit.
 */
bool Presumption::move(){
	return false;
}

/**
 * @param window surface okna pro vykreslení
 */
void Presumption::draw(SDL_Surface *window){
	anim_.draw(window, x_, y_);
}

void Presumption::update(){
	anim_.update();
}
