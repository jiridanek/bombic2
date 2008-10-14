
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

/** @details
 * Jakýsi copycontructor, který navíc k okopírování objektu nastaví souřadnice.
 * @param background objekt k okopírování
 * @param x souřadnice levého rohu v pixelech
 * @param y souřadnice horního rohu v pixelech
 */
Background::Background(const Background & background, Uint16 x, Uint16 y):
			StaticMO(x, y), anim_clean_(background.anim_clean_),
			anim_burned_(background.anim_burned_) {}

/**
 * @param window surface okna pro vykreslení
 */
void Background::draw(SDL_Surface* window){
// TODO hlidat plameny
	anim_clean_.draw(window, x_, y_);
}

void Background::update(){
	anim_clean_.update();
}


/************* END of class Background *************/

