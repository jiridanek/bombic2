
#include <iostream>
#include "SDL_lib.h"
#include "constants.h"
#include "stl_helper.h"
#include "game.h"
#include "game_mapobjects.h"
#include "game_player.h"
#include "game_AI.h"


/**
 * @see Creature::Creature()
 */
Player::Player(const Animation & anim_up, const Animation & anim_right,
			const Animation & anim_down, const Animation & anim_left,
			const Animation & anim_burned, Uint16 x, Uint16 y,
			Uint16 speed, Uint16 lives, Uint16 num):
	Creature(anim_up, anim_right, anim_down, anim_left, anim_burned, x, y, speed, lives, -1),
	num_(num), flamesize_(3), bombs_(3), next_timer_(0){

	d_ = DOWN;
}

/** @details
 * @param player hráč, jehož položky se mají okopírovat
 * @param x nové souřadnice středu v pixelech
 * @param y nové souřadnice středu v pixelech
 */
Player::Player(const Player & player, Uint16 x, Uint16 y):
	Creature(player, x, y), num_(player.num_),
	flamesize_(player.flamesize_), bombs_(player.bombs_), next_timer_(0) {}

/** @details
 * @param player hráč pro srovnání
 * @return Vrací TRUE, pokud mají hráči stejné souřadnice i směr otočení.
 * Ve hře to znamená, že se překrývají, tudíž spodní není vůbec vidět.
 */
bool Player::operator==(const Player & player) const {
	return x_==player.x_ && y_==player.y_ && d_==player.d_;
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
	if(Game::get_instance()->field_withObject(x_/CELL_SIZE, y_/CELL_SIZE, CREATURE))
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
	// TODO update all my bonuses
}

/** @details
 * Vykreslí svůj panel a všechny svoje bonusy a jejich hodnoty do něj.
 * @param window surface okna pro vykreslení
 */
void Player::drawPanel(SDL_Surface *window){
	Game * game = Game::get_instance();
	game->tools->draw_panel_player(
		window, num_, flamesize_, bombs_ - game->count_bombs(num_),
		0, false, false);
	game->tools->draw_panel_player(
		window, num_+2, flamesize_, bombs_ - game->count_bombs(num_),
		5, true, true);
}
