
#include <iostream> // debug
#include <vector>
#include <algorithm>
#include <string>
#include <helpers/sdl.h>
#include <helpers/stl.h>
#include <constants.h>
#include <config.h>
#include "game.h"
#include "mapobjects.h"
#include "AI.h"
#include "creature.h"
#include "player.h"
#include "bomb.h"

using namespace std;

/** @details
 * Statická fce, pouze rozhodne, který potomek AI se použije.
 * @param creature rodičovská nestvůra
 * @param intelligence výše umělé inteligence
 * @return Pointer na vytvořenou umělou inteligenci,
 *	nebo nula pokud výše umělé inteligence neexistuje.
 */
AI* AI::new_ai(Creature * creature, Sint16 intelligence){
	switch(intelligence){
		case -1: return new AI_fromKeyboard(creature);
		case  0: return new AI_0(creature);
		case  1: return new AI_1(creature);
		case  2: return new AI_2(creature);
		case  3: return new AI_3(creature);
		case  4: return new AI_4(creature);
		case  5: return new AI_5(creature);
		case  6: return new AI_6(creature);
		case  7: return new AI_7(creature);
		case  8: return new AI_8(creature);
		case  9: return new AI_9(creature);
		case  10: return new AI_10(creature);
		default: return 0;
	}
}

/** @details
 * Statická fce, pouze vybere potomka AI stejného jako je v parametru.
 * @param creature rodičovská nestvůra
 * @param ai pointer na již vytvořenou inteligenci,
 * ten se použije pouze na zjištění typu inteligence
 * @return Pointer na vytvořenou umělou inteligenci,
 *	nebo nula pokud umělá inteligence neexistuje.
 */
AI* AI::new_ai(Creature * creature, const AI * ai){
	if(!ai) return 0;
	return new_ai(creature, ai->type());
}

/** @details
 * @param creature pointer na nestvůru, které umělá inteligence patří
 */
AI::AI(Creature *creature): creature_(creature) {

}

/** @details
 * Vloží tolik pozic, aby bylo dost pro další použití.
 * @param position_min_count minimální počet pozic pro další použití
 */
void AI::initPositions(){
	if(positions_.size() < POS_LAST) {
		position_t default_pos;
		positions_.clear();
		positions_.insert(
			positions_.end(), POS_LAST, default_pos);
	}
}

/** @details
 * Nastaví cílové pozice pro všech pět možností kroku.
 * Nehýbat se(0), krok vpřed(1), krok vpravo(2),
 * krok zpět(3), krok vlevo(4).
 */
void AI::updatePositions(){
	// prvni inicializace
	initPositions();
	// soucasna pozice
	positions_[POS_STAY].d = creature_->d_;
	positions_[POS_STAY].x = creature_->x_;
	positions_[POS_STAY].y = creature_->y_;
	// nastavim smery pro otoceni
	Uint16 i;
	switch(creature_->d_){
		// do i nastavim kam se musim otocit abych koukal nahoru
		case UP: i = POS_STRAIGHT; break;
		case LEFT: i = POS_RIGHT; break;
		case DOWN: i = POS_BACK; break;
		case RIGHT: i = POS_LEFT; break;
		default:
			throw string("Invalid direction in AI::updatePositions()");
	}
	//souradnice pro otoceni a jeden krok vpred
	positions_[i].d = UP;
	positions_[i].x = creature_->x_;
	positions_[i].y = creature_->y_ - creature_->getSpeed();
	i = i%4 + 1;
	positions_[i].d = RIGHT;
	positions_[i].x = creature_->x_ + creature_->getSpeed();
	positions_[i].y = creature_->y_;
	i = i%4 + 1;
	positions_[i].d = DOWN;
	positions_[i].x = creature_->x_;
	positions_[i].y = creature_->y_ + creature_->getSpeed();
	i = i%4 + 1;
	positions_[i].d = LEFT;
	positions_[i].x = creature_->x_ - creature_->getSpeed();
	positions_[i].y = creature_->y_;
}

/** @details
 * Nastaví nestvůře souřadnice a otočení.
 * Stará se také o centrování na políčku.
 * @param position nová pozice
 */
void AI::setPosition(position_t & position){
	// prvne vycentrovat
	switch(position.d){
		case UP:
			centerCoordinates(position, false, +1);
			break;
		case RIGHT:
			centerCoordinates(position, true, -1);
			break;
		case DOWN:
			centerCoordinates(position, false, -1);
			break;
		case LEFT:
			centerCoordinates(position, true, +1);
			break;
		case BURNED:
			break;
		default:
			throw string("Invalid direction in AI::setPositions()");
	}
	// nastavit pozici prisery
	creature_->x_ = position.x;
	creature_->y_ = position.y;
	creature_->d_ = position.d;
}

/** @details
 * Podle zadaných parametrů se rozhlédne po okolních políčkách
 * a je-li třeba, přiblíží souřadnice pozice středu políčka.
 * @param position pozice, jejíž souřadnice chci centrovat
 * @param isMainX je-li hlavním směrem osa X
 * @param sign +1 nebo -1 podle orientace pohybu po hlavní ose
 * +1 pro vzhůru a vlevo, -1 pro dolů a vpravo
 */
