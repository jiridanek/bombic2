
#include <iostream>
#include "SDL_lib.h"
#include "constants.h"
// #include "game.h"
#include "game_mapobjects.h"
#include "game_wall.h"

/** @details
 * @param anim Animace zdi.
 * @param x souřadnice v pixelech
 * @param y souřadnice v pixelech
 * @see StaticMO::StaticMO()
 */
Wall::Wall(const Animation & anim, Uint16 x, Uint16 y):
		StaticMO(x, y), anim_(anim) {}

Wall::Wall(const Wall & wall, Uint16 x, Uint16 y, Uint16 height):
		// posunu nahoru o celou vysku, dolu o vysku zabranou v mape
		// => ve vysledku posunu o toplapping nahoru
		StaticMO(x, y - wall.anim_.height() + height*CELL_SIZE),
		anim_(wall.anim_) {}

/** @details
 * @param window Surface, do kterého chceme zeď vykreslit.
 */
void Wall::draw(SDL_Surface* window, const SDL_Rect & rect){
	anim_.draw(window, x_ +rect.x, y_ +rect.y);
}

void Wall::update(){
	anim_.update();
}
