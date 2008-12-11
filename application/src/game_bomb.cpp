
#include "SDL_lib.h"
#include "stl_helper.h"
#include "constants.h"
#include "game.h"
#include "game_mapobjects.h"
#include "game_bomb.h"
#include "game_flame.h"
#include "game_presumption.h"


/**
 * @param anim animace
 * @param x souřadnice středu v pixelech
 * @param y souřadnice středu v pixelech
 * @param flamesize velikost plamene v políčkách
 * @param speed rychlost jedoucí bomby
 * @param timer odpaluje-li se rucne
 */
Bomb::Bomb(const Animation & anim, Uint16 x, Uint16 y,
					Uint16 flamesize, Uint16 speed, bool timer):
		DynamicMO(x, y),
		anim_(anim), explodes_(false), timer_(timer),
		access_counter_(0), flamesize_(flamesize),
		speed_diff_((speed-1)/7+1),
		speed_rate_((speed-1)%7+2+speed_diff_), d_(BURNED) {}

/** @details
 * Vyhodí zbylé presumpce.
 * @see remove_presumptions_()
 */
Bomb::~Bomb(){
	remove_presumptions_();
}

/** @details
 * Hýbe s bombou, stará se o presumpce (vyhazuje nepotřebné, vytváří nové).
 * @return Vrací TRUE pokud objekt explodoval.
 */
bool Bomb::move(){
	Uint16 accessed = ++access_counter_%speed_rate_;
	if(accessed!=0 && accessed!=speed_rate_/2){
		if(d_!=BURNED){
			// puvodni pozice
			Uint16 old_x=x_, old_y=y_, x=x_/CELL_SIZE, y=y_/CELL_SIZE;
			Game * game = Game::get_instance();
			switch(d_){
				case UP:
					if(y_%CELL_SIZE>CELL_SIZE/2
					|| game->field_canGoOver(x, y-1) )
						y_-=speed_diff_;
					break;
				case RIGHT:
					if(x_%CELL_SIZE<CELL_SIZE/2
					|| game->field_canGoOver(x+1, y) )
						x_+=speed_diff_;
					break;
				case DOWN:
					if(y_%CELL_SIZE<CELL_SIZE/2
					|| game->field_canGoOver(x, y+1) )
						y_+=speed_diff_;
					break;
				case LEFT:
					if(x_%CELL_SIZE>CELL_SIZE/2
					|| game->field_canGoOver(x-1, y) )
						x_-=speed_diff_;
					break;
				default: break;
			}

			if(old_x==x_ && old_y==y_)
				kick(BURNED);
			// nastavit novou pozici take v mape
			setFieldInMap(old_x, old_y);
			// vyhodit stare presumpce
			remove_presumptions_();
		}
		create_presumptions_();
	}

	return explodes_;
}

/** @details
 * Nastaví flag pro výbuch.
 * Vytvoří plameny s ohledem na okolí.
 * Ohlídá si, aby běžela pouze jednou,
 * protože nemá smysl vytvářet plameny dvakrát.
 */
