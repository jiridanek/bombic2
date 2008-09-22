
#include <iostream>
#include "SDL_lib.h"
#include "constants.h"
#include "stl_helper.h"
#include "game.h"
#include "game_mapobjects.h"
#include "game_player.h"
#include "game_AI.h"

Player::Player(const Surface & sur_left, const Surface & sur_left_s,
			const Surface & sur_up, const Surface & sur_up_s,
			const Surface & sur_right, const Surface & sur_right_s,
			const Surface & sur_down, const Surface & sur_down_s,
			const Surface & sur_burned, Uint16 x, Uint16 y,
			Uint16 speed, Uint16 lives):
	Creature(sur_left, sur_left_s, sur_up, sur_up_s, sur_right, sur_right_s,
		sur_down, sur_down_s, sur_burned, x, y, speed, lives, 0),
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
	Uint16 accessed = ++access_counter_%speed_rate_;
	if(accessed!=0 && accessed!=speed_rate_/2){
		Uint16 old_x=x_, old_y=y_;
		if(ai_) ai_->move();
		setFieldInMap(old_x, old_y);
	}
	// update animation

}

