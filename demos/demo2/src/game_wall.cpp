
#include <iostream>
#include "SDL_lib.h"
#include "constants.h"
#include "game.h"
#include "game_mapobjects.h"
#include "game_wall.h"

/* * @details
 * @param sur Surface zdi.
 * @param x souřadnice v pixelech
 * @param y souřadnice v pixelech
 * @see StaticMO::StaticMO()
 */
Wall::Wall(const Surface & sur,	Uint16 x, Uint16 y):
		StaticMO(x, y), sur_(sur) {}

/* * @details
 * @param window Surface, do kterého chceme zeď vykreslit.
 * @see draw_surface()
 */
void Wall::draw(SDL_Surface* window){
	draw_surface(x_*CELL_SIZE,y_*CELL_SIZE,sur_.GetSurface(),window);
}

// OBJECT_TYPES Wall::type(){
// 	return WALL;
// }
