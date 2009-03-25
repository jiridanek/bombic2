
#include <vector>
#include <algorithm>
#include <string>
#include "SDL_lib.h"
#include "stl_helper.h"
#include "constants.h"
#include "config.h"
#include "game.h"
#include "game_mapobjects.h"
#include "game_AI.h"
#include "game_creature.h"
#include "game_player.h"
#include "game_bomb.h"

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
	if(positions_.size() < POS_COUNT) {
		position_t default_pos;
		positions_.clear();
		positions_.insert(
			positions_.end(), POS_COUNT, default_pos);
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
			throw(string("Invalid direction in AI::updatePositions()"));
	}
	//souradnice pro otoceni a jeden krok vpred
	positions_[i].d = UP;
	positions_[i].x = creature_->x_;
	positions_[i].y = creature_->y_ - creature_->speed_diff_;
	i = i%4 + 1;
	positions_[i].d = RIGHT;
	positions_[i].x = creature_->x_ + creature_->speed_diff_;
	positions_[i].y = creature_->y_;
	i = i%4 + 1;
	positions_[i].d = DOWN;
	positions_[i].x = creature_->x_;
	positions_[i].y = creature_->y_ + creature_->speed_diff_;
	i = i%4 + 1;
	positions_[i].d = LEFT;
	positions_[i].x = creature_->x_ - creature_->speed_diff_;
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
		default:
			break;
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
	Uint8 diff_speed = (creature_->speed_diff_ + 1) / 2;
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
		default:
			break;
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

void AI_0::move() {

}

/************************ AI_1 **************************/

AI_1::AI_1(Creature *creature):
	AI(creature), isBlocking_(isTypeOf::isWallBoxBomb) {

}

AI_1::AI_1(Creature *creature, isTypeOf & isBlocking):
	AI(creature), isBlocking_(isBlocking) {

}

void AI_1::move() {
	updatePositions();
	Uint16 posIndex = findPosIndexToWalkStraight_(isBlocking_);
	setPosition(positions_[posIndex]);
}