void Bomb::explode(){
	// explode() muze bezet jen jednou
	if(explodes_) return;

	explodes_ = true;
	Game * game = Game::get_instance();
	Uint16 x = x_/CELL_SIZE,
		y = y_/CELL_SIZE;

	game->insert_object(x, y,
		game->tools->flame_cross(x, y) );

	Uint16 i, dir, next_x, next_y;
	Sint16 factor_x, factor_y;
	Flame* flame;
	MapObject * obj;
	// pres vsechny smery
	for(dir=0 ; dir<4 ; ++dir){
		factor_x = factor_y = 0;
		if(dir%2)
			factor_x = 2-dir;
		else
			factor_y = 1-dir;;
		// pro velikost plamene
		for(i=1 ; i<=flamesize_ ; ++i){
			// souradnice pro novy plamen
			next_x = x+ i*factor_x;
			next_y = y+ i*factor_y;
			if(next_x>=game->map_width()
			|| next_y>=game->map_height())
				break;
			// zajimavy objekt na policku
			obj = game->field_getObject(next_x, next_y,
						isTypeOf::isWallBoxAnyBomb);
			// zarazi se na zdi a konec
			if(obj && obj->type()==WALL)
				break;
			// priprava noveho plamene
			if(obj && obj->type()!=BOX){
				flame = game->tools->flame_cross(next_x, next_y);
			}
			else switch(dir){
				case 0: flame= i==flamesize_
					? game->tools->flame_bottom(next_x, next_y)
					: game->tools->flame_topbottom(next_x, next_y);
					break;
				case 2: flame= i==flamesize_
					? game->tools->flame_top(next_x, next_y)
					: game->tools->flame_topbottom(next_x, next_y);
					break;
				case 1: flame= i==flamesize_
					? game->tools->flame_right(next_x, next_y)
					: game->tools->flame_leftright(next_x, next_y);
					break;
				case 3: flame= i==flamesize_
					? game->tools->flame_left(next_x, next_y)
					: game->tools->flame_leftright(next_x, next_y);
					break;
			}
			// vlozeni plamene do hry
			game->insert_object(next_x, next_y, flame);
			// zarazi se za bednou nebo bombou
			// (obecne veci ktere maji bouchnout ale nemaji plamen)
			if(obj)
				break;
		}
	}
}

/** @details
 * Nastaví nový směr.
 * Vycentruje bombu na políčku podle nového směru.
 * Pokud je na dalším políčku ve směru pohybu bomba,
 * posune sebe na vzdálenější okraj poíčka.
 * @param d nový směr bomby
 */
void Bomb::kick(DIRECTION d){
	d_ = d;
	Uint16 x=x_/CELL_SIZE, y=y_/CELL_SIZE;
	MapObject * obj;
	switch(d){
		case UP:
		case DOWN:
			obj = Game::get_instance()->field_getObject(
					x, y+ (d==UP ? -1 : +1), isTypeOf::isWallBoxAnyBomb);
			x_ = x*CELL_SIZE+CELL_SIZE/2;
			if(obj){ // neco jsem tam nasel
				// hybajici se bomba => presunu se na konec policka
				if(obj->type()==BOMB_MOVING)
					y_ = (y+ (d==UP ? 1 : 0) )*CELL_SIZE;
				// neco blokujiciho => nehybu se
				else
					d_ = BURNED;
			}
			break;
		case RIGHT:
		case LEFT:
			obj = Game::get_instance()->field_getObject(
					x+ (d==LEFT ? -1 : +1), y, isTypeOf::isWallBoxAnyBomb);
			y_ = y*CELL_SIZE+CELL_SIZE/2;
			if(obj){ // neco jsem tam nasel
				// hybajici se bomba => presunu se na konec policka
				if(obj->type()==BOMB_MOVING)
					x_ = (x+ (d==LEFT ? 1 : 0) )*CELL_SIZE;
				// neco blokujiciho => nehybu se
				else
					d_ = BURNED;
			}
			break;
		default:
			x_ = x*CELL_SIZE+CELL_SIZE/2;
			y_ = y*CELL_SIZE+CELL_SIZE/2;
	}
	setFieldInMap(x*CELL_SIZE, y*CELL_SIZE);
	remove_presumptions_();
	create_presumptions_();
}

/**
 */
void Bomb::remove_timer(){
	timer_=false;
}

