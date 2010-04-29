
#include <string>
#include <helpers/sdl.h>
#include <helpers/stl.h>
#include <constants.h>
#include "game.h"
#include "tools.h"
#include "mapobjects.h"
#include "bomb.h"
#include "flame.h"
#include "presumption.h"

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
		periods_to_flame_(timer_ ? 0 : anim_.periods_to_end()),
		access_counter_(0), flamesize_(flamesize),
		speed_diff_((speed-1)/7+1),
		speed_rate_((speed-1)%7+2+speed_diff_), d_(BURNED) {
	update_presumptions_();
}

/** @details
 * Vyhodí zbylé presumpce.
 * @see remove_presumptions_()
 */
Bomb::~Bomb(){
	remove_presumptions_();
}

/** @details
 * Sníží počet period do výbuchu.
 * Hýbe s bombou, stará se o presumpce (vyhazuje nepotřebné, vytváří nové).
 * @return Vrací TRUE pokud objekt explodoval.
 */
bool Bomb::move(){
	Uint16 accessed = ++access_counter_%speed_rate_;
	if(accessed!=0 && accessed!=speed_rate_/2){
		// obnovit pocet period do vybuchu
		update_periods_to_flame_();
		// kdyz se ma hybat hybnu s ni
		if(d_!=BURNED){
			// puvodni pozice
			Uint16 old_x = x_, old_y = y_;
			if(move_by_direction_()){
				// nastavit novou pozici take v mape
				setFieldInMap(old_x, old_y);
				remove_presumptions_();
			} else {
				// nepodarilo se pohnout bombou,
				// zastavim ji
				kick(BURNED);
			}
		}
		update_presumptions_();
	}

	return explodes_;
}

/** @details
 * Podle presumpce, která leží pod bombou (pokud tam leží)
 * si nastaví svůj počet period do konce.
 * @see Presumption::move()
 */
void Bomb::update_periods_to_flame_(){
	Presumption * presumption = static_cast<Presumption *>(
		GAME->field_getObject(target_field_, isTypeOf::isPresumption));
	if(presumption){
		periods_to_flame_ = presumption->periods_to_flame_;
	}
}

/** @details
 * @return Vrací TRUE pokud se podařilo nastavit novou polohu.
 * Jinak FALSE (typicky, když se bomba o něc zarazí).
 */
bool Bomb::move_by_direction_(){
	// bomba je na policku za pulkou
	bool isTooFar = false;
	// nasledujici policko
	field_t following_field(x_/CELL_SIZE, y_/CELL_SIZE);
	switch(d_){
		case UP:
			isTooFar = y_ % CELL_SIZE <= CELL_SIZE/2;
			--following_field.second;
			break;
		case RIGHT:
			isTooFar = x_ % CELL_SIZE >= CELL_SIZE/2;
			++following_field.first;
			break;
		case DOWN:
			isTooFar = y_ % CELL_SIZE >= CELL_SIZE/2;
			++following_field.second;
			break;
		case LEFT:
			isTooFar = x_ % CELL_SIZE <= CELL_SIZE/2;
			--following_field.first;
			break;
		default: break;
	}
	// mohu jeste bombu posunout
	if(!isTooFar || GAME->field_canGoOver(following_field)){
		switch(d_){
			case UP:
				y_ -= speed_diff_;
				break;
			case RIGHT:
				x_ += speed_diff_;
				break;
			case DOWN:
				y_ += speed_diff_;
				break;
			case LEFT:
				x_ -= speed_diff_;
				break;
			default: break;
		}
		return true;
	} else {
		return false;
	}
}

/** @details
 * Výbuch bomby.
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
	Flame* flame = 0;
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
	// nenecham do sebe kopat ve stejnem smeru,
	// v jakem se pohybuji
	if(d == d_){
		return;
	}
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
			// zastavuji bombu
			x_ = x*CELL_SIZE+CELL_SIZE/2;
			y_ = y*CELL_SIZE+CELL_SIZE/2;
	}
	// VZDY nastavit nove policko v mape
	setFieldInMap(0, 0);
	// vytvorit NOVE presumpce
	remove_presumptions_();
	update_presumptions_();
}


/** @details
 * Odstraní bombě ruční odpalování,
 * spustí klasické odpočítávání.
 */
void Bomb::remove_timer(){
	timer_ = false;
	periods_to_flame_ = anim_.periods_to_end();
	remove_presumptions_();
	update_presumptions_();
}

/** @details
 * Z cílověho políčka vytvoří ve směrech výbuchu presumpce.
 * @see find_target_field_()
 */
void Bomb::update_presumptions_(){

	target_field_ = find_target_field_();

	// pres vsechny smery
	for(Uint16 dir=0 ; dir<4 ; ++dir){
		Sint16 factor_x = 0, factor_y = 0;
		if(dir%2) {
			factor_x = 2-dir;
		} else {
			factor_y = 1-dir;
		}
		update_presumptions_from_field_(
			target_field_, factor_x, factor_y);
	}
}

