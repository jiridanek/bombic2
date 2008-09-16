

#include <iostream>
#include <algorithm>
#include "SDL_lib.h"
#include "constants.h"
#include "game.h"
#include "game_mapobjects.h"


/**************** getZ **************************/
bool isUnder(const MapObject * a, const MapObject * b){
	return a->getZ() < b->getZ();
}

Uint16 StaticMO::getZ() const {
	return 0;
}
Uint16 DynamicMO::getZ() const {
	return y_;
}
/********************** DynamicMO ****************/

/** @details
 * TODO
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
	return types_[object->type()];
}
