
#include <iostream>
#include "SDL_lib.h"
#include "constants.h"
#include "stl_helper.h"
#include "game.h"
#include "game_mapobjects.h"
#include "game_creature.h"
#include "game_AI.h"

/**
 * @param anim_up animace pro směr vzhůru
 * @param anim_right animace pro směr vpravo
 * @param anim_down animace pro směr dolů
 * @param anim_left animace pro směr vlevo
 * @param anim_burned animace pro umírání
 * @param x souřadnice středu v pixelech
 * @param y souřadnice středu
 * @param speed rychlost
 * @param lives počet životů
 * @param ai úroveň umělé inteligence
 */
Creature::Creature(const Animation & anim_up, const Animation & anim_right,
			const Animation & anim_down, const Animation & anim_left,
			const Animation & anim_burned, Uint16 x, Uint16 y,
			Uint16 speed, Uint16 lives, Sint16 ai):
	DynamicMO(x, y),
	anim_up_(anim_up), anim_right_(anim_right), anim_down_(anim_down),
	anim_left_(anim_left), anim_burned_(anim_burned),
	d_(static_cast<DIRECTION>(rand()%4)), ai_(AI::new_ai(this, ai)),
	moved_(false), access_counter_(0), last_die_(0), lives_(lives),
	// pro zjednoduseni zachazeni s rychlosti
	speed_diff_((speed-1)/7+1), speed_rate_((speed-1)%7+2+speed_diff_) {}

/** @details
 * Jakýsi copycontructor, který navíc k okopírování objektu nastaví souřadnice.
 * @param creature objekt k okopírování
 * @param x souřadnice středu v pixelech
 * @param y souřadnice středu v pixelech
 */
Creature::Creature(const Creature & creature, Uint16 x, Uint16 y):
	DynamicMO(x, y),
	anim_up_(creature.anim_up_), anim_right_(creature.anim_right_),
	anim_down_(creature.anim_down_), anim_left_(creature.anim_left_),
	anim_burned_(creature.anim_burned_), d_(creature.d_), ai_(AI::new_ai(this, creature.ai_)),
	moved_(false), access_counter_(0), last_die_(0), lives_(creature.lives_),
	// pro zjednoduseni zachazeni s rychlosti
	speed_diff_(creature.speed_diff_), speed_rate_(creature.speed_rate_) {}

/** @details
 * Destruuje umělou inteligenci.
 */
Creature::~Creature(){
	if(ai_)
		delete ai_;
}

/** @details
 * Pomocí umělé inteligence hýbne s nestvůrou,
 * posune frame animace.
 * @return Vrací TRUE pokud se má objekt zahodit.
 */
bool Creature::move(){
	if(Game::get_instance()->field_withObject(x_/CELL_SIZE, y_/CELL_SIZE, FLAME))
		die();
	// mrtvoly se nehybou
	if(d_==BURNED)
		return anim_burned_.run_num()>0;

	Uint16 accessed = ++access_counter_%speed_rate_;
	if(accessed!=0 && accessed!=speed_rate_/2){
		// puvodni pozice
		Uint16 old_x=x_, old_y=y_;
		DIRECTION old_d=d_;

		if(ai_) ai_->move();

		// nastavit novou pozici take v mape
		setFieldInMap(old_x, old_y);
		// zresetovat starou animaci
		if(old_d!=d_)
			anim_(old_d).reset();
		// nastavit priznak, zda se pohyboval
		moved_ = old_x!=x_ || old_y!=y_;
	}

	return false;
}

/** @details
 * Ubere život,
 * pokud životy dojdou, nastaví direction pro hoření.
 * Hlídá, kdy naposledy zemřel (přišel o život);
 */
void Creature::die(){
	if(last_die_ < CREATURE_PROTECTION_LENGTH)
		return;
	last_die_ = 0;
	if(lives_>1)
		--lives_;
	else
		d_ = BURNED;
}

extern Fonts g_font;

/**
 * @param window surface okna pro vykreslení
 */
void Creature::draw(SDL_Surface *window){
	int x=x_-anim_up_.width()+CELL_SIZE/2;
	int y=y_-anim_up_.height()+CELL_SIZE/2;

	anim_(d_).draw(window, x, y);

	/*/ TODO debug
	draw_pixel(window, x_, y_, Color::red);
	Surface sur = get_text(g_font[10],
		x2string(lives_).c_str(),
// 		("["+x2string(x_/CELL_SIZE)+","+x2string(y_/CELL_SIZE)+","+x2string(this->getZ())+"] "
// 			+x2string(speed_rate_)+":"+x2string(speed_diff_)).c_str(),
		Color::yellow);
	draw_surface(x-CELL_SIZE, y, sur.getSurface(), window);
	//*/
}

void Creature::update(){
	if(moved_ || d_==BURNED)
		anim_(d_).update();
	else
		anim_(d_).reset();

	if(last_die_ <= CREATURE_PROTECTION_LENGTH)
		last_die_ += MOVE_PERIOD;
}

/**
 * @param d směr, pro který chceme animaci
 * @return Vrací referenci na animaci zadaného směru pohybu.
 */
Animation & Creature::anim_(DIRECTION d) {
	switch(d){
		case UP: return anim_up_;
		case RIGHT: return anim_right_;
		case DOWN: return anim_down_;
		case LEFT: return anim_left_;
		case BURNED:
		default: return anim_burned_;
	}
}