void Bomb::update_presumptions_from_field_(const field_t & start_field,
				Sint16 factor_x, Sint16 factor_y){
	field_t field(start_field);
	// pro velikost plamene
	for(Uint16 i=0 ; i<=flamesize_ ; ++i){
		// pridam presumpci na nove souradnice
		if(!update_presumption_(field)){
			break;
		}
		field.first += factor_x;
		field.second += factor_y;
	}
}

field_t Bomb::find_target_field_() const {
	// nastavi startovni pozici
	field_t curr_field(x_/CELL_SIZE, y_/CELL_SIZE);
	// kdyz se nehybe vraci startovni pozici
	if(d_==BURNED) return curr_field;

	// pocet period do vybuchu
	Uint16 periods_to_end = timer_ ?
		0 : anim_.periods_to_end();
	// pocet policek ktere stihne do konce ujet
	Uint16 distance_in_fields =
		count_distance_(periods_to_end)/CELL_SIZE;

	for(Uint16 i = 0 ; i < distance_in_fields ; ++i){
		field_t next_field = curr_field;
		move_field_by_direction_(next_field, +1);

		// !! NELZE vyhledavat presumpce zaroven s bombami

		// nalezeni prekazky
		MapObject * obj = GAME->field_getObject(next_field,
			isTypeOf::isWallBoxAnyBombFlame);
		// nic nenalezeno
		if(obj==0){
			// podivam se jeste po presumpcich
			if(is_danger_presumption_(next_field, periods_to_end)){
				// az tam dojedu tak tam bude plamen
				// bouchnu tam taky
				return next_field;
			} else {
				// zadny zajimavy objekt, pokracuji dal
				curr_field = next_field;
				continue;
			}
		}

		switch(obj->type()){
			// na dalsi policko nesmim
			case WALL:
			case BOX:
			case BOMB_STAYING:
				// vracim aktualni
				return curr_field;
			// na dalsim policku bouchnu
			case FLAME:
				// vracim tedy dalsi
				return next_field;
			// na dalsim policku je pohybujici se bomba
			case BOMB_MOVING:
				{
					// pokud jede stejnym smerem
					// prevezme od ni cil
					Bomb * bomb_before =
						static_cast<Bomb*>(obj);
					if(bomb_before->d_==d_){
						return take_target_from_bomb_before_(
							bomb_before);
					} else {
						// nejede stejnym smerem, nezajima me
						curr_field = next_field;
						continue;
					}
				}
			default:
				throw std::string("Unhandled object type.");
		}
	}

	return curr_field;
}

/**
 * @param periods_to_end počet period do výbuchu bomby
 * @return Vzdálenost, kterou je bomba schopna maximálně ujet, než vybuchne.
 */
Uint16 Bomb::count_distance_(Uint16 periods_to_end) const{
	return periods_to_end * speed_diff_ * (speed_rate_-2) /speed_rate_;
}

/** @details
 * Pro delta rovno +1 hýbne políčkem o jedna ve směru bomby.
 * @param field políčko, které chceme posunout
 * @param delta o kolik se má políčko posunout (pro kladné ve směru pohybu)
 */