void AI::centerCoordinates(position_t & position, bool isMainX, Sint8 sign){
	bool isMainY = !isMainX;
	// hlavni souradnice
	Uint16 & main_coor = isMainX ? position.x : position.y;
	// vedlejsi souradnice
	Uint16 & extra_coor = isMainY ? position.x : position.y;
	// souradnice v ramci policka v hlavnim smeru
	Uint16 cell_coor = main_coor % CELL_SIZE + sign;
	// je za pulkou ve svem smeru
	bool isTooFar = sign > 0 ?
		cell_coor < CELL_SIZE/2 : cell_coor > CELL_SIZE/2;
	// zmena hlavni souradnice
	Sint8 main_diff = isTooFar ? sign : 0;
	// zmena vedlejsi souradnice
	Sint8 extra_diff;
	if(extra_coor % CELL_SIZE +1 < CELL_SIZE/2){
		extra_diff = +1;
	} else if(extra_coor % CELL_SIZE -1 > CELL_SIZE/2){
		extra_diff = -1;
	} else {
		extra_diff = 0;
	}
	// vykonat centrovani
	if(extra_diff != 0){
		Uint16 x = position.x/CELL_SIZE -
			( isMainX ? main_diff : extra_diff );
		Uint16 y = position.y/CELL_SIZE -
			( isMainY ? main_diff : extra_diff );
		if(!GAME->field_canGoOver(x, y)){
			// vycentrovat
			centerCoordinate(extra_coor, extra_diff);
			if(main_diff != 0){
				// zpomalit
				centerCoordinate(main_coor, main_diff);
			}
		}
	}
}

/** @details
 * Podle zadaného znaménka přidá k souřadnici nebo ubere od souřadnice
 * polovinu rychlosti nestvůry. Maximálně však do poloviny políčka.
 * @param coordinate centrovaná souřadnice
 * @param sign +1 nebo -1, přidá nebo ubere
 */
void AI::centerCoordinate(Uint16 & coordinate, Sint8 sign){
	// vzdalenost od stredu policka
	Uint8 diff_center = sign * (CELL_SIZE/2 - coordinate % CELL_SIZE);
	// polovina rychlosti
	Uint8 diff_speed = (creature_->getSpeed() + 1) / 2;
	coordinate += sign * min(diff_center, diff_speed);
}

/** @details
 * Zjistí podle zadané pozice jestli může na políčko vstoupit..
 * @param position zjištovaná pozice nestvůry
 * @param isBlocking predikát na určení blokujícího typu
 * @return TRUE pokud se na políčku nevyskytuje žádný z objektů zadaných typů.
 */
bool AI::checkField(const position_t & position,
					const isTypeOf & isBlocking){
	Uint16 x = position.x / CELL_SIZE,
		y = position.y / CELL_SIZE;
	// primo kam chci jit je blokujici objekt (krome bomby)
	MapObject * blocking =
		GAME->field_getObject(x, y, isBlocking);
	if(blocking && blocking->type()!=BOMB_STAYING)
		return false;

	// jsem-li za pulkou
	bool isTooFar = false;
	Sint16 diff_x = 0, diff_y = 0;
	switch(position.d){
		case UP:
			if(position.y % CELL_SIZE < CELL_SIZE/2){
				isTooFar = true;
				diff_y = -1;
			}
			break;
		case RIGHT:
			if(position.x % CELL_SIZE > CELL_SIZE/2){
				isTooFar = true;
				diff_x = +1;
			}
			break;
		case DOWN:
			if(position.y % CELL_SIZE > CELL_SIZE/2){
				isTooFar = true;
				diff_y = +1;
			}
			break;
		case LEFT:
			if(position.x % CELL_SIZE < CELL_SIZE/2){
				isTooFar = true;
				diff_x = -1;
			}
			break;
		case BURNED:
			break;
		default:
			throw string("Invalid direction in AI::checkField()");
	}

	bool isFree;
	if(isTooFar){
		// zkontroluji policko na ktere smeruji
		isFree = !GAME->field_withObject(
			x + diff_x, y + diff_y, isBlocking);
	} else {
		isFree = true;
	}
	return isFree;
}

/************************ AI_0 **************************/

AI_0::AI_0(Creature *creature): AI(creature){

}

/** @details
 * Nedělá vůbec nic.
 */
void AI_0::move() {

}

/************************ AI_1 **************************/

AI_1::AI_1(Creature *creature):
	AI(creature), isBlocking_(isTypeOf::isWallBoxBomb) {

}

AI_1::AI_1(Creature *creature, isTypeOf & isBlocking):
	AI(creature), isBlocking_(isBlocking) {

}

/** @details
 * Obnoví možné pozice, vybere z nich pomocí findPosIndex(),
 * nastaví ji.
 */
void AI_1::move() {
	updatePositions();
	Uint16 posIndex = findPosIndex(isBlocking_);
	setPosition(positions_[posIndex]);
}

/** @details
 * Najde index pomocí findPosIndexToWalkStraight_()
 * @param isBlocking predikát určující blokující objekty
 * @return Index pozice, na kterou máme jít.
 */
AI::PositionIndex AI_1::findPosIndex(isTypeOf & isBlocking){
	return findPosIndexToWalkStraight_(isBlocking);
}

/** @details
 * Pokud může, vybere pozici pro pohyb rovně,
 * pokud nemůže, vybere náhodně spravedlivě z možních pozic.
 * @param isBlocking predikát určující blokující objekty
 * @return Index pozice, na kterou máme jít.
 */
AI::PositionIndex AI_1::findPosIndexToWalkStraight_(isTypeOf & isBlocking){
	// vpred
	if(checkField(positions_[POS_STRAIGHT], isBlocking)){
		return POS_STRAIGHT;
	}
	// zjistim pocet neblokovanych moznosti
	Uint16 nonBlockedPositions = 0;
	// pres zbyvajici pozice
	for(Uint16 i = POS_STRAIGHT +1 ; i < POS_LAST ; ++i){
		// ulozim si info o blokovani
		if(checkField(positions_[i], isBlocking)){
			positions_[i].isBlocked = false;
			// zapocitam jako neblokovanou
			++nonBlockedPositions;
		} else {
			positions_[i].isBlocked = true;
		}
	}

	for(Uint16 i = POS_STRAIGHT +1 ; i < POS_LAST ; ++i){
		// neni neblokovana, nezajima me
		if(positions_[i].isBlocked){
			continue;
		}
		// vyberu nahodne spravedlive mezi neblokovanymi
		if(rand() % nonBlockedPositions == 0){
			return static_cast<PositionIndex>(i);
		}
		// snizim pocet neblokovanych pozic
		--nonBlockedPositions;
	}

	// zustat na miste
	return POS_STAY;
}

