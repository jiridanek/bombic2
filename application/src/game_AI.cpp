
#include <vector>
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
 * Statická fce, pouze rozhodne, která třída AI se použije.
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
		case  10: return new AI_10(creature);
		default: return 0;
	}
}

/** @details
 * Statická fce, pouze rozhodne, která třída AI se použije.
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
	positions_[i].y=creature_->y_ - creature_->speed_diff_;
// 	positions_[i].y=creature_->y_ - 1;
	i=i%4+1;
	positions_[i].d=RIGHT;
	positions_[i].x=creature_->x_ + creature_->speed_diff_;
// 	positions_[i].x=creature_->x_ + 1;
	positions_[i].y=creature_->y_;
	i=i%4+1;
	positions_[i].d=DOWN;
	positions_[i].x=creature_->x_;
	positions_[i].y=creature_->y_ + creature_->speed_diff_;
// 	positions_[i].y=creature_->y_ + 1;
	i=i%4+1;
	positions_[i].d=LEFT;
	positions_[i].x=creature_->x_ - creature_->speed_diff_;
// 	positions_[i].x=creature_->x_ - 1;
	positions_[i].y=creature_->y_;
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
			if(position.y%CELL_SIZE+1<CELL_SIZE/2){
				// vlevo
				if(position.x%CELL_SIZE+1<CELL_SIZE/2){
					if(!GAME->field_canGoOver(
					position.x/CELL_SIZE-1,position.y/CELL_SIZE-1)){
						centerCoordinate(position.x, +1);
						centerCoordinate(position.y, +1);
					}
				// vpravo
				} else if(position.x%CELL_SIZE-1>CELL_SIZE/2)
					if(!GAME->field_canGoOver(
					position.x/CELL_SIZE+1,position.y/CELL_SIZE-1)){
						centerCoordinate(position.x, -1);
						centerCoordinate(position.y, +1);
					}
			} else {
				if(position.x%CELL_SIZE+1<CELL_SIZE/2){
					if(!GAME->field_canGoOver(
					position.x/CELL_SIZE-1,position.y/CELL_SIZE)){
						centerCoordinate(position.x, +1);
// 						centerCoordinate(position.y, +1);
					}
				} else if(position.x%CELL_SIZE-1>CELL_SIZE/2)
					if(!GAME->field_canGoOver(
					position.x/CELL_SIZE+1,position.y/CELL_SIZE)){
						centerCoordinate(position.x, -1);
// 						centerCoordinate(position.y, +1);
					}
			} break;
		case RIGHT:
			if(position.x%CELL_SIZE-1>CELL_SIZE/2){
				// nahore
				if(position.y%CELL_SIZE+1<CELL_SIZE/2){
					if(!GAME->field_canGoOver(
					position.x/CELL_SIZE+1,position.y/CELL_SIZE-1)){
						centerCoordinate(position.x, -1);
						centerCoordinate(position.y, +1);
					}
				// dole
				} else if(position.y%CELL_SIZE-1>CELL_SIZE/2)
					if(!GAME->field_canGoOver(
					position.x/CELL_SIZE+1,position.y/CELL_SIZE+1)){
						centerCoordinate(position.x, -1);
						centerCoordinate(position.y, -1);
					}
			} else {
				if(position.y%CELL_SIZE+1<CELL_SIZE/2){
					if(!GAME->field_canGoOver(
					position.x/CELL_SIZE,position.y/CELL_SIZE-1)){
// 						centerCoordinate(position.x, -1);
						centerCoordinate(position.y, +1);
					}
				} else if(position.y%CELL_SIZE-1>CELL_SIZE/2)
					if(!GAME->field_canGoOver(
					position.x/CELL_SIZE,position.y/CELL_SIZE+1)){
// 						centerCoordinate(position.x, -1);
						centerCoordinate(position.y, -1);
					}

			} break;
		case DOWN:
			if(position.y%CELL_SIZE-1>CELL_SIZE/2){
				if(position.x%CELL_SIZE+1<CELL_SIZE/2){
					if(!GAME->field_canGoOver(
					position.x/CELL_SIZE-1,position.y/CELL_SIZE+1)){
						centerCoordinate(position.x, +1);
						centerCoordinate(position.y, -1);
					}
				} else if(position.x%CELL_SIZE-1>CELL_SIZE/2)
					if(!GAME->field_canGoOver(
					position.x/CELL_SIZE+1,position.y/CELL_SIZE+1)){
						centerCoordinate(position.x, -1);
						centerCoordinate(position.y, -1);
					}
			} else {
				if(position.x%CELL_SIZE+1<CELL_SIZE/2){
					if(!GAME->field_canGoOver(
					position.x/CELL_SIZE-1,position.y/CELL_SIZE)){
						centerCoordinate(position.x, +1);
// 						centerCoordinate(position.y, -1);
					}
				} else if(position.x%CELL_SIZE-1>CELL_SIZE/2)
					if(!GAME->field_canGoOver(
					position.x/CELL_SIZE+1,position.y/CELL_SIZE)){
						centerCoordinate(position.x, -1);
// 						centerCoordinate(position.y, -1);
					}
			} break;
		case LEFT:
			if(position.x%CELL_SIZE+1<CELL_SIZE/2){
				if(position.y%CELL_SIZE+1<CELL_SIZE/2){
					if(!GAME->field_canGoOver(
					position.x/CELL_SIZE-1,position.y/CELL_SIZE-1)){
						centerCoordinate(position.x, +1);
						centerCoordinate(position.y, +1);
					}
				} else if(position.y%CELL_SIZE-1>CELL_SIZE/2)
					if(!GAME->field_canGoOver(
					position.x/CELL_SIZE-1,position.y/CELL_SIZE+1)){
						centerCoordinate(position.x, +1);
						centerCoordinate(position.y, -1);
					}
			}else {
				if(position.y%CELL_SIZE+1<CELL_SIZE/2){
					if(!GAME->field_canGoOver(
					position.x/CELL_SIZE,position.y/CELL_SIZE-1)){
// 						centerCoordinate(position.x, +1);
						centerCoordinate(position.y, +1);
					}
				} else if(position.y%CELL_SIZE-1>CELL_SIZE/2)
					if(!GAME->field_canGoOver(
					position.x/CELL_SIZE,position.y/CELL_SIZE+1)){
// 						centerCoordinate(position.x, +1);
						centerCoordinate(position.y, -1);
					}
			} break;
		default: break;
	}
	// nastavit pozici prisery
	creature_->x_=position.x;
	creature_->y_=position.y;
	creature_->d_=position.d;
}

/** @details
 * Podle zadaného znaménka přidá k souřadnici nebo ubere od souřadnice
 * polovinu rychlosti nestvůry. Maximálně však do poloviny políčka.
 * @param coordinate centrovaná souřadnice
 * @param sign +1 nebo -1, přidá nebo ubere
 */