void Bomb::move_field_by_direction_(field_t & field, Sint16 delta) const{
	switch(d_){
		case UP:
			field.second -= delta;
			break;
		case RIGHT:
			field.first += delta;
			break;
		case DOWN:
			field.second += delta;
			break;
		case LEFT:
			field.first -= delta;
			break;
		default: break;
	}
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
bool Bomb::is_danger_presumption_(const field_t & field,
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
		presumption->periods_to_flame_;
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
 * @note bomb_b je bomb_before
 * @param bomb_b předcházející bomba
 * @return Vrací svůj cíl.
 */
field_t Bomb::take_target_from_bomb_before_(Bomb * bomb_b) const{
	// zjistim, jak daleko je bomb_before
	Uint16 distance_from_bomb_b=
		abs_minus(x_/CELL_SIZE, bomb_b->x_/CELL_SIZE) +
		abs_minus(y_/CELL_SIZE, bomb_b->y_/CELL_SIZE);
	// zaciname na policku, ktere je cilem predchazejici bomby
	field_t field = bomb_b->target_field_;
	if(distance_from_bomb_b<= 1){
		// je hned predemnou, muj target je hned za jejim
		move_field_by_direction_(field, -1);
	} else {
		// pocet policek od cile predchazejici bomby
		Uint16 distance_from_bomb_b_target =
			abs_minus(x_/CELL_SIZE, int(field.first)) +
			abs_minus(y_/CELL_SIZE, int(field.second));
		// pocet policek, ktere ujedu nez bouchne predchazejici bomba
		Uint16 distance_to_bomb_b_flame =
			count_distance_(bomb_b->periods_to_flame_)
			 / CELL_SIZE;
		// vzdalenost od cile je mensi nez kolik jeste ujedu
		if(distance_from_bomb_b_target -1 <=
				distance_to_bomb_b_flame ){
			// zarazim se tedy pred jejim cilem
			move_field_by_direction_(field, -1);
		} else {
			Uint16 from_my_target_to_bomb_b_target =
				distance_from_bomb_b_target
				- distance_to_bomb_b_flame;
			// vzdalenost cilu je mensi nez plamen
			if(from_my_target_to_bomb_b_target
					<= bomb_b->flamesize_){
				// pouziju vzdalenost cilu
				move_field_by_direction_(field,
					-from_my_target_to_bomb_b_target);
			} else {
				// pouziju velikost plamene
				move_field_by_direction_(field,
					-bomb_b->flamesize_);
			}
		}
	}
	return field;
}

/** @details
 * Vytvoří presumpci, pokud ještě na políčku není,
 * vloží ji do mapy a do seznamu presumpcí bomby.
 * Pokud na políčku už je, nastaví jí aspon svojí dobu do výbuchu,
 * pokud je doba do výbuchu presumpce vetší.
 * @param field políčko na které chceme vkládat
 * @return Vrací FALSE, pokud je presumpce v tomto smeru zastavena.
 * Pokud je volno, vrací TRUE.
 */
bool Bomb::update_presumption_(const field_t & field){
	// mimo mapu nevkladam
	if(field.first >= GAME->map_width()
	|| field.second >= GAME->map_height()){
		return false;
	}
	// pod zed a bednu nevkladam
	if(GAME->field_withObject(field, isTypeOf::isWallBox)){
		return false;
	}
	// presumpce na policku
	Presumption * presumption = static_cast<Presumption *>(
		GAME->field_getObject(field, isTypeOf::isPresumption) );
	if(presumption){
		// uz tam naka je, nastavim ji novou dobu do vyhoreni, pokud se snizi
		if(presumption->periods_to_flame_ > periods_to_flame_){
			presumption->periods_to_flame_ = periods_to_flame_;
		}
	} else {
		// na policku jeste neni zadna presumpce
		// priprava nove presumpce
		presumption =
			GAME->tools->presumption(
				field.first, field.second,
				periods_to_flame_);
		// vlozeni presumpce do hry
		GAME->insert_object(field, presumption);
		// vlozeni do seznamu presumpci
		presumptions_.push_back(presumption);
	}
	// zarazi se za bombou
	return true;
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
 * @param rect obdelnik mapy v okne
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
 * Výbuch bomby.
 * Pokud může (s ohledem na okolí) rozmístí a nechá bouchnout
 * okolo sebe čtyři bomby (maximálně).
 * Ohlídá si, aby běžela pouze jednou,
 * protože se nesmí vytvářet bomby vícekrát.
 */
void MegaBomb::explode(){
	if(explodes_) return;
	// zde nenastavovat explodes_ na true
	// nastavi se na konci v Bomb::explode()

	// zatřást hrou
	GAME->shake_views();

	// aktualni policko
	field_t curr_field(x_/CELL_SIZE, y_/CELL_SIZE);

	// pres vsechny smery
	for(Uint16 dir=0 ; dir<4 ; ++dir){
		Sint16 factor_x = 0, factor_y = 0;
		if(dir%2)
			factor_x = 2-dir;
		else
			factor_y = 1-dir;
		// zacneme ze stredu
		field_t next_field = curr_field;
		// posuneme se podle smeru na vedlejsi policko
		next_field.first += factor_x;
		next_field.second += factor_y;
		// vytvorime tam bombu
		try_insert_bomb_(next_field);
	}

	// pro odpaleni beden v okoli (coz je funkcni zalezitost)
	// a pro vytvoreni flame_cross (coz je jen kosmetika)
	Bomb::explode();
}

/** @details
 * Pokud to jde (s ohledem na okolí), vloží na políčko novou bombu.
 */
void MegaBomb::try_insert_bomb_(const field_t & field){
	// kdyz tam muzeme umistit bombu
	if(!GAME->field_withObject(field, isTypeOf::isWallBox)){
		// umistime tam bombu
		Bomb * bomb = GAME->tools->bomb_normal(field, flamesize_);
		// hned nebouchne, prvne musi bouchnout hlavni stredova bomba,
		// od ni pak automaticky bouchne i nově vytvořená

		// vlozime ji do hry (mapy)
		GAME->insert_object(field, bomb);
	}
}

/**
 */
void MegaBomb::update_presumptions_(){
	target_field_ = find_target_field_();

	// na svoje policko
	update_presumption_(target_field_);
	// pres vsechny smery
	for(Uint16 dir=0 ; dir<4 ; ++dir){
		Sint16 factor_x = 0, factor_y = 0;
		if(dir%2)
			factor_x = 2-dir;
		else
			factor_y = 1-dir;
		// zacneme ze stredu
		field_t next_field(target_field_);
		// posuneme se podle smeru na vedlejsi policko
		next_field.first += factor_x;
		next_field.second += factor_y;
		// opet pres vsechny smery
		for(Uint16 inner_dir=0 ; inner_dir<4 ; ++inner_dir){
			Sint16 inner_factor_x = 0, inner_factor_y = 0;
			if(inner_dir%2)
				inner_factor_x = 2-inner_dir;
			else
				inner_factor_y = 1-inner_dir;
			// vytvorime v danem inner_smeru presumpce
			update_presumptions_from_field_(
				next_field, inner_factor_x, inner_factor_y);
		}
	}
}
