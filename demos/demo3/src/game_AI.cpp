
#include <iostream>
#include <vector>
#include "SDL_lib.h"
#include "constants.h"
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

/** @details TODO
 * Statická fce, pouze rozhodne, která třída AI se použije.
 * @param creature rodičovská nestvůra
 * @param intelligence výše umělé inteligence
 * @return Pointer na vytvořenou umělou inteligenci,
 *	nebo nula pokud výše umělé inteligence neexistuje.
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
	Game * game = Game::get_instance();
	// prvne vycentrovat
	switch(position.d){
		case UP:
			if(position.y%CELL_SIZE+1<CELL_SIZE/2){
				// vlevo
				if(position.x%CELL_SIZE+1<CELL_SIZE/2){
					if(!game->field_canGoOver(
					position.x/CELL_SIZE-1,position.y/CELL_SIZE-1)){
						centerCoordinate(position.x, +1);
						centerCoordinate(position.y, +1);
					}
				// vpravo
				} else if(position.x%CELL_SIZE-1>CELL_SIZE/2)
					if(!game->field_canGoOver(
					position.x/CELL_SIZE+1,position.y/CELL_SIZE-1)){
						centerCoordinate(position.x, -1);
						centerCoordinate(position.y, +1);
					}
			} else {
				if(position.x%CELL_SIZE+1<CELL_SIZE/2){
					if(!game->field_canGoOver(
					position.x/CELL_SIZE-1,position.y/CELL_SIZE)){
						centerCoordinate(position.x, +1);
// 						centerCoordinate(position.y, +1);
					}
				} else if(position.x%CELL_SIZE-1>CELL_SIZE/2)
					if(!game->field_canGoOver(
					position.x/CELL_SIZE+1,position.y/CELL_SIZE)){
						centerCoordinate(position.x, -1);
// 						centerCoordinate(position.y, +1);
					}
			} break;
		case RIGHT:
			if(position.x%CELL_SIZE-1>CELL_SIZE/2){
				// nahore
				if(position.y%CELL_SIZE+1<CELL_SIZE/2){
					if(!game->field_canGoOver(
					position.x/CELL_SIZE+1,position.y/CELL_SIZE-1)){
						centerCoordinate(position.x, -1);
						centerCoordinate(position.y, +1);
					}
				// dole
				} else if(position.y%CELL_SIZE-1>CELL_SIZE/2)
					if(!game->field_canGoOver(
					position.x/CELL_SIZE+1,position.y/CELL_SIZE+1)){
						centerCoordinate(position.x, -1);
						centerCoordinate(position.y, -1);
					}
			} else {
				if(position.y%CELL_SIZE+1<CELL_SIZE/2){
					if(!game->field_canGoOver(
					position.x/CELL_SIZE,position.y/CELL_SIZE-1)){
// 						centerCoordinate(position.x, -1);
						centerCoordinate(position.y, +1);
					}
				} else if(position.y%CELL_SIZE-1>CELL_SIZE/2)
					if(!game->field_canGoOver(
					position.x/CELL_SIZE,position.y/CELL_SIZE+1)){
// 						centerCoordinate(position.x, -1);
						centerCoordinate(position.y, -1);
					}

			} break;
		case DOWN:
			if(position.y%CELL_SIZE-1>CELL_SIZE/2){
				if(position.x%CELL_SIZE+1<CELL_SIZE/2){
					if(!game->field_canGoOver(
					position.x/CELL_SIZE-1,position.y/CELL_SIZE+1)){
						centerCoordinate(position.x, +1);
						centerCoordinate(position.y, -1);
					}
				} else if(position.x%CELL_SIZE-1>CELL_SIZE/2)
					if(!game->field_canGoOver(
					position.x/CELL_SIZE+1,position.y/CELL_SIZE+1)){
						centerCoordinate(position.x, -1);
						centerCoordinate(position.y, -1);
					}
			} else {
				if(position.x%CELL_SIZE+1<CELL_SIZE/2){
					if(!game->field_canGoOver(
					position.x/CELL_SIZE-1,position.y/CELL_SIZE)){
						centerCoordinate(position.x, +1);
// 						centerCoordinate(position.y, -1);
					}
				} else if(position.x%CELL_SIZE-1>CELL_SIZE/2)
					if(!game->field_canGoOver(
					position.x/CELL_SIZE+1,position.y/CELL_SIZE)){
						centerCoordinate(position.x, -1);
// 						centerCoordinate(position.y, -1);
					}
			} break;
		case LEFT:
			if(position.x%CELL_SIZE+1<CELL_SIZE/2){
				if(position.y%CELL_SIZE+1<CELL_SIZE/2){
					if(!game->field_canGoOver(
					position.x/CELL_SIZE-1,position.y/CELL_SIZE-1)){
						centerCoordinate(position.x, +1);
						centerCoordinate(position.y, +1);
					}
				} else if(position.y%CELL_SIZE-1>CELL_SIZE/2)
					if(!game->field_canGoOver(
					position.x/CELL_SIZE-1,position.y/CELL_SIZE+1)){
						centerCoordinate(position.x, +1);
						centerCoordinate(position.y, -1);
					}
			}else {
				if(position.y%CELL_SIZE+1<CELL_SIZE/2){
					if(!game->field_canGoOver(
					position.x/CELL_SIZE,position.y/CELL_SIZE-1)){
// 						centerCoordinate(position.x, +1);
						centerCoordinate(position.y, +1);
					}
				} else if(position.y%CELL_SIZE-1>CELL_SIZE/2)
					if(!game->field_canGoOver(
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
 * @param isBlocked predikát na určení blokujícího typu
 * @return TRUE pokud se na políčku nevyskytuje žádný z objektů zadaných typů.
 */
