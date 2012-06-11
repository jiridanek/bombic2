
#include "shaker.h"

/******************* class GameShaker ******************************/

GameShaker::GameShaker():
	delay_(0), started_(false), diff_x_(0), diff_y_(0){}

/** @details
 * Nastaví nové souřadnice,
 * zkrátí dobu třesení.
 */
void GameShaker::update(){
	if(isShaking()){
		if(!delay_) {
			delay_ = GAME_SHAKER_VOLUME;
		}
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

/** @details
 * Spustí dlouhodobé třesení. Třesení se samo nezastaví, trvá, dokud není
 * explicitně zavolána metoda GameShaker::stop().
 */
void GameShaker::start(){
	started_ = true;
}

/** @details
 * Zastaví dlouhodobé třesení.
 * Třesení se zastaví, pokud bylo předtím spuštěno metodou GameShaker::start().
 * Pokud mezitím bylo nastaveno krátkodobé třesení metodou GameShaker::shake(),
 * a doba nastavená tímto voláním nevypršela, krátkodobé třesení pokračuje.
 */
void GameShaker::stop(){
	started_ = false;
	diff_x_ = diff_y_ = 0;
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
	return delay_!=0 || started_;
}



/************** END OF class GameShaker ******************************/