AI::PositionIndex AI_1::findPosIndexToWalkStraight_(isTypeOf & isBlocking){
	// vpred
	if(checkField(positions_[POS_STRAIGHT], isBlocking)){
		return POS_STRAIGHT;
	}
	// zjistim pocet neblokovanych moznosti
	Uint16 nonBlockedPositions = 0;
	// pres zbyvajici pozice
	for(Uint16 i = POS_STRAIGHT +1 ; i < POS_COUNT ; ++i){
		// ulozim si info o blokovani
		if(checkField(positions_[i], isBlocking)){
			positions_[i].isBlocked = false;
			// zapocitam jako neblokovanou
			++nonBlockedPositions;
		} else {
			positions_[i].isBlocked = true;
		}
	}

	for(Uint16 i = POS_STRAIGHT +1 ; i < POS_COUNT ; ++i){
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

void AI_3::move() {
	updatePositions();
	PositionIndex posIndex = findPosIndexToWalkStraight_(isBad_);
	// kdyby mel zustat stat
	if(posIndex == POS_STAY){
		bool isInPresumption =
			GAME->field_withObject(
				positions_[posIndex].x / CELL_SIZE,
				positions_[posIndex].y / CELL_SIZE,
				isTypeOf::isPresumption );
		if(isInPresumption){
			posIndex = findPosIndexToWalkStraight_(isBlocking_);
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

void AI_4::move() {
	updatePositions();
	PositionIndex posIndex;
	if(distanceWalkedStraight_ < minDistanceWalkedStraight_) {
		posIndex = findPosIndexToWalkStraight_(isBlocking_);
	} else {
		posIndex = findPosIndexToWalkRandomly_(isBlocking_);
	}
	updateDistance_(positions_[posIndex]);
	setPosition(positions_[posIndex]);
}

AI::PositionIndex AI_4::findPosIndexToWalkRandomly_(isTypeOf & isBlocking){
	// zjistim pocet neblokovanych moznosti
	Uint16 nonBlockedPositions = 0;
	// pres vsechny pozice
	for(Uint16 i = POS_STRAIGHT ; i < POS_COUNT ; ++i){
		// ulozim si info o blokovani
		if(checkField(positions_[i], isBlocking)){
			positions_[i].isBlocked = false;
			// zapocitam jako neblokovanou
			++nonBlockedPositions;
		} else {
			positions_[i].isBlocked = true;
		}
	}

	for(Uint16 i = POS_STRAIGHT ; i < POS_COUNT ; ++i){
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

/************************ AI_10 **************************/

AI_10::AI_10(Creature *creature):
			AI(creature), isBlocking_(isTypeOf::isWallBoxBombFlame),
			isBad_(isTypeOf::isWallBoxBombFlamePresumption),
			target_x_(1), target_y_(1), last_trace_update_(0) {}

void AI_10::move() {
	// pripravim cilove pozice
	updatePositions();
	// pokud se nehybam nebo uz jsem se hybal hodne
	if(!last_trace_update_
	|| last_trace_update_ >= AI_10_MAX_UPDATE_PERIOD){
			last_trace_update_ = rand()%AI_10_MAX_UPDATE_PERIOD /2;
			// pripravim trasovaci pole
			update_trace_array_();
	}

	// najdu vhodnou pozici
	position_t & position = get_position_();
	if(!checkField(position, isBlocking_))
		position = get_random_position_();
	// prictu o kolik se posunu
	last_trace_update_ += abs_minus(position.x, positions_[0].x)
		+ abs_minus(position.y, positions_[0].y);
	// nastavim pozici
	setPosition(position);
}

void AI_10::update_trace_array_(){
	// vytvoreni trasovaciho pole s hodnotami -1
	if(empty_trace_array_.empty()){
		vector< Sint16 > empty_column;
		empty_column.insert(empty_column.end(), GAME->map_height(), -1);
		empty_trace_array_.insert(
			empty_trace_array_.end(), GAME->map_width(), empty_column);
	}

	// nastavit pocatecni souradnice
	old_x_ = positions_[0].x/CELL_SIZE;
	old_y_ = positions_[0].y/CELL_SIZE;
	// zkopirovat prazdne pole
	trace_array_ = empty_trace_array_;
	trace_array_[old_x_][old_y_] = 0;
	// pripravit pocatecni policko do fronty
	fields_queue_t fields_queue;
	fields_queue.push(make_pair(old_x_, old_y_));
	// ohodnotit trasovaci pole
	eval_trace_array_(fields_queue,
		GAME->field_withObject(old_x_, old_y_, isBad_));
}

void AI_10::eval_trace_array_(fields_queue_t & fields_queue, bool in_danger ){
	if(fields_queue.empty())
		return;
	// obnovit souradnice cile
	target_x_ = fields_queue.front().first,
	target_y_ = fields_queue.front().second;
	// mame hrace, koncime a jdeme na nej
	if(GAME->field_withObject(target_x_, target_y_, PLAYER))
		return;
	// jsem v ohrozeni a nasel jsem dobre policko, koncim
	if(in_danger
	&& !GAME->field_withObject(target_x_, target_y_, isBad_))
		return;

	// pripravim novou hodnotu policka
	Sint16 val = trace_array_[target_x_][target_y_]+1;

	Uint16 i, dir, next_x, next_y;
	// pres vsechny smery
	if(val <= AI_10_MAX_TRACE_DEPTH){
		// trocha nahody
		for(i=0, dir=rand()%4 ; i<4 ; ++i, ++dir){
			// souradnice
			next_x = target_x_; next_y = target_y_;
			if(dir%2)
				next_x += 2-dir%4;
			else
				next_y += 1-dir%4;
			// mohu na policko vstoupit
			if(trace_array_[next_x][next_y]==-1
			&& !GAME->field_withObject(next_x, next_y,
						in_danger ? isBlocking_ : isBad_)){
				// nastavim hodnotu
				trace_array_[next_x][next_y] = val;
				// vlozim do fronty
				fields_queue.push(make_pair(next_x, next_y));
			}
		}
	}

	// vyhodim z fronty
	fields_queue.pop();
	// vyresim dalsi policko
	eval_trace_array_(fields_queue, in_danger);
}

AI::position_t & AI_10::get_position_(){
	Sint16 val = trace_array_[target_x_][target_y_]-1;
	// konec rekurze
	if(val<=0){
		// nahoru
		if(target_y_ < old_y_)
			return positions_[ (4+UP-positions_[0].d)%4+1 ];
		// dolu
		if(target_y_ > old_y_)
			return positions_[ (4+DOWN-positions_[0].d)%4+1 ];
		// vlevo
		if(target_x_ < old_x_)
			return positions_[ (4+LEFT-positions_[0].d)%4+1 ];
		// vpravo
		if(target_x_ > old_x_)
			return positions_[ (4+RIGHT-positions_[0].d)%4+1 ];
	}
	Uint16 dir, next_x, next_y;
	// pres vsechny smery najdu odkud jsem se sem dostal
	for(dir=0 ; dir<4 ; ++dir){
		// souradnice
		next_x = target_x_; next_y = target_y_;
		if(dir%2)
			next_x += 2-dir;
		else
			next_y += 1-dir;
		if(trace_array_[next_x][next_y]==val)
			break;
	}
	// vratim rekurzi naslou pozici
	target_x_ = next_x; target_y_ = next_y;
	return get_position_();
}

AI::position_t & AI_10::get_random_position_(){
	// vpred
	if(checkField(positions_[1], isBad_))
		return positions_[1];
	// otoceni doprava
	if(checkField(positions_[2], isBad_))
		return positions_[2];
	// otoceni doleva
	if(checkField(positions_[4], isBad_))
		return positions_[4];
	// vzad
	if(checkField(positions_[3], isBad_))
		return positions_[3];
	// vpred
	if(checkField(positions_[1], isBlocking_))
		return positions_[1];
	// otoceni doprava
	if(checkField(positions_[2], isBlocking_))
		return positions_[2];
	// otoceni doleva
	if(checkField(positions_[4], isBlocking_))
		return positions_[4];
	// vzad
	if(checkField(positions_[3], isBlocking_))
		return positions_[3];

	// zustat namiste
	return positions_[0];
}
/************************ AI_fromKeybord **************************/

AI_fromKeyboard::AI_fromKeyboard(Creature *creature): AI(creature) {
	 keystate_= SDL_GetKeyState(0);
}

void AI_fromKeyboard::move() {
	Player * player = static_cast<Player *>(creature_);
	// update pozic pro pohyb jako bych byl otoceny nahoru
	DIRECTION cur_d = player->d_;
	player->d_ = UP;
	updatePositions();
	// souradnice v polickach
	Uint16 x, y, num = player->player_num();

	if(keystate_[CONFIG->player(num, KEY_UP)]){
		player->d_ = UP;
		makePosition(positions_[1], 0, -1);
	}
	else if(keystate_[CONFIG->player(num, KEY_RIGHT)]){
		player->d_ = RIGHT;
		makePosition(positions_[2], 1, 0);
	}
	else if(keystate_[CONFIG->player(num, KEY_DOWN)]){
		player->d_ = DOWN;
		makePosition(positions_[3], 0, 1);
	}
	else if(keystate_[CONFIG->player(num, KEY_LEFT)]){
		player->d_ = LEFT;
		makePosition(positions_[4], -1, 0);
	}
	else player->d_ = cur_d;

	if(keystate_[CONFIG->player(num, KEY_PLANT)]){
		x = player->x_/CELL_SIZE;
		y = player->y_/CELL_SIZE;

		Bomb * bomb =0;
		if(player->bonus_fireman_){
			if(player->next_timer_ < MOVE_PERIOD){
				bomb = GAME->tools->bomb_normal(x, y, 1);
				bomb->explode();
				player->next_timer_ = TIMER_PERIOD;
			}
		}
		else if(!GAME->field_withObject(x, y, isTypeOf::isWallBoxAnyBombFlame)){
			if(player->megabombs_){
				bomb = GAME->tools->bomb_mega(x, y,
							player->flamesize_, player->bonus_timer_);
				--player->megabombs_;
			}
			else if(GAME->count_bombs(num)< player->bombs_)
				bomb = GAME->tools->bomb_normal(x, y,
							player->flamesize_, player->bonus_timer_);
		}
		if(bomb){
			if(player->bonus_slider_
			&& player->x_==positions_[0].x
			&& player->y_==positions_[0].y)
				bomb->kick(player->d_);
			GAME->plant_bomb(num, x, y, bomb);
		}
	}

	if(keystate_[CONFIG->player(num, KEY_TIMER)]
	&& player->bonus_timer_	&& player->next_timer_ < MOVE_PERIOD){
		GAME->explode_bomb(num);
		player->next_timer_ = TIMER_PERIOD;
	}

}

void AI_fromKeyboard::makePosition(position_t & position,
				Sint16 factor_x, Sint16 factor_y) {
	Player * player = static_cast<Player *>(creature_);

	Uint16 x=position.x/CELL_SIZE,
		y=position.y/CELL_SIZE;
	if(GAME->field_canGoOver(x, y, false)){
		if((factor_x<0 && position.x%CELL_SIZE<CELL_SIZE/2)
		|| (factor_x>0 && position.x%CELL_SIZE>CELL_SIZE/2)
		|| (factor_y<0 && position.y%CELL_SIZE<CELL_SIZE/2)
		|| (factor_y>0 && position.y%CELL_SIZE>CELL_SIZE/2) ){

			MapObject * obj = GAME->field_getObject(x+factor_x, y+factor_y,
					isTypeOf::isWallBoxAnyBomb);
			if(obj){
				// jakakoli bomba, zkusim do ni kopnout (jestli muzu)
				if(player->bonus_kicker_
				&& (obj->type()==BOMB_STAYING || obj->type()==BOMB_MOVING))
					static_cast< Bomb * >(obj)->kick(player->d_);
				// WALL | BOX | BOMB_STAYING (po kopnuti se nehybe)
				if(obj->type()!=BOMB_MOVING){
					if(factor_x)
						position.x = x*CELL_SIZE+CELL_SIZE/2;
					if(factor_y)
						position.y = y*CELL_SIZE+CELL_SIZE/2;
				}
			}
		}
		if((factor_x<0 && position.x<positions_[0].x)
		|| (factor_x>0 && position.x>positions_[0].x)
		|| (factor_y<0 && position.y<positions_[0].y)
		|| (factor_y>0 && position.y>positions_[0].y) )
			setPosition(position);
	}
}