bool AI::checkField(const position_t & position,
					const isTypeOf & isBlocked){
	Game * game = Game::get_instance();
	Uint16 x=position.x/CELL_SIZE, y=position.y/CELL_SIZE;
	if(game->field_withObject(x, y, isBlocked))
		return false;

	switch(position.d){
		case UP: if(game->field_withObject(x, y-1, isBlocked)
				&& position.y%CELL_SIZE<CELL_SIZE/2)
					return false;
			break;
		case RIGHT: if(game->field_withObject(x+1, y, isBlocked)
				&& position.x%CELL_SIZE>CELL_SIZE/2)
					return false;
			break;
		case DOWN: if(game->field_withObject(x, y+1, isBlocked)
				&& position.y%CELL_SIZE>CELL_SIZE/2)
					return false;
			break;
		case LEFT: if(game->field_withObject(x-1, y, isBlocked)
				&& position.x%CELL_SIZE<CELL_SIZE/2)
					return false;
			break;
		default: break;
	}
	return true;
}

/************************ AI_0 **************************/

AI_0::AI_0(Creature *creature): AI(creature) {
	isBlocked_.addType(WALL).addType(BOX).addType(BOMB);
}

void AI_0::move() {
	updatePositions();
	// vpred
	if(checkField(positions_[1], isBlocked_)){
		setPosition(positions_[1]);
		return;
	}
	// otoceni doprava
	if(rand()%3==0 && checkField(positions_[2], isBlocked_)){
		setPosition(positions_[2]);
		return;
	}
	// otoceni doleva
	if(rand()%2==0 && checkField(positions_[4], isBlocked_)){
		setPosition(positions_[4]);
		return;
	}
	// vzad
	if(checkField(positions_[3], isBlocked_))
		setPosition(positions_[3]);
}

/************************ AI_1 **************************/

AI_1::AI_1(Creature *creature): AI(creature) {
	isBlocked_.addType(WALL).addType(BOX).addType(BOMB);
}

