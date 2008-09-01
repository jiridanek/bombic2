
#include <iostream>
#include "SDL_lib.h"
#include "constants.h"
#include "game.h"
#include "game_mapobjects.h"
#include "game_bonus.h"

Bonus::Bonus(const Surface & sur, Uint16 x, Uint16 y):
	DynamicMO(x,y), sur_(sur), visible_(false) {}


void Bonus::move(){
}

void Bonus::draw(SDL_Surface *window){
	if(!visible_) return;
	draw_surface(x_, y_, sur_.GetSurface(), window);
}