void AI::centerCoordinate(Uint16 & coordinate, Sint8 sign){
	Uint8 diff_center = sign*(CELL_SIZE/2-coordinate%CELL_SIZE);
	Uint8 diff_speed = (creature_->speed_diff_+1)/2;
	coordinate = coordinate + sign*(
		diff_center<diff_speed ? diff_center : diff_speed );
}

/** @details
 * Zjistí podle zadané pozice jestli může na políčko vstoupit..
 * @param position zjištovaná pozice nestvůry
 * @param isBlocking predikát na určení blokujícího typu
 * @return TRUE pokud se na políčku nevyskytuje žádný z objektů zadaných typů.
 */
bool AI::checkField(const position_t & position,
					const isTypeOf & isBlocking){
	Uint16 x=position.x/CELL_SIZE, y=position.y/CELL_SIZE;
	MapObject * blocking = GAME->field_getObject(x, y, isBlocking);
	if(blocking && blocking->type()!=BOMB_STAYING)
		return false;

	switch(position.d){
		case UP: if(position.y%CELL_SIZE<CELL_SIZE/2
				&& GAME->field_withObject(x, y-1, isBlocking) )
					return false;
			break;
		case RIGHT: if(position.x%CELL_SIZE>CELL_SIZE/2
				&& GAME->field_withObject(x+1, y, isBlocking) )
					return false;
			break;
		case DOWN: if(position.y%CELL_SIZE>CELL_SIZE/2
				&& GAME->field_withObject(x, y+1, isBlocking) )
					return false;
			break;
		case LEFT: if(position.x%CELL_SIZE<CELL_SIZE/2
				&& GAME->field_withObject(x-1, y, isBlocking) )
					return false;
			break;
		default: break;
	}
	return true;
}

/************************ AI_0 **************************/

AI_0::AI_0(Creature *creature):
		AI(creature), isBlocking_(isTypeOf::isWallBoxBomb) {}

void AI_0::move() {
	updatePositions();
	// vpred
	if(checkField(positions_[1], isBlocking_)){
		setPosition(positions_[1]);
		return;
	}
	// otoceni doprava
	if(rand()%3==0 && checkField(positions_[2], isBlocking_)){
		setPosition(positions_[2]);
		return;
	}
	// otoceni doleva
	if(rand()%2==0 && checkField(positions_[4], isBlocking_)){
		setPosition(positions_[4]);
		return;
	}
	// vzad
	if(checkField(positions_[3], isBlocking_))
		setPosition(positions_[3]);
}

/************************ AI_1 **************************/

AI_1::AI_1(Creature *creature):
		AI(creature), isBlocking_(isTypeOf::isWallBoxBomb) {}

void AI_1::move() {
	updatePositions();

	// vpred
	if(rand()%100<=97 && checkField(positions_[1], isBlocking_)){
		setPosition(positions_[1]);
		return;
	}
	// otoceni doprava
	if(rand()%5<=2 && checkField(positions_[2], isBlocking_)){
		setPosition(positions_[2]);
		return;
	}
	// otoceni doleva
	if(rand()%3<=2 && checkField(positions_[4], isBlocking_)){
		setPosition(positions_[4]);
		return;
	}
	// vzad
	if(checkField(positions_[3], isBlocking_))
		setPosition(positions_[3]);
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
