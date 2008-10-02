

#include <iostream>
#include <algorithm>
#include "SDL_lib.h"
#include "constants.h"
#include "game.h"
#include "game_mapobjects.h"


/**************** getZ **************************/

/**
 * @param a objekt mapy k porovnání
 * @param b objekt mapy k porovnání
 * @return TRUE pokud je objekt a pod objektem b,
 * objekt b ho tedy překrývá.
 */
bool isUnder(const MapObject * a, const MapObject * b){
	return a->getZ() < b->getZ();
}

/** @details
 * Statický objekt je vždy nejníže.
 * @return Vrací vždy nulu.
 */
Uint16 StaticMO::getZ() const {
	return 0;
}
/**
 * @return Vrací y-ovou souřadnici pro vykreslení na obrazovku,
 * objekt kter je k nám blíže má být vidět.
 */
Uint16 DynamicMO::getZ() const {
	return y_;
}
/********************** DynamicMO ****************/

/** @details
 * Pokud objekt opustil staré políčko, vymaže se objekt z mapy
 * a vloží se na nové políčko.
 * @param old_x stará souřadnice
 * @param old_y stará souřadnice
 */
void DynamicMO::setFieldInMap(Uint16 old_x, Uint16 old_y){
	Uint16 x=x_/CELL_SIZE, y=y_/CELL_SIZE;
	old_x/=CELL_SIZE; old_y/=CELL_SIZE;
	if(old_x==x && old_y==y) return;
	// vyhozeni ze stareho policka
	Game::map_array_t::value_type::value_type::iterator it;
	it= find(Game::map_array_[old_x][old_y].begin(),
			Game::map_array_[old_x][old_y].end(), this );
	if(it!=Game::map_array_[old_x][old_y].end())
		Game::map_array_[old_x][old_y].erase(it);
	// vlozeni na nove policko
	Game::map_array_[x][y].push_back(this);
}

/**************** isTypeOf *************************/

isTypeOf::isTypeOf(){
	clear();
}
isTypeOf::isTypeOf(OBJECT_TYPES type){
	clear();
	addType(type);
}

isTypeOf & isTypeOf::addType(OBJECT_TYPES type){
	types_[type]=true;
	return *this;
}

void isTypeOf::clear(){
	types_.clear();
	types_.insert(types_.end(), OBJECT_TYPES_COUNT, false);
}

bool isTypeOf::operator()(MapObject * object){
	return object && types_[object->type()];
}
