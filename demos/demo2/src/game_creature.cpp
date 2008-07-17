
#include <iostream>
#include "SDL_lib.h"
#include "constants.h"
#include "game.h"
#include "game_creature.h"

Creature::Creature(const Surface & sur_left, const Surface & sur_left_s,
			const Surface & sur_up, const Surface & sur_up_s,
			const Surface & sur_right, const Surface & sur_right_s,
			const Surface & sur_down, const Surface & sur_down_s,
			const Surface & sur_burned, Uint16 x, Uint16 y):
	sur_left_(sur_left),sur_left_s_(sur_left_s),
	sur_up_(sur_up), sur_up_s_(sur_up_s),
	sur_right_(sur_right), sur_right_s_(sur_right_s),
	sur_down_(sur_down), sur_down_s_(sur_down_s),
	sur_burned_(sur_burned), x_((x+0.5)*CELL_SIZE), y_((y+0.5)*CELL_SIZE){}


void Creature::move(){
}

void Creature::draw(SDL_Surface *window){
	int x=static_cast<int>(x_-sur_left_.width()/2);
	int y=static_cast<int>(y_-sur_left_.height()+CELL_SIZE/2);
	draw_surface(x, y, sur_left_s_.GetSurface(), window);
	draw_surface(x, y, sur_left_.GetSurface(), window);
}

OBJECT_TYPES Creature::type(){
	return CREATURE;
}


