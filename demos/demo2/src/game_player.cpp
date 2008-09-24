
#include <iostream>
#include "SDL_lib.h"
#include "constants.h"
#include "stl_helper.h"
#include "game.h"
#include "game_mapobjects.h"
#include "game_player.h"
#include "game_AI.h"

Player::Player(const Animation & anim_up, const Animation & anim_right,
			const Animation & anim_down, const Animation & anim_left,
			const Animation & anim_burned, Uint16 x, Uint16 y,
			Uint16 speed, Uint16 lives):
	Creature(anim_up, anim_right, anim_down, anim_left, anim_burned, x, y, speed, lives, 0),
	key_up_(SDLK_UP), key_right_(SDLK_RIGHT), key_down_(SDLK_DOWN),
	key_left_(SDLK_LEFT), key_plant_(SDLK_RETURN), key_timer_(SDLK_DELETE){

	d_ = DOWN;
	if(ai_)
		delete ai_;
	ai_ = new AI_fromKeyboard(this);
}

Player::~Player(){
	if(ai_)
		delete ai_;
}

void Player::move(){
	Creature::move();
	if(Game::field_withCreature(x_/CELL_SIZE, y_/CELL_SIZE))
		d_=BURNED;
}

