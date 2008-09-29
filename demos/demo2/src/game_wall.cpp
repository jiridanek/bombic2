
#include <iostream>
#include "SDL_lib.h"
#include "constants.h"
#include "game.h"
#include "game_mapobjects.h"
#include "game_wall.h"

/** @details
 * @param anim Animace zdi.
 * @param x souřadnice v pixelech
 * @param y souřadnice v pixelech
 * @see StaticMO::StaticMO()
 */
Wall::Wall(const Animation & anim,	Uint16 x, Uint16 y):
		StaticMO(x, y), anim_(anim) {}

/** @details
 * @param window Surface, do kterého chceme zeď vykreslit.
 */
void Wall::draw(SDL_Surface* window){
	anim_.draw(window, x_, y_);
}

