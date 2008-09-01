
#include <iostream>
#include "SDL_lib.h"
#include "constants.h"
#include "game.h"
#include "game_mapobjects.h"
#include "game_box.h"

Box::Box(const Surface & sur_img, const Surface & sur_burning,
				Uint16 toplapping, Uint16 x, Uint16 y):
	DynamicMO(x,y), sur_img_(sur_img), sur_burning_(sur_burning),
	toplapping_(toplapping),
	height_((sur_img.height()-toplapping)/CELL_SIZE),
	width_(sur_img.width()/CELL_SIZE){}

void Box::move(){
}

void Box::draw(SDL_Surface *window){
	draw_surface(x_*CELL_SIZE, (y_-toplapping_)*CELL_SIZE,
			sur_img_.GetSurface(), window);
}

// OBJECT_TYPES Box::type(){
// 	return BOX;
// }


