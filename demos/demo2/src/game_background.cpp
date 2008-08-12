
#include <iostream>
#include "SDL_lib.h"
#include "constants.h"
#include "game.h"
#include "game_mapobjects.h"
#include "game_background.h"

/************* class Background ******************/
Background::Background(const Surface & sur_clean,
		const Surface & sur_burned,	Uint16 x, Uint16 y):
			StaticMO(x, y),
			sur_clean_(sur_clean), sur_burned_(sur_burned) {}

void Background::draw(SDL_Surface* window){
	draw_surface(x_*CELL_SIZE,y_*CELL_SIZE,sur_clean_.GetSurface(),window);
}

OBJECT_TYPES Background::type(){
	return BACKGROUND;
}

/************* END of class Background *************/

