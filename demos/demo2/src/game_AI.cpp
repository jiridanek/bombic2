
#include <iostream>
#include <vector>
#include "SDL_lib.h"
#include "constants.h"
#include "game.h"
#include "game_mapobjects.h"
#include "game_AI.h"

Uint16 AI::x_=0;
Uint16 AI::y_=0;

std::vector< AI::Direction_XY > AI::move_;
/** @details
 */
DIRECTION AI::step(int x, int y, DIRECTION d, Uint8 ai){
	initialize();
	// nynejsi souradnice
	x_=x/CELL_SIZE;
	y_=y/CELL_SIZE;
	// nastavim smery pro otoceni
	Uint8 i=0;
	switch(d){
		case UP: i=0; break;
		case LEFT: i=1; break;
		case DOWN: i=2; break;
		case RIGHT: i=3; break;
	}
	//souradnice pro otoceni a jedno policko vpred
	move_[  i%4].d=UP;  	move_[i%4].x=x_;	move_[i%4].y=y_-1;
	move_[++i%4].d=RIGHT;	move_[i%4].x=x_+1;	move_[i%4].y=y_;
	move_[++i%4].d=DOWN;	move_[i%4].x=x_;	move_[i%4].y=y_+1;
	move_[++i%4].d=LEFT;	move_[i%4].x=x_-1;	move_[i%4].y=y_;
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
	if(Game::field_canGoOver(move_[0].x,move_[0].y))
		return move_[0].d;
	return ai1();
}

/**
 */
DIRECTION AI::ai1(){
	double random = SDL_Rand();
	// vpred
	if(random>0.5){
		if(Game::field_canGoOver(move_[0].x,move_[0].y))
			return move_[0].d;
		else random=SDL_Rand()/2;
	}
	// otoceni doprava
	if(random>0.3){
		if(Game::field_canGoOver(move_[1].x,move_[1].y))
			return move_[1].d;
		else random=SDL_Rand()/3;
	}
	// otoceni doleva
	if(random>0.1){
		if(Game::field_canGoOver(move_[3].x,move_[3].y))
			return move_[3].d;
	}
	// vzad
	if(Game::field_canGoOver(move_[2].x,move_[2].y))
		return move_[2].d;
	return BURNED;
}

/**
 */
void AI::initialize(){
	if(move_.size()>=4)
		return;
	Direction_XY dirxy;
	move_.clear();
	move_.push_back(dirxy);
	move_.push_back(dirxy);
	move_.push_back(dirxy);
	move_.push_back(dirxy);
}
