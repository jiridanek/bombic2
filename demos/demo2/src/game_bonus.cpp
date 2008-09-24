
#include <iostream>
#include "SDL_lib.h"
#include "constants.h"
#include "game.h"
#include "game_mapobjects.h"
#include "game_bonus.h"

Bonus::Bonus(const Animation & anim, Uint16 x, Uint16 y):
	DynamicMO(x,y), anim_(anim), visible_(false) {}


void Bonus::move(){
	if(!visible_) return;
	anim_.update();
}

void Bonus::draw(SDL_Surface *window){
	if(!visible_) return;
	anim_.draw(window, x_, y_);
}