/************************ AI_2 **************************/

AI_2::AI_2(Creature *creature):
	AI_1(creature, isTypeOf::isWallBoxBombFlame) {

}

/************************ AI_3 **************************/

AI_3::AI_3(Creature *creature):
	AI_1(creature, isTypeOf::isWallBoxBombFlame),
	isBad_(isTypeOf::isWallBoxBombFlamePresumption) {

}

/** @details
 * Obnoví možné pozice, vybere z nich pomocí findPosIndex(),
 * přičemž se vyhýbá špatným objektům.
 * Kdyby měl zůstat stát na špatném objektu,
 * použije ještě jednou findPosIndex(),
 * tentokrát s ohledem pouze na blokující objekty.

 */
void AI_3::move() {
	updatePositions();
	PositionIndex posIndex = findPosIndex(isBad_);
	// kdyby mel zustat stat
	if(posIndex == POS_STAY){
		bool isInPresumption =
			positions_[posIndex].field_withObject(
				isTypeOf::isPresumption);
		if(isInPresumption){
			posIndex = findPosIndex(isBlocking_);
		}
	}
	setPosition(positions_[posIndex]);
}

/************************ AI_4 **************************/

AI_4::AI_4(Creature *creature):
		AI_1(creature, isTypeOf::isWallBoxBomb),
		distanceWalkedStraight_(0) {

	minDistanceWalkedStraight_ =
		AI_4_MIN_DISTANCE_WALKED_STRAIGHT;
}

AI_4::AI_4(Creature *creature, isTypeOf & isBlocking,
				Uint16 minDistance):
		AI_1(creature, isBlocking),
		distanceWalkedStraight_(0) {

	minDistanceWalkedStraight_ = minDistance;
}

/** @details
 * Obnoví možné pozice, vybere z nich pomocí findPosIndex(),
 * obnoví vzdálenost ušlou přímo,
 * nastaví vybranou pozici.
 */
void AI_4::move() {
	updatePositions();
	PositionIndex posIndex = findPosIndex(isBlocking_);
	updateDistance_(positions_[posIndex]);
	setPosition(positions_[posIndex]);
}

/** @details
 * Najde index pomocí findPosIndexToWalkStraight_(),
 * pokud ušel dostatečnou vzdálenost rovně,
 * použije findPosIndexToWalkRandomly_().
 * @param isBlocking predikát určující blokující objekty
 * @return Index pozice, na kterou máme jít.
 */
AI::PositionIndex AI_4::findPosIndex(isTypeOf & isBlocking){
	if(distanceWalkedStraight_ < minDistanceWalkedStraight_) {
		return findPosIndexToWalkStraight_(isBlocking);
	} else {
		return findPosIndexToWalkRandomly_(isBlocking);
	}
}


/** @details
 * Vybere náhodně spravedlivě z možních pozic.
 * @param isBlocking predikát určující blokující objekty
 * @return Index pozice, na kterou máme jít.
 */
AI::PositionIndex AI_4::findPosIndexToWalkRandomly_(isTypeOf & isBlocking){
	// zjistim pocet neblokovanych moznosti
	Uint16 nonBlockedPositions = 0;
	// pres vsechny pozice
	for(Uint16 i = POS_STRAIGHT ; i < POS_LAST ; ++i){
		// ulozim si info o blokovani
		if(checkField(positions_[i], isBlocking)){
			positions_[i].isBlocked = false;
			// zapocitam jako neblokovanou
			++nonBlockedPositions;
		} else {
			positions_[i].isBlocked = true;
		}
	}

	for(Uint16 i = POS_STRAIGHT ; i < POS_LAST ; ++i){
		// neni neblokovana, nezajima me
		if(positions_[i].isBlocked){
			continue;
		}
		// vyberu nahodne spravedlive mezi neblokovanymi
		if(rand() % nonBlockedPositions == 0){
			return static_cast<PositionIndex>(i);
		}
		// snizim pocet neblokovanych pozic
		--nonBlockedPositions;
	}

	// zustat na miste
	return POS_STAY;
}

/** @details
 * Přidá ušlou vzdálenost k vzdálenosti ušlé rovně,
 * pokud už původní vzdálenost ušlá rovně byla dost velká,
 * vynuluje ji.
 * @param position pozice, podle které máme obnovit vzdálenost
 */
void AI_4::updateDistance_(position_t & position){
	// spocitam vzdalenost nove uslou po ose x a y
	Uint16 distance_x =
		abs_minus(position.x, positions_[POS_STAY].x);
	Uint16 distance_y =
		abs_minus(position.y, positions_[POS_STAY].y);
	// vetsinou chodim jen jednim smerem (x nebo y)
	// pokud jdu po x i y, pocitam to jako kdybych sel kazdou zvlast
	// cili je to trochu dal nez kdybych sel sikmo, ale to nevadi
	Uint16 distance = distance_x + distance_y;

	if(distanceWalkedStraight_ < minDistanceWalkedStraight_) {
		// jdu porad rovne, zvetsim vzdalenost uslou rovne
		distanceWalkedStraight_ += distance;
	}
	else {
		// nejdu rovne, vzdalenost uslou rovne zahodim
		// a zacnu ji pocitat znova
		distanceWalkedStraight_ = distance;
	}
}

