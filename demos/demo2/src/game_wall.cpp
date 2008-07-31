
#include <iostream>
#include "SDL_lib.h"
#include "constants.h"
#include "game.h"
#include "game_mapobjects.h"
#include "game_wall.h"

Wall::Wall(const Surface & sur,	Uint16 x, Uint16 y):
		sur_(sur), x_(x), y_(y){}

void Wall::draw(SDL_Surface* window){
	draw_surface(x_*CELL_SIZE,y_*CELL_SIZE,sur_.GetSurface(),window);
}

OBJECT_TYPES Wall::type(){
	return WALL;
}