
#include <iostream>
#include "SDL_lib.h"
#include "constants.h"
#include "game.h"
#include "game_mapobjects.h"
#include "game_background.h"

/************* class Background ******************/

Background::Background(const Animation & anim_clean,
		const Animation & anim_burned,	Uint16 x, Uint16 y):
			StaticMO(x, y),
			anim_clean_(anim_clean), anim_burned_(anim_burned) {}

// TODO update
void Background::draw(SDL_Surface* window){
	anim_clean_.draw(window, x_, y_);
}

/************* END of class Background *************/