/************************ AI_5 **************************/

AI_5::AI_5(Creature *creature):
	AI_4(creature, isTypeOf::isWallBoxBombFlame,
		AI_5_MIN_DISTANCE_WALKED_STRAIGHT ) {

}

/************************ AI_6 **************************/

AI_6::AI_6(Creature *creature):
	AI_4(creature, isTypeOf::isWallBoxBombFlame,
		AI_6_MIN_DISTANCE_WALKED_STRAIGHT ),
	isBad_(isTypeOf::isWallBoxBombFlamePresumption) {

}

AI_6::AI_6(Creature *creature, Uint16 minDistance):
	AI_4(creature, isTypeOf::isWallBoxBombFlame, minDistance),
	isBad_(isTypeOf::isWallBoxBombFlamePresumption) {

}

/** @details
 * Obnoví možné pozice, vybere z nich pomocí findPosIndex(),
 * přičemž se vyhýbá špatným objektům.
 * Kdyby měl zůstat stát na špatném objektu,
 * použije ještě jednou findPosIndex(),
 * tentokrát s ohledem pouze na blokující objekty.
 * Obnoví vzdálenost ušlou rovně, nastaví vybranou pozici.
 */

void AI_6::move() {
	updatePositions();
	PositionIndex posIndex = findPosIndex(isBad_);
	// kdyby mel zustat stat
	if(posIndex == POS_STAY){
		bool isInPresumption =
			positions_[posIndex].field_withObject(
				isTypeOf::isPresumption);
		if(isInPresumption){
			posIndex = findPosIndex(isBlocking_);
		}
	}
	updateDistance_(positions_[posIndex]);
	setPosition(positions_[posIndex]);
}


/************************ AI_ShortAttack **************************/
AI_ShortAttack::AI_ShortAttack(Creature * creature):
	creature_(creature), targetFound_(false){

}

/** @details
 * Nastaví currPosition_ na aktuální pozici příšery.
 * Pokud najde hráče, na kterého může útočit,
 * nastaví targetFound_ na TRUE a target_relative_* na pozici kam se má útočit.
 * Pokud nenajde hráče, nastaví targetFound_ na FALSE.
 */
void AI_ShortAttack::updateTargetPosition(){
	currPosition_.d = creature_->d_;
	currPosition_.x = creature_->x_;
	currPosition_.y = creature_->y_;

	Uint16 curr_x = currPosition_.x / CELL_SIZE;
	Uint16 curr_y = currPosition_.y / CELL_SIZE;
	// projde políčka okolo mého
	for(Sint16 relative_x = -1 ; relative_x <= 1 ; ++relative_x){
		for(Sint16 relative_y = -1 ; relative_y <= 1 ; ++relative_y){
			// podiva se jestli je hrac na policku
			targetFound_ = GAME->field_withObject(
				curr_x + relative_x,
				curr_y + relative_y,
				isTypeOf::isPlayer );
			// hrace na policku nasel, este neni jasne jestli k nemu muze
			if(targetFound_){
				if(trySetTargetPosition_(curr_x, curr_y,
						relative_x, relative_y)) {
					return;
				} else {
					targetFound_ = false;
				}
			}
		}
	}
}

/** @details
 * Nastaví útočící pozici pokud může.
 * Útočit přímo může vždy.
 * POZOR že může útočit přímo ikdyž vchází na blokované políčko,
 * to se stává často když hráč položí bombu a zůstane na ní stát.
 * Ůtočit šikmo (dvěma směry naráz) může pouze když je alespoň jeden směr volný.
 * @param field_x x-ová souřadnice výchozího políčka
 * @param field_y y-ová souřadnice výchozího políčka
 * @param relative_x relativní x-ová souřadnice cíle
 * @param relative_y relativní y-ová souřadnice cíle
 * @return TRUE pokud je alespon jeden směr pro útok volný.
 *	Pokud není volný, nenastaví souřadnice cíle a vrací FALSE.
 */
bool AI_ShortAttack::trySetTargetPosition_(Uint16 field_x, Uint16 field_y,
				Sint16 relative_x, Sint16 relative_y){
	// utok naprimo
	if(relative_x==0 || relative_y==0){
		target_relative_x_ = relative_x;
		target_relative_y_ = relative_y;
		return true;
	}
	// utok do rohu
	// musime zajistit abychom neutocili pres policko na ktere nemuzem
	bool blocked_x =
		GAME->field_withObject(
			field_x + relative_x, field_y,
			isTypeOf::isWallBoxBomb);
	bool blocked_y =
		GAME->field_withObject(
			field_x, field_y + relative_y,
			isTypeOf::isWallBoxBomb);
	if(!blocked_x || !blocked_y){
		// pokud je x-ova souradnice blokovana, nemuzu se po ni hnout)
		target_relative_x_ = blocked_x ? 0 : relative_x;
		// stejne jako u x
		target_relative_y_ = blocked_y ? 0 : relative_y;
		return true;
	} else {
		return false;
	}
}

/** @details
 * Podle target_relative_* vytvoří útočící pozici.
 * Útočící pozice je vždy vzdálena od aktuální maximálně
 * dvojnásobek rychlosti příšery, průměrně však 1.45 násobek.
 * Jednak chceme nablízko útočit o něco rychleji, jednak když
 * použijeme rychlost příšery v obou směrech, jdeme po úhlopříčce
 * (a ta je sqrt(2) krát větší než kdybychom šli ortogonálně).
 */
