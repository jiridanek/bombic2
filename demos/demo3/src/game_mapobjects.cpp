

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
	// zmena policka
	Game::get_instance()->change_position(old_x, old_y, x, y, this);
}

/**************** isTypeOf *************************/

/** @details
 * Inicializuje vnitřní strukturu.
 * @see clear()
 */
isTypeOf::isTypeOf(){
	clear();
}
/** @details
 * Inicializuje vnitřní strukturu, přidá typ.
 * @param type Typ který chceme přidat.
 * @see clear(), addType()
 */
isTypeOf::isTypeOf(OBJECT_TYPES type){
	clear();
	addType(type);
}

/** @details
 * Přidá do vnitřní struktury typ.
 * @param type Typ který chceme přidat.
 * @return Vrací referenci na sama sebe,
 * to podporuje řetězení fcí za sebe.
 */
isTypeOf & isTypeOf::addType(OBJECT_TYPES type){
	types_[type]=true;
	return *this;
}

/** @details
 * Smaže a inicializuje vnitřní strukturu.
 * Pro každý typ vloží do seznamu hledaných typů FALSE.
 * @return Vrací referenci na sama sebe,
 * to podporuje řetězení fcí za sebe.
 */
isTypeOf & isTypeOf::clear(){
	types_.clear();
	types_.insert(types_.end(), OBJECT_TYPES_COUNT, false);
	return *this;
}

/** @details
 * Kontroluje pointer na nenulovost,
 * zjistí zda typ zadaného objektu hledáme.
 * @param object pointer na objekt, jehož typ nás zajímá
 * @return Vrací TRUE pokud je pointer nenulový
 * a hledáme typ objektu.
 */
bool isTypeOf::operator()(MapObject * object){
	return object && types_[object->type()];
}