void AI_1::move() {
	updatePositions();

	// vpred
	if(rand()%100<=97 && checkField(positions_[1], isBlocked_)){
		setPosition(positions_[1]);
		return;
	}
	// otoceni doprava
	if(rand()%5<=2 && checkField(positions_[2], isBlocked_)){
		setPosition(positions_[2]);
		return;
	}
	// otoceni doleva
	if(rand()%3<=2 && checkField(positions_[4], isBlocked_)){
		setPosition(positions_[4]);
		return;
	}
	// vzad
	if(checkField(positions_[3], isBlocked_))
		setPosition(positions_[3]);
}

/************************ AI_10 **************************/

AI_10::AI_10(Creature *creature):
			AI(creature), old_x_(0), old_y_(0) {
	isBlocked_.addType(WALL).addType(BOX).addType(BOMB);
	isBad_.addType(WALL).addType(BOX).addType(BOMB)
		.addType(FLAME).addType(PRESUMPTION);

}

void AI_10::move() {
	updatePositions();

	if(old_x_ != positions_[0].x/CELL_SIZE
	|| old_y_ != positions_[0].y/CELL_SIZE){
		old_x_ = positions_[0].x/CELL_SIZE;
		old_y_ = positions_[0].y/CELL_SIZE;
		update_trace_array_();
	}
	position_t & position = get_position_(target_x_, target_y_);
	if(!checkField(position, isBlocked_))
		position = get_random_position_();
	setPosition(position);
}

void AI_10::update_trace_array_(){
	// vytvoreni trasovaciho pole s hodnotami -1
	if(empty_trace_array_.empty()){
		Game * game = Game::get_instance();

		vector< Sint16 > empty_column;
		empty_column.insert(empty_column.end(), game->map_height(), -1);
		empty_trace_array_.insert(
			empty_trace_array_.end(), game->map_width(), empty_column);
	}

	// zkopirovat prazdne pole
	trace_array_ = empty_trace_array_;
	trace_array_[old_x_][old_y_] = 0;
	// pripravit pocatecni policko do fronty
	fields_queue_t fields_queue;
	fields_queue.push(make_pair(old_x_, old_y_));
	// ohodnotit trasovaci pole
	eval_trace_array_(fields_queue);
}

