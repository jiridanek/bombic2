
#include <iostream>
#include "SDL_lib.h"
#include "constants.h"
#include "game.h"
#include "game_mapobjects.h"
#include "game_floorobject.h"

Floorobject::Floorobject(const Surface & sur,	Uint16 x, Uint16 y):
	StaticMO(x, y), sur_(sur) {}

void Floorobject::draw(SDL_Surface* window){
	draw_surface(x_, y_, sur_.GetSurface(),window);
}

