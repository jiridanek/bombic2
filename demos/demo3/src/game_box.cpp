
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
	burning_(false), toplapping_(toplapping),
	height_((anim.height()-toplapping)/CELL_SIZE),
	width_(anim.width()/CELL_SIZE){}

/** @details
 * Jakýsi copycontructor, který navíc k okopírování objektu nastaví souřadnice.
 * @param box objekt k okopírování
 * @param x souřadnice levého rohu v pixelech
 * @param y souřadnice horního rohu v pixelech
 */
Box::Box(const Box & box, Uint16 x, Uint16 y):
	DynamicMO(x,y), anim_(box.anim_), anim_burning_(box.anim_burning_),
	burning_(false), toplapping_(box.toplapping_),
	height_(box.height_), width_(box.width_){}

/** @details
 * Ohlídá plameny, pokud hoří, posune sám frame animace.
 * @return Vrací TRUE pokud je již vhodné objekt odstranit.
 */
bool Box::move(){
	if(Game::get_instance()->field_withObject(x_/CELL_SIZE, y_/CELL_SIZE, FLAME))
		burning_=true;
	if(burning_)
		return anim_burning_.update();
	else
		return false;
}

/** @details
 * Posune frame normální animace.
 */
void Box::update(){
	anim_.update();
}

/**
 * @param window surface okna pro vykreslení
 */
void Box::draw(SDL_Surface *window){
	if(burning_)
		anim_burning_.draw(window, x_, y_- toplapping_*CELL_SIZE);
	else
		anim_.draw(window, x_, y_- toplapping_*CELL_SIZE);
}



