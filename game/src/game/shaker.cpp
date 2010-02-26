
#include "shaker.h"

/******************* class GameShaker ******************************/

GameShaker::GameShaker():
	delay_(0), diff_x_(0), diff_y_(0){}

/** @details
 * Nastaví nové souřadnice,
 * zkrátí dobu třesení.
 */
void GameShaker::update(){
	if(isShaking()){
		--delay_;
		if(delay_%GAME_SHAKER_VOLUME==0){
			diff_x_ = getNewDiff();
			diff_y_ = getNewDiff();
		}
	}
}

Sint16 GameShaker::getNewDiff(){
	return rand() % (GAME_SHAKER_MAX_DIFF*2 + 1) - GAME_SHAKER_MAX_DIFF;
}

/** @details
 * Nastaví třesení po zadanou dobu.
 * @param new_delay doba (počet kroků hry), po kterou chceme nadále třást
 */
void GameShaker::shake(Uint16 new_delay){
	if(delay_ < new_delay){
		delay_ = new_delay;
		diff_x_ = getNewDiff();
		diff_y_ = getNewDiff();
	}
}

Sint16 GameShaker::getDiffX(){
	return isShaking() ? diff_x_ : 0;
}

Sint16 GameShaker::getDiffY(){
	return isShaking() ? diff_y_ : 0;
}

/**
 * @return True, pokud se aktuálně hrou třese, jinak false;
 */
bool GameShaker::isShaking(){
	return delay_!=0;
}



/************** END OF class GameShaker ******************************/

