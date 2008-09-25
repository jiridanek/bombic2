
#include <iostream>
#include "SDL_lib.h"
#include "constants.h"
#include "game.h"
#include "game_mapobjects.h"
#include "game_floorobject.h"

Floorobject::Floorobject(const Animation & anim, Uint16 x, Uint16 y):
	StaticMO(x, y), anim_(anim) {}

void Floorobject::draw(SDL_Surface* window){
	anim_.draw(window, x_, y_);
}