void Bomb::find_target_(Uint16 & x, Uint16 & y) const {
	x = x_/CELL_SIZE;
	y = y_/CELL_SIZE;
	if(d_==BURNED) return;

	Uint16 tar_x = x_, tar_y = y_, column = x, field = y,
		to_end, distance, bombs_before = 0;

	to_end = timer_ ? 0 : anim_.periods_to_end();
	distance = count_distance_(to_end);

	switch(d_){
		case UP:
			tar_y = tar_y<distance ? 0 : tar_y-distance;
			break;
		case RIGHT:
			tar_x += distance;
			break;
		case DOWN:
			tar_y += distance;
			break;
		case LEFT:
			tar_x = tar_x<distance ? 0 : tar_x-distance;
			break;
		default: break;
	}
	tar_x/=CELL_SIZE;
	tar_y/=CELL_SIZE;

	while(column != tar_x || field != tar_y){
		// posun o dalsi policko
		if(column!=tar_x)
			x<tar_x ? ++column : --column;
		else
			y<tar_y ? ++field : --field;

		// nalezeni prekazky
		MapObject * obj = GAME->field_getObject(column, field,
				isTypeOf::isWallBoxAnyBombFlame);
		if(obj) switch(obj->type()){
			case WALL:
			case BOX:
			case BOMB_STAYING:
				// zastavi se pred prekazkou
				column = tar_x; field = tar_y;
				continue;
			case BOMB_MOVING:
				// zvysi pocet predchozich bomb,
				// pokud jede stejnym smerem
				if(static_cast<Bomb*>(obj)->d_==d_)
					++bombs_before;
				break;
			case FLAME:
				// posun cile o dalsi policko
				x = column;
				y = field;
				// konec
				column = tar_x; field = tar_y;
				continue;
			default: break;
		}
		else {
			// posun cile o dalsi policko
			x = column;
			y = field;

			obj = GAME->field_getObject(column, field,
					isTypeOf::isPresumption);
			if(obj){
				// doba do bouchnuti nalezene predpovedi
				Uint16 another_to_end =
					static_cast<Presumption *>(obj)->periods_to_flame();
				// zastavi se na presumpci ktera bouchne driv nez ja
				// ale stihne bouchnout nez z ni ujedu,
				// a bez ohledu na predchozi bomby
				if(another_to_end < to_end
				&& count_distance_(another_to_end)/CELL_SIZE <
					abs_minus(static_cast<Uint16>(x_/CELL_SIZE), x)+
					abs_minus(static_cast<Uint16>(y_/CELL_SIZE), y) )
							return;
			}
		}
	}
	if(bombs_before) {
		tar_x = x/CELL_SIZE;
		tar_y = y/CELL_SIZE;
		if(abs_minus(x,tar_x)+abs_minus(y,tar_y)
					<= bombs_before){
			x = tar_x;
			y = tar_y;
		}
		else switch(d_){
			case UP: y+=bombs_before; break;
			case RIGHT: x-=bombs_before; break;
			case DOWN: y-=bombs_before; break;
			case LEFT: x+=bombs_before; break;
			default: break;
		}
	}
}

Uint16 Bomb::count_distance_(Uint16 to_end) const{
	return to_end * speed_diff_ * (speed_rate_-2) /speed_rate_;
}

/**
 */
void Bomb::create_presumptions_(){
	Uint16 i, dir, x, y;

	find_target_(x, y);
	// TODO debug
// 	std::cout << x << "," << y << std::endl;

	Sint16 factor_x, factor_y;

	// na svoje policko
	add_presumption_(x, y);
	// pres vsechny smery
	for(dir=0 ; dir<4 ; ++dir){
		factor_x = factor_y = 0;
		if(dir%2)
			factor_x = 2-dir;
		else
			factor_y = 1-dir;
		// pro velikost plamene
		for(i=1 ; i<=flamesize_ ; ++i){
			// pridam presumpci na nove souradnice
			if(!add_presumption_(x+ i*factor_x, y+ i*factor_y))
				break;
		}
	}
}

/** @details
 * Vytvoří presumpci, pokud již na políčku není,
 * vloží ji do mapy a do seznamu presumpcí bomby.
 * @return Vrací FALSE, pokud je plamen v tomto smeru zastaven.
 * Pokud je volno, vrací TRUE.
 */
bool Bomb::add_presumption_(Uint16 x, Uint16 y){
	Game * game = Game::get_instance();
	if(x>=game->map_width() || y>=game->map_height())
		return false;
	MapObject * blocking = game->field_getObject(x, y, isTypeOf::isWallBoxBomb);
	// zarazi se pred zdi nebo pred bednou
	if(blocking && blocking->type()!=BOMB_STAYING)
		return false;
	// na policku jeste neni presumpce
	if(!game->field_withObject(x, y, isTypeOf::isPresumption)){
		// priprava nove presumpce
		Presumption * presumption = game->tools->presumption(x, y,
						anim_.periods_to_end());
		// vlozeni presumpce do hry
		game->insert_object(x, y, presumption);
		// vlozeni do seznamu presumpci
		presumptions_.push_back(presumption);
	}
	// zarazi se za bombou
	return blocking==0;
}

