
#include "SDL_lib.h"
#include "constants.h"
#include "stl_helper.h"
#include "game.h"
#include "game_mapobjects.h"
#include "game_player.h"
#include "game_AI.h"
#include "game_bonus_application.h"


/**
 * @see Creature::Creature()
 */
Player::Player(const Animation & anim_up, const Animation & anim_right,
			const Animation & anim_down, const Animation & anim_left,
			const Animation & anim_burned, Uint16 x, Uint16 y,
			Uint16 speed, Uint16 num):
	Creature(anim_up, anim_right, anim_down, anim_left, anim_burned, x, y, speed, 1, -1),
	num_(num), flamesize_(1), bombs_(1), megabombs_(0), boots_(0), next_timer_(0),
	bonus_kicker_(false), bonus_slider_(false),
	bonus_timer_(false), bonus_fireman_(false){

	d_ = DOWN;
}

/** @details
 * @param player hráč, jehož položky se mají okopírovat
 * @param x nové souřadnice středu v pixelech
 * @param y nové souřadnice středu v pixelech
 */
Player::Player(const Player & player, Uint16 x, Uint16 y):
	Creature(player, x, y), num_(player.num_),
	flamesize_(player.flamesize_), bombs_(player.bombs_),
	megabombs_(0), boots_(player.boots_), next_timer_(0),
	bonus_kicker_(false), bonus_slider_(false),
	bonus_timer_(false), bonus_fireman_(false) {}

Player::~Player(){
	bonuses_t::iterator it;
	for(it = bonuses_.begin() ; it!=bonuses_.end() ; ++it)
		delete *it;
}

/**
 */
void Player::set_properties(const PlayerProperties & prop){
	lives_ = prop.lives;
	bombs_ = prop.bombs;
	flamesize_ = prop.flames;
	// odecist stare
	speed_rate_ -= boots_;
	// nastavit nove
	boots_ = prop.boots;
	// pricist nove
	speed_rate_ += boots_;
}

/**
 */
void Player::get_properties(PlayerProperties & prop) const {
	prop.lives = lives_;
	prop.bombs = bombs_;
	prop.flames = flamesize_;
	prop.boots = boots_;
}

/** @details
 * @param player hráč pro srovnání
 * @return Vrací TRUE, pokud mají hráči stejné souřadnice i směr otočení.
 * Ve hře to znamená, že se překrývají, tudíž spodní není vůbec vidět.
 */
bool Player::operator==(const Player & player) const {
	return d_==player.d_
		&& abs_minus(x_, player.x_)<3
		&& abs_minus(y_, player.y_)<3;
}

/** @details
 * Navíc hlídá, zda ho nesezrala nestvůra.
 * @see Creature::move()
 * @return Vrací TRUE, pokud se má objekt zahodit.
 */
bool Player::move(){
	// zmensime dobu dalsiho odpalu
	if(next_timer_ >= MOVE_PERIOD)
		next_timer_ -= MOVE_PERIOD;

	// prisery zabijeji
	if(GAME->field_withObject(x_/CELL_SIZE, y_/CELL_SIZE, CREATURE))
		die();
	// pohyb jako prisera
	return Creature::move();
}

/** @details
 * Posune animaci.
 * Obnoví stav všech bonusů, které vlastní.
 * Ty, které už nemají působit zruší.
 * @see Creature::update()
 */
void Player::update(){
	Creature::update();

	bonuses_t::iterator it;
	for(it = bonuses_.begin() ; it!=bonuses_.end() ; ++it){
		if((*it)->update()){
			delete *it;
			*it = 0;
		}
	}
	bonuses_.remove(0);
}

/** @details
 * Vykreslí svůj panel a všechny svoje bonusy a jejich hodnoty do něj.
 * @param window surface okna pro vykreslení
 */
void Player::draw_panel(SDL_Surface *window, const SDL_Rect & rect){
	// vykreslit svuj panel
	GAME->tools->draw_panel_player(
		window, rect, num_,
		lives_, flamesize_, bombs_ - GAME->count_bombs(num_),
		megabombs_, bonus_slider_, bonus_kicker_);
	// do nej svoje bonusy
	bonuses_t::iterator it;
	for(it = bonuses_.begin() ; it!=bonuses_.end() ; ++it){
		(*it)->draw_panel(window, rect);
	}
}
