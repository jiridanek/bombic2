
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
	draw_surface(x_, y_, sur_clean_.GetSurface(), window);
}


/************* END of class Background *************/

