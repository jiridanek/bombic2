
#include <iostream>
#include "SDL_lib.h"
#include "constants.h"
#include "game.h"
#include "game_mapobjects.h"
#include "game_bonus.h"

Bonus::Bonus(const Surface & sur, Uint16 x, Uint16 y):
	sur_(sur), x_(x), y_(y), visible_(false){}


void Bonus::move(){
}

void Bonus::draw(SDL_Surface *window){
	if(!visible_) return;
	draw_surface(x_*CELL_SIZE, y_*CELL_SIZE, sur_.GetSurface(), window);
}

OBJECT_TYPES Bonus::type(){
	return BONUS;
}


