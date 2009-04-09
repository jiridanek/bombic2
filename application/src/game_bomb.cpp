
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
			Uint16 old_x = x_, old_y = y_;
			// policko na kterem je bomba
			Uint16 field_x = x_/CELL_SIZE, field_y = y_/CELL_SIZE;
			switch(d_){
				case UP:
					if(y_ % CELL_SIZE > CELL_SIZE/2
					|| GAME->field_canGoOver(field_x, field_y-1) )
						y_ -= speed_diff_;
					break;
				case RIGHT:
					if(x_ % CELL_SIZE < CELL_SIZE/2
					|| GAME->field_canGoOver(field_x+1, field_y) )
						x_ += speed_diff_;
					break;
				case DOWN:
					if(y_ % CELL_SIZE < CELL_SIZE/2
					|| GAME->field_canGoOver(field_x, field_y+1) )
						y_ += speed_diff_;
					break;
				case LEFT:
					if(x_ % CELL_SIZE > CELL_SIZE/2
					|| GAME->field_canGoOver(field_x-1, field_y) )
						x_ -= speed_diff_;
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
	Uint16 x = x_/CELL_SIZE,
		y = y_/CELL_SIZE;

	GAME->insert_object(x, y,
		GAME->tools->flame_cross(x, y) );

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
			if(next_x>=GAME->map_width()
			|| next_y>=GAME->map_height())
				break;
			// zajimavy objekt na policku
			obj = GAME->field_getObject(next_x, next_y,
						isTypeOf::isWallBoxAnyBomb);
			// zarazi se na zdi a konec
			if(obj && obj->type()==WALL)
				break;
			// priprava noveho plamene
			if(obj && obj->type()!=BOX){
				flame = GAME->tools->flame_cross(next_x, next_y);
			}
			else switch(dir){
				case 0: flame= i==flamesize_
					? GAME->tools->flame_bottom(next_x, next_y)
					: GAME->tools->flame_topbottom(next_x, next_y);
					break;
				case 2: flame= i==flamesize_
					? GAME->tools->flame_top(next_x, next_y)
					: GAME->tools->flame_topbottom(next_x, next_y);
					break;
				case 1: flame= i==flamesize_
					? GAME->tools->flame_right(next_x, next_y)
					: GAME->tools->flame_leftright(next_x, next_y);
					break;
				case 3: flame= i==flamesize_
					? GAME->tools->flame_left(next_x, next_y)
					: GAME->tools->flame_leftright(next_x, next_y);
					break;
			}
			// vlozeni plamene do hry
			GAME->insert_object(next_x, next_y, flame);
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
 * posune sebe na vzdálenější okraj políčka.
 * @param d nový směr bomby
 */
void Bomb::kick(DIRECTION d){
	d_ = d;
	Uint16 x=x_/CELL_SIZE, y=y_/CELL_SIZE;
	MapObject * obj;
	switch(d){
		case UP:
		case DOWN:
			obj = GAME->field_getObject(
					x, y+ (d==UP ? -1 : +1),
					isTypeOf::isWallBoxAnyBomb);
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
			obj = GAME->field_getObject(
					x+ (d==LEFT ? -1 : +1), y,
					isTypeOf::isWallBoxAnyBomb);
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

void Bomb::remove_timer(){
	timer_ = false;
}

/** @details
 * Z cílověho políčka vytvoří ve směrech výbuchu presumpce.
 * @see find_target_field_()
 */
void Bomb::create_presumptions_(){

	target_field_ = find_target_field_();
	// TODO debug
// 	std::cout << x << "," << y << std::endl;

	// na svoje policko
	add_presumption_(target_field_);
	// pres vsechny smery
	for(Uint16 dir=0 ; dir<4 ; ++dir){
		Sint16 factor_x = 0, factor_y = 0;
		if(dir%2)
			factor_x = 2-dir;
		else
			factor_y = 1-dir;
		// pro velikost plamene
		for(Uint16 i=1 ; i<=flamesize_ ; ++i){
			// pridam presumpci na nove souradnice
			field_t new_field = target_field_;
			new_field.first +=  i*factor_x;
			new_field.second += i*factor_y;
			if(!add_presumption_(new_field)){
				break;
			}
		}
	}
}

/** @details
 * @param x
 * @param y
 */
field_t Bomb::find_target_field_() const {
	// nastavi startovni pozici
	field_t start_field(x_/CELL_SIZE, y_/CELL_SIZE);
	// kdyz se nehybe vraci startovni pozici
	if(d_==BURNED) return start_field;

	Uint16 periods_to_end = timer_ ?
		0 : anim_.periods_to_end();
	Uint16 distance_in_fields =
		count_distance_(periods_to_end)/CELL_SIZE;

	field_t curr_field = start_field;
	for(Uint16 i = 0 ; i < distance_in_fields ; ++i){
		field_t next_field = curr_field;
		switch(d_){
			case UP: --next_field.second;
				break;
			case RIGHT: ++next_field.first;
				break;
			case DOWN: ++next_field.second;
				break;
			case LEFT: --next_field.first;
				break;
			default: break;
		}


		// !! NELZE vyhledavat presumpce zaroven s bombami

		// nalezeni prekazky
		MapObject * obj = GAME->field_getObject(next_field,
			isTypeOf::isWallBoxAnyBombFlame);
		if(obj==0){
			// podivam se jeste po presumpcich
			if(is_danger_presumption(next_field, periods_to_end)){

				return next_field;
			} else {
				// zadny zajimavy objekt, podivam se dal
				curr_field = next_field;
				continue;
			}
		}

		switch(obj->type()){
			case WALL:
			case BOX:
			case BOMB_STAYING:
				return curr_field;
			case FLAME:
				return next_field;
			case BOMB_MOVING:
				// pokud jede stejnym smerem
				// prevezme od ni cil a posune ho proti smeru
				Bomb * bomb_before =
					static_cast<Bomb*>(obj);
				if(bomb_before->d_==d_){
					return subtract_from_bomb_before(bomb_before);
				} else {
					curr_field = next_field;
					continue;
				}
			default:
				continue;
		}
	}

	return curr_field;
}

/**
 * @param time_to_end počet period do výbuchu bomby
 * @return Vzdálenost, kterou je bomba schopna maximálně ujet, než vybuchne.
 */
Uint16 Bomb::count_distance_(Uint16 periods_to_end) const{
	return periods_to_end * speed_diff_ * (speed_rate_-2) /speed_rate_;
}

/** @details
 * Pokusí se najít presumpci,
 * porovnává počet period do výbuchu presumpce a mě,
 * pokud by mohla presumpce bouchnout dříve než já,
 * testuje, jestli nebudu už za tímto výbuchem.
 * @param field policko, ktere testujeme
 * @param periods_to_end počet period do mého bouchnutí
 * @return Vrací TRUE pokud je na policku presumpce,
 * ktera bouchne (vytvoří se místo ní plamen) dříve, než stačím odjet,
 * pokud tam presumpce není nebo nesplnuje podmínku výše, vrací FALSE.
 */
bool Bomb::is_danger_presumption(const field_t & field,
					Uint16 periods_to_end) const{
	Presumption * presumption = static_cast<Presumption *>(
		GAME->field_getObject(field, isTypeOf::isPresumption));

	// nenalezena presumpce neni nebezpecna
	if(presumption==0){
		return false;
	}
	// zastavi se na presumpci ktera bouchne driv nez ja
	// ale stihne bouchnout nez z ni ujedu,
	// a bez ohledu na predchozi bomby

	// doba do bouchnuti nalezene predpovedi
	Uint16 presumption_to_end =
		presumption->periods_to_flame();
	if(presumption_to_end < periods_to_end){
		// pocet policek, ktera stihnu ujet nez presumpce bouchne
		Uint16 fields_to_presumption_end=
			count_distance_(presumption_to_end)/CELL_SIZE;
		// pocet policek, na policko presumpce
		Uint16 fields_to_presumption =
			abs_minus(Uint16(x_/CELL_SIZE), field.first)+
			abs_minus(Uint16(y_/CELL_SIZE), field.second);
		// nestihnu odjet z policka presumpce nez bouchne
		if(fields_to_presumption_end <= fields_to_presumption) {
			return true;
		} else {
			return false;
		}
	} else {
		return false;
	}
}

/** @details
 * Převezme od předchozí bomby cíl a
 * posune ho o jedna proti směru pohybu.
 * @param bomb_before předcházející bomba
 * @return Vrací svůj cíl.
 */
field_t Bomb::subtract_from_bomb_before(Bomb * bomb_before) const{
	field_t field = bomb_before->target_field_;
	switch(d_){
		case UP:
			++field.second;
			break;
		case RIGHT:
			--field.first;
			break;
		case DOWN:
			--field.second;
			break;
		case LEFT:
			++field.first;
			break;
		default:
			break;
	}
	return field;
}

/** @details
 * Vytvoří presumpci, pokud již na políčku není,
 * vloží ji do mapy a do seznamu presumpcí bomby.
 * @param field políčko na které chceme vkládat
 * @return Vrací FALSE, pokud je presumpce v tomto smeru zastavena.
 * Pokud je volno, vrací TRUE.
 */
bool Bomb::add_presumption_(const field_t & field){
	if(field.first >= GAME->map_width()
	|| field.second >= GAME->map_height()){
		return false;
	}
	MapObject * blocking =
		GAME->field_getObject(
			field.first, field.second, isTypeOf::isWallBoxBomb);
	// zarazi se pred zdi nebo pred bednou
	if(blocking!=0 && blocking->type()!=BOMB_STAYING)
		return false;
	// na policku jeste neni presumpce
	bool fieldWithPresumption =
		GAME->field_withObject(
			field.first, field.second,
			isTypeOf::isPresumption);
	if(!fieldWithPresumption){
		// priprava nove presumpce
		Presumption * presumption =
			GAME->tools->presumption(
				field.first, field.second,
				anim_.periods_to_end());
		// vlozeni presumpce do hry
		GAME->insert_object(field.first, field.second, presumption);
		// vlozeni do seznamu presumpci
		presumptions_.push_back(presumption);
	}
	// zarazi se za bombou
	return blocking==0 || blocking->type()==BOMB_STAYING;
}

/** @details
 * Vyhodí všechny presumpce bomby.
 * @see Game::remove_object()
 */
void Bomb::remove_presumptions_(){
	for(Uint16 i=0 ; i<presumptions_.size() ; ++i){
		GAME->remove_object(presumptions_[i]);
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
	|| GAME->field_withObject(
				x_/CELL_SIZE, y_/CELL_SIZE, isTypeOf::isFlame))
		explode();
}


/****** class MegaBomb ********/

MegaBomb::MegaBomb(const Animation & anim, Uint16 x, Uint16 y,
					Uint16 flamesize, Uint16 speed, bool timer):
	Bomb(anim, x, y, flamesize, speed, timer){}



/** @details
 * Nastaví flag pro výbuch.
 * Vytvoří plameny s ohledem na okolí.
 * Ohlídá si, aby běžela pouze jednou,
 * protože nemá smysl vytvářet plameny dvakrát.
 */
void MegaBomb::explode(){

	if(explodes_) return;
	Uint16 old_x = x_, old_y = y_, x, y;
	// vytvorit dlouhe plameny
	for(x_= old_x-CELL_SIZE ; x_<= old_x+CELL_SIZE ; x_+=CELL_SIZE){
		for(y_= old_y-CELL_SIZE ; y_<= old_y+CELL_SIZE ; y_+=CELL_SIZE){
			if(x_/CELL_SIZE >= GAME->map_width()
			|| y_/CELL_SIZE >= GAME->map_height())
				continue;
			MapObject* obj = GAME->field_getObject(
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
			if(x >= GAME->map_width()
			|| y >= GAME->map_height())
				continue;
			if(!GAME->field_withObject(x, y, isTypeOf::isWallBoxAnyBomb)){
				GAME->insert_object(x, y,
					GAME->tools->flame_cross(x, y) );
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
			if(x_/CELL_SIZE >= GAME->map_width()
			|| y_/CELL_SIZE >= GAME->map_height())
				continue;
			if(!GAME->field_withObject(
					x_/CELL_SIZE, y_/CELL_SIZE, isTypeOf::isWallBoxBomb))
				Bomb::create_presumptions_();
		}
	}
	x_ = old_x;
	y_ = old_y;
}