AI::position_t AI_ShortAttack::createPositionToAttack_(){
	if(!targetFound()){
		return currPosition_;
	}

	AI::position_t newPos = currPosition_;
	bool isInCorner =
		target_relative_x_!=0 && target_relative_y_!=0;

	Uint16 speed = creature_->getSpeed();
	if(!isInCorner){
		// kdyz jde po uhlopricce, jde implicitne sqrt(2) krat rychleji
		// pro utok primo zvetsime rychlost explicitne
		speed *= rand()%2 + 1;
	}

	// vyresim policka nahore, dole
	if(target_relative_y_ < 0){
		newPos.y -= speed;
		newPos.d = UP;
	} else if(target_relative_y_ > 0){
		newPos.y += speed;
		newPos.d = DOWN;
	}

	// vyresim policka vlevo, vpravo
	if(target_relative_x_ < 0){
		newPos.x -= speed;
		if(!isInCorner){
			newPos.d = LEFT;
		}
	} else if(target_relative_x_ > 0){
		newPos.x += speed;
		if(!isInCorner){
			newPos.d = RIGHT;
		}
	}

	return newPos;
}

/************************ AI_7 **************************/
AI_7::AI_7(Creature * creature):
	AI_2(creature), AI_ShortAttack(creature){

}

/** @details
 * Pokusí se najít cíl pro útok nablízko,
 * pokud se podaří, zaútočí, jinak se hýbe podle AI_2::move().
 */
void AI_7::move(){
	updateTargetPosition();
	if(targetFound()){
		position_t newPosition =
			createPositionToAttack_();
		setPosition(newPosition);
	} else {
		AI_2::move();
	}

}

/************************ AI_8 **************************/
AI_8::AI_8(Creature * creature):
	AI_6(creature), AI_ShortAttack(creature){

}

/** @details
 * Pokusí se najít cíl pro útok nablízko,
 * pokud se podaří, zaútočí, jinak se hýbe podle AI_6::move().
 */
void AI_8::move(){
	updateTargetPosition();
	if(targetFound()){
		position_t newPosition =
			createPositionToAttack_();
		setPosition(newPosition);
	} else {
		AI_6::move();
	}

}

/************************ AI_9 **************************/

AI_9::AI_9(Creature *creature):
	AI_6(creature, AI_9_MIN_DISTANCE_WALKED_STRAIGHT) {

}

/** @details
 * Pokud je v nebezpečí, zachová se chytře a vyhledá
 * bezpečné políčko.
 * Jinak se zachová standardně podle findPosIndex().
 */
void AI_9::move() {
	updatePositions();
	bool isInPresumption =
		positions_[POS_STAY].field_withObject(
			isTypeOf::isPresumption);
	// index nove pozice
	PositionIndex posIndex;
	// ma jit jeste rovne
	if(isInPresumption){
		if(distanceWalkedStraight_ < minDistanceWalkedStraight_) {
			posIndex =
				findPosIndexToWalkStraight_(isBlocking_);
		} else {
			posIndex =
				findPosIndexToWalkFromRisk_(isBlocking_);
		}
	} else {
		posIndex = findPosIndex(isBad_);
	}
	updateDistance_(positions_[posIndex]);
	setPosition(positions_[posIndex]);
}

/** @details
 * Předpokládá, že se nacházíme v nebezpečí a snaží se najít bezpečné políčko.
 * Výsledkem je pozice, která nás přiblíží nejbližšímu bezpečnému políčku.
 * @param isBlocking predikát odhalující blokující prvek na políčku
 * @return Vrací index pozice, na kterou bychom měli jít.
 */
AI::PositionIndex AI_9::findPosIndexToWalkFromRisk_(isTypeOf & isBlocking){
	traceArray_t traceArray;
	// ohodnocení a nalezení cíle
	evalTraceArray_(traceArray, isBlocking, isTypeOf::isPresumption, UNWANTED);
	if(targetFound()){
		// backtracking, nalezeni vysledne pozice
		return findPosIndexUsingBacktracking_(traceArray);
	} else {
		return findPosIndex(isBlocking);
	}
}

/** @details
 * Nastartuje rekurzivní ohodnocení trasy.
 * Inicializuje frontu políček, začínáme u aktuální pozice.
 * @param traceArray inicializované trasovací pole
 * @param isBlocking predikát odhalující blokující objekty
 * @param isInteresting predikát odhalující objekty,
 * které jsou zajímavé (svou přítomností nebo naopak nepřítomností)
 * @param interestArea upřesňuje, jak jsou zajímavé objekty zajímavé
 */
void AI_9::evalTraceArray_(traceArray_t & traceArray,
			isTypeOf & isBlocking, isTypeOf & isInteresting, wanted_t interestArea){
	// pocatecni souradnice
	traceField_t currField(
		positions_[POS_STAY].x / CELL_SIZE,
		positions_[POS_STAY].y / CELL_SIZE);
	// nastavit pocatecni policko
	traceArray[currField].setDepth(0);
	// pripravit pocatecni policko do fronty
	fieldsQueue_t fieldsQueue;
	fieldsQueue.push(currField);
	// rekurzivne ohodnotit trasovaci pole
	recursiveEvalTraceArray_(traceArray, fieldsQueue,
		isBlocking, isInteresting, interestArea);
}

/** @details
 * Rekurzivní ohodnocení trasy řeší aktuálně jedno políčko ve frontě.
 * Prohledáváním do šířky se snaží najít políčko, které nás zajímá.
 * V jednom kroku vyřeší políčko, které je první ve frontě, odebere ho
 * a přidá do ní nejvýše čtyři sousední políčka (pokud bychom na ně mohli jít).
 * @param traceArray inicializované trasovací pole
 * @param fieldsQueue fronta políček k prohledávání
 * @param isBlocking predikát odhalující blokující objekty
 * @param isInteresting predikát odhalující objekty,
 * které jsou zajímavé (svou přítomností nebo naopak nepřítomností)
 * @param interestArea upřesňuje, jak jsou zajímavé objekty zajímavé
 */
