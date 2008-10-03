
#include <iostream>
#include "SDL_lib.h"
#include "constants.h"
#include "game.h"
#include "game_mapobjects.h"
#include "game_box.h"

/**
 * @param anim animace pro normální stav
 * @param anim_burning animace hoření
 * @param toplapping počet políček mapy, které bedna nezabírá (odshora).
 * @param x souřadnice levého rohu v pixelech
 * @param y souřadnice horního rohu v pixelech
 */
Box::Box(const Animation & anim, const Animation & anim_burning,
				Uint16 toplapping, Uint16 x, Uint16 y):
	DynamicMO(x,y), anim_(anim), anim_burning_(anim_burning),
	toplapping_(toplapping),
	height_((anim.height()-toplapping)/CELL_SIZE),
	width_(anim.width()/CELL_SIZE){}

/** @details
 * Posune frame animace
 */
void Box::move(){
	anim_.update();
}

/**
 * @param window surface okna pro vykreslení
 */
void Box::draw(SDL_Surface *window){
	anim_.draw(window, x_, y_- toplapping_*CELL_SIZE);
}



