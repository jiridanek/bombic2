
#include <iostream>
#include "SDL_lib.h"
#include "constants.h"
#include "game.h"
#include "game_mapobjects.h"
#include "game_background.h"

/************* class Background ******************/

/**
 * @param anim_clean animace před výbuchem
 * @param anim_burned animace po výbuchu
 * @param x souřadnice levého rohu v pixelech
 * @param y souřadnice pravého rohu v pixelech
 */
Background::Background(const Animation & anim_clean,
		const Animation & anim_burned,	Uint16 x, Uint16 y):
			StaticMO(x, y),
			anim_clean_(anim_clean), anim_burned_(anim_burned) {}

// TODO update
/**
 * @param window surface okna pro vykreslení
 */
void Background::draw(SDL_Surface* window){
	anim_clean_.draw(window, x_, y_);
}

/************* END of class Background *************/