void AI_9::recursiveEvalTraceArray_(
			traceArray_t & traceArray, fieldsQueue_t & fieldsQueue,
			isTypeOf & isBlocking, isTypeOf & isInteresting,
			wanted_t interestArea){
	// konec rekurze
	if(fieldsQueue.empty()){
		// nenasli jsme cil a prohledali jsme co se dalo
		targetFound_ = false;
		return;
	}
	// aktualni pozice
	const traceField_t & currField = fieldsQueue.front();
	// nasli jsme neco zajimaveho?
	if(interestingFound_(currField, isInteresting, interestArea)){
		// nastavime cil na souradnice tohoto kroku
		targetFound_ = true;
		targetField_ = currField;
		return;
	}
	// ohodnotit a pripadne vlozit do fronty sousedici policka
	evalAndQueueNextFields_(traceArray, fieldsQueue, isBlocking);
	// vyhodim z fronty vyresene policko
	fieldsQueue.pop();
	// vyresim dalsi policko ve fronte
	recursiveEvalTraceArray_(traceArray, fieldsQueue, isBlocking,
		isInteresting, interestArea);
}

/** @details
 * Zjistí, jestli je zadané políčko v mapě zajímavé či nikoli.
 * @param field políčko v mapě
 * @param isInteresting predikát odhalující objekty,
 * které jsou zajímavé (svou přítomností nebo naopak nepřítomností)
 * @param interestArea upřesňuje, jak jsou zajímavé objekty zajímavé
 * @return TRUE pokud je zadané políčko zajímavé, jinak false.
 * @throw string Chybová hláška,
 	pokud nejsou ošetřeny všechny oblasti zajímavosti.
 */
bool AI_9::interestingFound_(const traceField_t & field,
			isTypeOf & isInteresting, wanted_t interestArea){
	bool field_withInteresting =
		GAME->field_withObject(
			field.first, field.second, isInteresting);
	switch(interestArea){
		case WANTED:
			return field_withInteresting;
		case UNWANTED:
			return !field_withInteresting;
		default:
			throw string("Invalid wanted type in AI_9::interestingFound_()");
	}
}

/** @details
 * Sousední políčka jsou políčka vlevo, vpravo, nad a pod
 * políčkem, které je první ve frontě (aktuální políčko).
 * Tato políčka, pokud již nebyla hodnocena označí.
 * Nastavuje, že na něj nemůžeme vstoupit nebo jim nastaví hloubku prohledávání,
 * v jaké jsme schopni na ně dojít.
 * Navíc políčka, na která můžeme vstoupit, vloží do fronty pro pozdější zpracování.
 * Do fronty nevkládá taková políčka, která by měla hloubku větší
 * než povolenou v AI_9_MAX_TRACE_DEPTH.
 * @param traceArray trasovací pole pro ohodnocení
 * @param fieldsQueue fronta políček
 * @param isBlocking predikát určující políčka, na která nemůžeme vstoupit
 */
void AI_9::evalAndQueueNextFields_(traceArray_t & traceArray,
			fieldsQueue_t & fieldsQueue, isTypeOf & isBlocking){
	const traceField_t & currField = fieldsQueue.front();
	Uint16 curr_depth = traceArray[currField].getDepth();
	// prilis velka hloubka prohledavani, dale nebudeme prohledavat
	if(curr_depth >= AI_9_MAX_TRACE_DEPTH){
		return;
	}
	vector<DIRECTION> directions(BURNED);
	directions.push_back(UP);
	directions.push_back(RIGHT);
	directions.push_back(DOWN);
	directions.push_back(LEFT);
	random_shuffle(directions.begin(), directions.end());
	for(Uint16 i = 0 ; i < directions.size() ; ++i){
		// souradnice
		traceField_t nextField = currField;
		moveFieldCoordinate_(nextField, directions[i]);

		if(traceArray[nextField].wasNotTraced()){
			// na policko nemuzu => oznacim ho
			bool isNextFieldBlocked =
				GAME->field_withObject(
					nextField.first, nextField.second,
					isBlocking);
			if(isNextFieldBlocked){
				// oznacim ho abych ho uz priste netestoval
				traceArray[nextField].setCannotGoOver();
			} else {
				// nastavim hloubku a odkud jsem tam prisel
				traceArray[nextField].setDepth(curr_depth +1);
				traceArray[nextField].setFromField(currField);
				// vlozim do fronty
				fieldsQueue.push(nextField);
			}
		}
	}
}

/** @details
 * Posune políčko na sousední podle zadaného směru.
 * @param field políčko, které chceme posunout
 * @param dir směr, podle kterého budeme posunovat
 * @throw string Chybová hláška,
 *	pokud nejsou ošetřeny všechny směry.
 */
void AI_9::moveFieldCoordinate_(traceField_t & field, DIRECTION dir){
	switch(dir){
		case UP:
			--field.second;
			break;
		case DOWN:
			++field.second;
			break;
		case LEFT:
			--field.first;
			break;
		case RIGHT:
			++field.first;
			break;
		case BURNED:
			// nikam neposouvam
			break;
		default:
			throw string("Invalid direction in AI_9::moveFieldCoordinate_()");
	}
}

/** @details
 * Podle výsledku prohledávání do šířky provede backtracking,
 * vybere výslednou pozici, která nejlépe využije směr.
 * Využívá member položku targetField_,
 * kontroluje její správnost pomocí targetFound().
 * @param traceArray ohodnocené trasovací pole
 */
