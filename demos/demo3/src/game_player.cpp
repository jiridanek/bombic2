
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
	num_(num), flamesize_(3), bombs_(3), next_timer_(0),
	key_up_(SDLK_UP), key_right_(SDLK_RIGHT), key_down_(SDLK_DOWN),
	key_left_(SDLK_LEFT), key_plant_(SDLK_RETURN), key_timer_(SDLK_DELETE){

	d_ = DOWN;
}

Player::Player(const Player & player, Uint16 x, Uint16 y):
	Creature(player, x, y), num_(player.num_),
	flamesize_(player.flamesize_), bombs_(player.bombs_), next_timer_(0),
	key_up_(player.key_up_), key_right_(player.key_right_),
	key_down_(player.key_down_), key_left_(player.key_left_),
	key_plant_(player.key_plant_), key_timer_(player.key_timer_) {}

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
		d_=BURNED;
	// pohyb jako prisera
	return Creature::move();
}