/** @details
 * Vyhodí všechny presumpce bomby.
 * @see Game::remove_object()
 */
void Bomb::remove_presumptions_(){
	Game * game = Game::get_instance();
	for(Uint16 i=0 ; i<presumptions_.size() ; ++i){
		game->remove_object(presumptions_[i]);
	}
	presumptions_.clear();
}

/**
 * @param window surface okna pro vykreslení
 */
void Bomb::draw(SDL_Surface *window, const SDL_Rect & rect){
	anim_.draw(window,
		x_-anim_.width()/2 +rect.x, y_-anim_.height()/2 +rect.y);
}

/** @details
 * Posune frame animace, pokud není na ruční odpal.
 * Pokud animace dobehne nebo je na policku bomby plamen,
 * odpálí bombu.
 * @see explode()
 */
void Bomb::update(){
	if( ( !timer_ && anim_.update() )
	|| Game::get_instance()->field_withObject(
				x_/CELL_SIZE, y_/CELL_SIZE, isTypeOf::isFlame))
		explode();
}


/****** class MegaBomb ********/

MegaBomb::MegaBomb(const Animation & anim, Uint16 x, Uint16 y,
					Uint16 flamesize, Uint16 speed, bool timer):
	Bomb(anim, x, y, 2*flamesize, speed, timer){}



/** @details
 * Nastaví flag pro výbuch.
 * Vytvoří plameny s ohledem na okolí.
 * Ohlídá si, aby běžela pouze jednou,
 * protože nemá smysl vytvářet plameny dvakrát.
 */
void MegaBomb::explode(){

	if(explodes_) return;
	Game * game = Game::get_instance();
	Uint16 old_x = x_, old_y = y_, x, y;
	// vytvorit dlouhe plameny
	for(x_= old_x-CELL_SIZE ; x_<= old_x+CELL_SIZE ; x_+=CELL_SIZE){
		for(y_= old_y-CELL_SIZE ; y_<= old_y+CELL_SIZE ; y_+=CELL_SIZE){
			if(x_/CELL_SIZE >= Game::get_instance()->map_width()
			|| y_/CELL_SIZE >= Game::get_instance()->map_height())
				continue;
			MapObject* obj = game->field_getObject(
				x_/CELL_SIZE, y_/CELL_SIZE,
				isTypeOf::isWallBoxAnyBomb);
			if(!obj || obj==this){
				Bomb::explode();
				explodes_ = false;
			}
		}
	}
	explodes_ = true;
	x_ = old_x;
	y_ = old_y;
	old_x/=CELL_SIZE;
	old_y/=CELL_SIZE;
	// vlozit do stredu krize at to vypada hezky
	for(x = old_x-1 ; x<= old_x+1 ; ++x){
		for(y = old_y-1 ; y<= old_y+1 ; ++y) {
			if(x >= Game::get_instance()->map_width()
			|| y >= Game::get_instance()->map_height())
				continue;
			if(!game->field_withObject(x, y, isTypeOf::isWallBoxAnyBomb)){
				game->insert_object(x, y,
					game->tools->flame_cross(x, y) );
			}
		}
	}
}

/**
 */
void MegaBomb::create_presumptions_(){
	Uint16 old_x = x_, old_y = y_;

	for(x_= old_x-CELL_SIZE ; x_<= old_x+CELL_SIZE ; x_+=CELL_SIZE){
		for(y_= old_y-CELL_SIZE ; y_<= old_y+CELL_SIZE ; y_+=CELL_SIZE) {
			if(x_/CELL_SIZE >= Game::get_instance()->map_width()
			|| y_/CELL_SIZE >= Game::get_instance()->map_height())
				continue;
			if(!Game::get_instance()->field_withObject(
					x_/CELL_SIZE, y_/CELL_SIZE, isTypeOf::isWallBoxBomb))
				Bomb::create_presumptions_();
		}
	}
	x_ = old_x;
	y_ = old_y;
}