AI::PositionIndex AI_9::findPosIndexUsingBacktracking_(
			traceArray_t & traceArray){
	if(!targetFound()){
		return POS_STAY;
	}
	// backtracking na hloubku 1
	traceField_t field = targetField_;
	while(traceArray[field].getDepth() > 1){
		field = traceArray[field].getFromField();
	}

	// moje souradnice
	Uint16 curr_x = positions_[POS_STAY].x / CELL_SIZE;
	Uint16 curr_y = positions_[POS_STAY].y / CELL_SIZE;
	// zda-li je mensi souradnice policka velde nez ta soucasna
	int next_vs_curr_x = sgn_minus(field.first, curr_x);
	int next_vs_curr_y = sgn_minus(field.second, curr_y);
	// pres pozice na ktere bych se mohl hnout
	for(Uint16 i = POS_STRAIGHT ; i < POS_LAST ; ++i){
		int i_vs_stay_x =
			sgn_minus(positions_[i].x, positions_[POS_STAY].x);
		int i_vs_stay_y =
			sgn_minus(positions_[i].y, positions_[POS_STAY].y);
		// pokud jsou v poradku pomery na obou souradnicich
		if(next_vs_curr_x==i_vs_stay_x && next_vs_curr_y==i_vs_stay_y){
			return static_cast<PositionIndex>(i);
		}
	}
	// nenasel jsem vhodnou pozici, zustanu stat
	return POS_STAY;
}

/************************ AI_10 **************************/

AI_10::AI_10(Creature *creature):
			AI_9(creature){

}

/** @details
 * Najde index pomocí findPosIndexToWalkStraight_(),
 * pokud ušel dostatečnou vzdálenost rovně,
 * použije findPosIndexToComeCloseToPlayer_().
 * @param isBlocking predikát určující blokující objekty
 * @return Index pozice, na kterou máme jít.
 */
AI::PositionIndex AI_10::findPosIndex(isTypeOf & isBlocking){
	if(distanceWalkedStraight_ < minDistanceWalkedStraight_) {
		return findPosIndexToWalkStraight_(isBlocking);
	} else {
		return findPosIndexToComeCloseToPlayer_(isBlocking);
	}
}


/** @details
 * Snaží se v mapě vyhledat hráče, na kterého by mohla zaútočit.
 * Výsledkem je pozice, která nás přiblíží nejbližšímu hráči,
 * nebo náhodná pozice, pokud jsme hráče neobjevili.
 * @param isBlocking predikát odhalující blokující prvek na políčku
 * @return Vrací index pozice, na kterou bychom měli jít.
 */
AI::PositionIndex AI_10::findPosIndexToComeCloseToPlayer_(
					isTypeOf & isBlocking){
	traceArray_t traceArray;
	// ohodnocení a nalezení cíle
	evalTraceArray_(traceArray, isBlocking, isTypeOf::isPlayer, WANTED);
	if(targetFound()){
		// backtracking, nalezeni vysledne pozice
		return findPosIndexUsingBacktracking_(traceArray);
	} else {
		return findPosIndexToWalkRandomly_(isBlocking);
	}
}


/************************ AI_fromKeybord **************************/

AI_fromKeyboard::AI_fromKeyboard(Creature *creature): AI(creature) {
	 keystate_= SDL_GetKeyState(0);
}

/** @details
 * Provede update možných pozic jako bych byl otočený nahoru.
 * Postupně zavolá všechny handlery kláves.
 */
void AI_fromKeyboard::move() {
	// nemelo by se stat ze mame AI_fromKeyboard prirazenou prisere
	Player * player = static_cast<Player *>(creature_);
	// update pozic pro pohyb jako bych byl otoceny nahoru
	DIRECTION cur_d = player->d_;
	player->d_ = UP;
	updatePositions();
	player->d_ = cur_d;

	bool moved = handleMoveKeys_(player);
	handlePlantKey_(player, moved);
	handleTimerKey_(player);
}

/** @details
 * Pro stisknuté klávesy nastaví směr hráče
 * a pokusí se hráčem hýbnout.
 * @param player hráč, pro nějž handler voláme
 * @return TRUE pokud se hráč pohnul.
 */
bool AI_fromKeyboard::handleMoveKeys_(Player * player){
	Uint16 num = player->player_num();

	if(keystate_[CONFIG->player(num, KEY_UP)]){
		player->d_ = UP;
		return trySetPosition_(player, positions_[POS_STRAIGHT]);
	}
	else if(keystate_[CONFIG->player(num, KEY_RIGHT)]){
		player->d_ = RIGHT;
		return trySetPosition_(player, positions_[POS_RIGHT]);
	}
	else if(keystate_[CONFIG->player(num, KEY_DOWN)]){
		player->d_ = DOWN;
		return trySetPosition_(player, positions_[POS_BACK]);
	}
	else if(keystate_[CONFIG->player(num, KEY_LEFT)]){
		player->d_ = LEFT;
		return trySetPosition_(player, positions_[POS_LEFT]);
	}
	else {
		return false;
	}
}

/** @details
 * Pokusí se hráči nastavit novou pozici.
 * Při tom se zabývá tím, aby hráč nevlezl někam kam nesmí.
 * Řeší kopání do bomb.
 * Když je možné hráče posunout ve směru stanoveném pozicí
 * alespon trochu, funkce tak učiní.
 * @param player hráč, pro nějž handler voláme
 * @param position nová pozice pro nastavení
 * @return TRUE pokud se hráč pohnul.
 */
