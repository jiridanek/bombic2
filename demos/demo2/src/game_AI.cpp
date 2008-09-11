
#include <iostream>
#include <vector>
#include "SDL_lib.h"
#include "constants.h"
#include "game.h"
#include "game_mapobjects.h"
#include "game_AI.h"
#include "game_creature.h"

/** @details
 * Statická fce, pouze rozhodne, která třída AI se použije.
 * @param creature rodičovská nestvůra
 * @param intelligence výše umělé inteligence
 * @return Pointer na vytvořenou umělou inteligenci,
 *	nebo nula pokud výše umělé inteligence neexistuje.
 */
AI* AI::new_ai(Creature * creature, Uint16 intelligence){
	switch(intelligence){
		case 0: return new AI_0(creature);
		case 1: return new AI_1(creature);
		default: return 0;
	}
}

/** @details
 * @param creature pointer na nestvůru, které umělá inteligence patří
 */
AI::AI(Creature *creature): creature_(creature) {}

/** @details
 * Nastaví cílové pozice pro všech pět možností kroku.
 * Nehýbat se(0), krok vpřed(1), krok vpravo(2),
 * krok zpět(3), krok vlevo(4).
 */
void AI::updatePositions(){
	// prvni inicializace
	if(positions_.size()<5) {
		position_t pos;
		positions_.clear();
		positions_.push_back(pos);
		positions_.push_back(pos);
		positions_.push_back(pos);
		positions_.push_back(pos);
		positions_.push_back(pos);
	}

	// nastavim smery pro otoceni
	Uint16 i;
	switch(creature_->d_){
		case UP: i=1; break;
		case LEFT: i=2; break;
		case DOWN: i=3; break;
		case RIGHT: i=4; break;
		default: break;
	}
	// soucasna pozice
	positions_[0].d = creature_->d_;
	positions_[0].x = creature_->x_;
	positions_[0].y = creature_->y_;
	//souradnice pro otoceni a jedno policko vpred
	positions_[i].d=UP;
	positions_[i].x=creature_->x_;
	positions_[i].y=creature_->y_ - creature_->speed_;
	i=i%4+1;
	positions_[i].d=RIGHT;
	positions_[i].x=creature_->x_ + creature_->speed_;
	positions_[i].y=creature_->y_;
	i=i%4+1;
	positions_[i].d=DOWN;
	positions_[i].x=creature_->x_;
	positions_[i].y=creature_->y_ + creature_->speed_;
	i=i%4+1;
	positions_[i].d=LEFT;
	positions_[i].x=creature_->x_ - creature_->speed_;
	positions_[i].y=creature_->y_;
}

/** @details
 * Nastaví nestvůře souřadnice a otočení.
 * @param position nová pozice
 */
void AI::setPosition(const position_t & position){
	creature_->x_=position.x;
	creature_->y_=position.y;
	creature_->d_=position.d;
}

/************************ AI_0 **************************/

AI_0::AI_0(Creature *creature): AI(creature) {}

void AI_0::move() {
	updatePositions();
	// vpred
	if(checkfield_(positions_[1])){
		setPosition(positions_[1]);
		return;
	}
	// otoceni doprava
	if(rand()%3==0 && checkfield_(positions_[2])){
		setPosition(positions_[2]);
		return;
	}
	// otoceni doleva
	if(rand()%2==0 && checkfield_(positions_[4])){
		setPosition(positions_[4]);
		return;
	}
	// vzad
	if(checkfield_(positions_[3]))
		setPosition(positions_[3]);
}

/** @details
 * Zjistí podle zadané pozice jestli může na políčko vstoupit.
 * @param position zjištovaná pozice nestvůry
 * @return True pokud můžeme na políčko zadané pozice vstoupit, jinak false.
 */
bool AI_0::checkfield_(const position_t & position){
	Uint16 x=position.x/CELL_SIZE, y=position.y/CELL_SIZE;
	if(!Game::field_canGoOver(x,y))
		return false;

	switch(position.d){
		case UP: if(!Game::field_canGoOver(x,y-1)
				&& position.y%CELL_SIZE<CELL_SIZE/2)
					return false;
			break;
		case RIGHT: if(!Game::field_canGoOver(x+1,y)
				&& position.x%CELL_SIZE>CELL_SIZE/2)
					return false;
			break;
		case DOWN: if(!Game::field_canGoOver(x,y+1)
				&& position.y%CELL_SIZE>CELL_SIZE/2)
					return false;
			break;
		case LEFT: if(!Game::field_canGoOver(x-1,y)
				&& position.x%CELL_SIZE<CELL_SIZE/2)
					return false;
			break;
		default: break;
	}
	return true;
}

/************************ AI_1 **************************/

AI_1::AI_1(Creature *creature): AI(creature) {}

void AI_1::move() {
	updatePositions();
	// vpred
	if(rand()%100<=95 && checkfield_(positions_[1])){
		setPosition(positions_[1]);
		return;
	}
	// otoceni doprava
	if(rand()%5<=2 && checkfield_(positions_[2])){
		setPosition(positions_[2]);
		return;
	}
	// otoceni doleva
	if(rand()%3<=2 && checkfield_(positions_[4])){
		setPosition(positions_[4]);
		return;
	}
	// vzad
	if(checkfield_(positions_[3]))
		setPosition(positions_[3]);
}

/** @details
 * Zjistí podle zadané pozice jestli může na políčko vstoupit.
 * @param position zjištovaná pozice nestvůry
 * @return True pokud můžeme na políčko zadané pozice vstoupit, jinak false.
 */
bool AI_1::checkfield_(const position_t & position){
	Uint16 x=position.x/CELL_SIZE, y=position.y/CELL_SIZE;
	if(!Game::field_canGoOver(x,y))
		return false;

	switch(position.d){
		case UP: if(!Game::field_canGoOver(x,y-1)
				&& position.y%CELL_SIZE<CELL_SIZE/2)
					return false;
			break;
		case RIGHT: if(!Game::field_canGoOver(x+1,y)
				&& position.x%CELL_SIZE>CELL_SIZE/2)
					return false;
			break;
		case DOWN: if(!Game::field_canGoOver(x,y+1)
				&& position.y%CELL_SIZE>CELL_SIZE/2)
					return false;
			break;
		case LEFT: if(!Game::field_canGoOver(x-1,y)
				&& position.x%CELL_SIZE<CELL_SIZE/2)
					return false;
			break;
		default: break;
	}
	return true;
}

/*
DIRECTION AI::from_keyboard(Uint16 & x, Uint16 & y, Uint16 speed,
			SDLKey up, SDLKey right, SDLKey down, SDLKey left){
	// inicializace jako bych byl otoceny nahoru
	initialize(x, y, UP, speed);

	if(keystate_[up]){
		if(ai1_checkfield_(0)){
			y = positions_[0].y;
		}
		return positions_[0].d;
	}
	if(keystate_[right]){
		if(ai1_checkfield_(1)){
			x = positions_[1].x;
		}
		return positions_[1].d;
	}
	if(keystate_[down]){
		if(ai1_checkfield_(2)){
			y = positions_[2].y;
		}
		return positions_[2].d;
	}
	if(keystate_[left]){
		if(ai1_checkfield_(3)){
			x = positions_[3].x;
		}
		return positions_[3].d;
	}
	// zadna zmacknuta klavesa
	return BURNED;
}*/
