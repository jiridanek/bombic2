
#include <iostream>
#include <vector>
#include "SDL_lib.h"
#include "constants.h"
#include "game.h"
#include "game_mapobjects.h"
#include "game_AI.h"

Uint16 AI::x_=0;
Uint16 AI::y_=0;

Uint8 *AI::keystate_ = SDL_GetKeyState(0);

std::vector< AI::Direction_XY > AI::move_;
/** @details
 */
DIRECTION AI::step(int x, int y, DIRECTION d, Uint8 speed, Uint8 ai){
	initialize(x, y, d, speed);
	// rozhodneme ktera umela inteligence se pouzije
	switch(ai){
		case 0: return ai0();
		case 1: return ai1();
		// defaultne se nehybe
		default: return BURNED;
	}
}

/**
 */
DIRECTION AI::ai0(){
	if(ai1_checkfield(0))
		return move_[0].d;
	return ai1();
}

bool AI::ai1_checkfield(Uint8 i){
	Uint16 x=move_[i].x/CELL_SIZE, y=move_[i].y/CELL_SIZE;
	if(!Game::field_canGoOver(x,y))
		return false;

	switch(move_[i].d){
		case UP: if(!Game::field_canGoOver(x,y-1)
				&& move_[i].y%CELL_SIZE<CELL_SIZE/2)
					return false;
			break;
		case RIGHT: if(!Game::field_canGoOver(x+1,y)
				&& move_[i].x%CELL_SIZE>CELL_SIZE/2)
					return false;
			break;
		case DOWN: if(!Game::field_canGoOver(x,y+1)
				&& move_[i].y%CELL_SIZE>CELL_SIZE/2)
					return false;
			break;
		case LEFT: if(!Game::field_canGoOver(x-1,y)
				&& move_[i].x%CELL_SIZE<CELL_SIZE/2)
					return false;
			break;
		default: break;
	}
	return true;
}
/**
 */
DIRECTION AI::ai1(){
// pravdepodobnost kterym smerem se dat
#ifndef AI1_straight
#define AI1_straight 95.0/100
#define AI1_right 2.0/100
#define AI1_left 2.0/100
#endif
	double random = SDL_Rand();

	// vpred
	if(random<AI1_straight){
		if(ai1_checkfield(0))
			return move_[0].d;
		random=SDL_Rand()*(1-AI1_straight);
	}
	else random-=AI1_straight;
	// otoceni doprava
	if(random<AI1_right){
		if(ai1_checkfield(1))
			return move_[1].d;
		random=SDL_Rand()*(1-AI1_straight-AI1_right);
	}
	else random-=AI1_right;
	// otoceni doleva
	if(random<AI1_left){
		if(ai1_checkfield(3))
			return move_[3].d;
	}
	// vzad
	if(ai1_checkfield(2))
		return move_[2].d;
	return BURNED;
}

DIRECTION AI::from_keyboard(Uint16 & x, Uint16 & y, Uint8 speed,
			SDLKey up, SDLKey right, SDLKey down, SDLKey left){
	// obnoveni stavu klavesnice
	SDL_PumpEvents();
	// inicializace jako bych byl otoceny nahoru
	initialize(x, y, UP, speed);

	if(keystate_[up]){
		if(ai1_checkfield(0)){
			y = move_[0].y;
		}
		return move_[0].d;
	}
	if(keystate_[right]){
		if(ai1_checkfield(1)){
			x = move_[1].x;
		}
		return move_[1].d;
	}
	if(keystate_[down]){
		if(ai1_checkfield(2)){
			y = move_[2].y;
		}
		return move_[2].d;
	}
	if(keystate_[left]){
		if(ai1_checkfield(3)){
			x = move_[3].x;
		}
		return move_[3].d;
	}
	// zadna zmacknuta klavesa
	return BURNED;
}


/**
 */
void AI::initialize(int x, int y, DIRECTION direction, Uint8 speed){
	// prvni inicializace
	if(move_.size()<4) {
		Direction_XY dirxy;
		move_.clear();
		move_.push_back(dirxy);
		move_.push_back(dirxy);
		move_.push_back(dirxy);
		move_.push_back(dirxy);
	}

	x_=x;
	y_=y;

	// nynejsi souradnice
	// nastavim smery pro otoceni
	Uint8 i=0;
	switch(direction){
		case UP: i=0; break;
		case LEFT: i=1; break;
		case DOWN: i=2; break;
		case RIGHT: i=3; break;
		default: break;
	}
	//souradnice pro otoceni a jedno policko vpred
	move_[  i%4].d=UP;  	move_[i%4].x=x;      	move_[i%4].y=y-speed;
	move_[++i%4].d=RIGHT;	move_[i%4].x=x+speed;	move_[i%4].y=y;
	move_[++i%4].d=DOWN;	move_[i%4].x=x;      	move_[i%4].y=y+speed;
	move_[++i%4].d=LEFT;	move_[i%4].x=x-speed;	move_[i%4].y=y;
}