void AI_10::eval_trace_array_(fields_queue_t & fields_queue){
	Game * game = Game::get_instance();

	if(fields_queue.empty())
		return;
	// obnovit souradnice cile
	target_x_ = fields_queue.front().first,
	target_y_ = fields_queue.front().second;
	// mame hrace, koncime a jdeme na nej
	if(game->field_withObject(target_x_, target_y_, PLAYER))
		return;
	// pripravim novou hodnotu policka
	Sint16 val = trace_array_[target_x_][target_y_]+1;

	Uint16 i, dir, next_x, next_y;
	// pres vsechny smery
	if(val <= AI_10_MAX_TRACE){
		// trocha nahody
		for(i=0, dir=rand()%4 ; i<4 ; ++i, ++dir){
			// souradnice
			next_x = target_x_; next_y = target_y_;
			if(dir%2)
				next_x += 2-dir%4;
			else
				next_y += 1-dir%4;
			// mohu na policko vstoupit TODO plameny
			if(trace_array_[next_x][next_y]==-1
			&& !game->field_withObject(next_x, next_y, isBad_)){
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
	eval_trace_array_(fields_queue);
}

AI::position_t & AI_10::get_position_(Uint16 x, Uint16 y){
	Sint16 val = trace_array_[x][y]-1;
	// konec rekurze
	if(val<=0){
		// nahoru
		if(y < old_y_)
			return positions_[ (4+UP-positions_[0].d)%4+1 ];
		// dolu
		if(y > old_y_)
			return positions_[ (4+DOWN-positions_[0].d)%4+1 ];
		// vlevo
		if(x < old_x_)
			return positions_[ (4+LEFT-positions_[0].d)%4+1 ];
		// vpravo
		if(x > old_x_)
			return positions_[ (4+RIGHT-positions_[0].d)%4+1 ];
	}
	Uint16 dir, next_x, next_y;
	// pres vsechny smery najdu odkud jsem se sem dostal
	for(dir=0 ; dir<4 ; ++dir){
		// souradnice
		next_x = x; next_y = y;
		if(dir%2)
			next_x += 2-dir;
		else
			next_y += 1-dir;
		if(trace_array_[next_x][next_y]==val)
			break;
	}
	// vratim rekurzi naslou pozici
	return get_position_(next_x, next_y);
}

AI::position_t & AI_10::get_random_position_(){
	// vpred
	if(rand()%100<=97 && checkField(positions_[1], isBlocked_))
		return positions_[1];
	// otoceni doprava
	if(rand()%5<=2 && checkField(positions_[2], isBlocked_))
		return positions_[2];
	// otoceni doleva
	if(rand()%3<=2 && checkField(positions_[4], isBlocked_))
		return positions_[4];
	// vzad
	if(checkField(positions_[3], isBlocked_))
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
	Uint16 x,y;

	if(keystate_[player->key_up_]){
		player->d_ = UP;
		x=positions_[1].x/CELL_SIZE; y=positions_[1].y/CELL_SIZE;
		if(Game::get_instance()->field_canGoOver(x, y, false)){
			if(!Game::get_instance()->field_canGoOver(x, y-1, true)
			&& positions_[1].y%CELL_SIZE<CELL_SIZE/2){
				positions_[1].y =y*CELL_SIZE+CELL_SIZE/2;
			}
			if(positions_[1].y<positions_[0].y)
				setPosition(positions_[1]);
		}
	}
	else if(keystate_[player->key_right_]){
		player->d_ = RIGHT;
		x=positions_[2].x/CELL_SIZE; y=positions_[2].y/CELL_SIZE;
		if(Game::get_instance()->field_canGoOver(x, y, false)){
			if(!Game::get_instance()->field_canGoOver(x+1, y, true)
			&& positions_[2].x%CELL_SIZE>CELL_SIZE/2)
				positions_[2].x =x*CELL_SIZE+CELL_SIZE/2;
			if(positions_[2].x>positions_[0].x)
				setPosition(positions_[2]);
		}
	}
	else if(keystate_[player->key_down_]){
		player->d_ = DOWN;
		x=positions_[3].x/CELL_SIZE; y=positions_[3].y/CELL_SIZE;
		if(Game::get_instance()->field_canGoOver(x, y, false)){
			if(!Game::get_instance()->field_canGoOver(x, y+1, true)
			&& positions_[3].y%CELL_SIZE>CELL_SIZE/2)
				positions_[3].y =y*CELL_SIZE+CELL_SIZE/2;
			if(positions_[3].y>positions_[0].y)
				setPosition(positions_[3]);
		}
	}
	else if(keystate_[player->key_left_]){
		player->d_ = LEFT;
		x=positions_[4].x/CELL_SIZE; y=positions_[4].y/CELL_SIZE;
		if(Game::get_instance()->field_canGoOver(x, y, false)){
			if(!Game::get_instance()->field_canGoOver(x-1, y, true)
			&& positions_[4].x%CELL_SIZE<CELL_SIZE/2)
				positions_[4].x =x*CELL_SIZE+CELL_SIZE/2;
			if(positions_[4].x<positions_[0].x)
				setPosition(positions_[4]);
		}
	}
	else player->d_ = cur_d;

	if(keystate_[player->key_plant_]){
		Game * game = Game::get_instance();
		x = player->x_/CELL_SIZE;
		y = player->y_/CELL_SIZE;
		if(game->count_bombs(player->player_num())< player->bombs_
		&& game->field_canGoOver(x, y, true))
			game->plant_bomb(player->player_num(), x, y,
				game->tools->bomb_normal(x, y, player->flamesize_) );
	}

	if(keystate_[player->key_timer_] && player->next_timer_ < MOVE_PERIOD){
		Game::get_instance()->explode_bomb(player->player_num());
		player->next_timer_ = TIMER_PERIOD;
	}

	// TODO timer
}