bool AI_fromKeyboard::trySetPosition_(Player * player, position_t & position) {
	// souradnice v polickach
	Uint16 field_x = position.x/CELL_SIZE;
	Uint16 field_y = position.y/CELL_SIZE;
	// nemuzu tam vubec vstoupit, koncim
	if(!GAME->field_canGoOver(field_x, field_y, false)){
		return false;
	}

	// faktor posunu souradnic (normalizovany rozdil)
	int factor_x = sgn_minus(position.x, positions_[POS_STAY].x);
	int factor_y = sgn_minus(position.y, positions_[POS_STAY].y);

	// podle faktoru pohybu a pozice na policku
	// zjistim jestli muzu na policku za pulku
	if((factor_x < 0  &&  position.x%CELL_SIZE < CELL_SIZE/2)
	|| (factor_x > 0  &&  position.x%CELL_SIZE > CELL_SIZE/2)
	|| (factor_y < 0  &&  position.y%CELL_SIZE < CELL_SIZE/2)
	|| (factor_y > 0  &&  position.y%CELL_SIZE > CELL_SIZE/2) ){

		// zajimavy objekt na nasledujicim policku
		MapObject * obj = GAME->field_getObject(
			field_x + factor_x, field_y + factor_y,
			isTypeOf::isWallBoxAnyBomb);
		// nasli jsme zajimavy objekt, podivame se co s nim
		if(obj){
			// jakakoli bomba, zkusim do ni kopnout (jestli muzu)
			bool isObjAnyBomb =
				obj->type()==BOMB_STAYING || obj->type()==BOMB_MOVING;
			if(isObjAnyBomb && player->bonus_kicker_ ){
				Bomb * bomb = static_cast< Bomb * >(obj);
				bomb->kick(player->d_);
				// hybe se, podarilo se do ni kopnout
				if(bomb->type()==BOMB_MOVING){
					// po kopnuti se nikam neposunuju
					position = positions_[POS_STAY];
				}
			}
			// WALL | BOX | BOMB_STAYING (po kopnuti se nehybe)
			if(obj->type()!=BOMB_MOVING){
				// nastavim prostredek policka v tom smeru,
				// ve kterem se hybu
				if(factor_x) {
					position.x = field_x * CELL_SIZE
						+ CELL_SIZE/2;
				}
				if(factor_y) {
					position.y = field_y * CELL_SIZE
						+ CELL_SIZE/2;
				}
			}
		}
	}

	// podle faktoru pohybu zjistim, jestli se vubec hybu v zadanem smeru
	if((factor_x < 0  &&  position.x < positions_[POS_STAY].x)
	|| (factor_x > 0  &&  position.x > positions_[POS_STAY].x)
	|| (factor_y < 0  &&  position.y < positions_[POS_STAY].y)
	|| (factor_y > 0  &&  position.y > positions_[POS_STAY].y) ) {
		// pokud se hybu v zadanem smeru, nastavim novou pozici
		setPosition(position);
		return true;
	}
	else {
		return false;
	}
}


/** @details
 * Vytváří a vkládá do mapy bomby.
 * Řeší firemana, megabomby a posílání bomb.
 * @param player hráč, pro nějž handler voláme
 */
void AI_fromKeyboard::handlePlantKey_(Player * player, bool playerMoved){
	Uint16 num = player->player_num();
	// neni stisknuta klavesa pokladani bomby => koncime
	if(!keystate_[CONFIG->player(num, KEY_PLANT)]){
		return;
	}
	// souradnice v polickach
	Uint16 field_x = player->x_ / CELL_SIZE;
	Uint16 field_y = player->y_ / CELL_SIZE;

	// bomba pro vlozeni do mapy
	Bomb * bomb =0;

	// pokusim se vytvorit bombu kterou potrebuju
	if(player->bonus_fireman_){
		// uz muzu pokladat dalsi bombu firemanem
		if(player->next_timer_ < MOVE_PERIOD){
			// fireman ma velikost plamene 1
			bomb = GAME->tools->bomb_normal(field_x, field_y, 1);
			// hned bouchne
			bomb->explode();
			// nemuzeme hned pokladat dalsi
			player->next_timer_ = TIMER_PERIOD;
		}
	} else {
		// jestli muze na policko pokladat bombu
		bool canPlantBomb = !GAME->field_withObject(
			field_x, field_y,
			isTypeOf::isWallBoxAnyBombFlame);
		if(canPlantBomb){
			// kdyz ma megabomby pouzije ji
			if(player->megabombs_ > 0){
				--player->megabombs_;
				bomb = GAME->tools->bomb_mega(field_x, field_y,
					player->flamesize_, player->bonus_timer_);
			}
			// jinak kdyz jeste nepolozil vsechny bomby
			else if(GAME->count_bombs(num) < player->bombs_){
				bomb = GAME->tools->bomb_normal(field_x, field_y,
					player->flamesize_, player->bonus_timer_);
			}
		}
	}

	// vytvorili jsme nejakou bombu, polozime ji
	if(bomb){
		// vlozime ji do hry (mapy)
		GAME->plant_bomb(num, field_x, field_y, bomb);

		// kdyz ma hrac posilani a nehybe se
		if(player->bonus_slider_ && !playerMoved){
			// tak do bomby jakoby kopne (posle ji)
			bomb->kick(player->d_);
		}
	}
}

/** @details
 * Řeší ruční odpalování.
 * @param player hráč, pro nějž handler voláme
 */
void AI_fromKeyboard::handleTimerKey_(Player * player){
	Uint16 num = player->player_num();
	// neni stisknuta klavesa => koncime
	if(!keystate_[CONFIG->player(num, KEY_TIMER)]){
		return;
	}
	// mame bonus odpalovani, muzeme hned odpalovat
	if(player->bonus_timer_ && player->next_timer_ < MOVE_PERIOD){
		// odpalime jednu bombu
		GAME->explode_bomb(num);
		// dalsi odpalovani pozdrzime
		player->next_timer_ = TIMER_PERIOD;
	}
}
