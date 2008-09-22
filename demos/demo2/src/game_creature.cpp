
#include <iostream>
#include "SDL_lib.h"
#include "constants.h"
#include "stl_helper.h"
#include "game.h"
#include "game_mapobjects.h"
#include "game_creature.h"
#include "game_AI.h"

Creature::Creature(const Surface & sur_left, const Surface & sur_left_s,
			const Surface & sur_up, const Surface & sur_up_s,
			const Surface & sur_right, const Surface & sur_right_s,
			const Surface & sur_down, const Surface & sur_down_s,
			const Surface & sur_burned, Uint16 x, Uint16 y,
			Uint16 speed, Uint16 lives, Uint16 ai):
	DynamicMO(x, y),
	sur_left_(sur_left),sur_left_s_(sur_left_s),
	sur_up_(sur_up), sur_up_s_(sur_up_s),
	sur_right_(sur_right), sur_right_s_(sur_right_s),
	sur_down_(sur_down), sur_down_s_(sur_down_s),
	sur_burned_(sur_burned), d_(static_cast<DIRECTION>(rand()%4)),
	// pro zjednoduseni zachazeni s rychlosti
	speed_diff_((speed-1)/7+1), speed_rate_((speed-1)%7+2+speed_diff_), lives_(lives),
	ai_(AI::new_ai(this, ai)), access_counter_(0) {}

Creature::~Creature(){
	if(ai_)
		delete ai_;
}

void Creature::move(){
	Uint16 accessed = ++access_counter_%speed_rate_;
	if(accessed!=0 && accessed!=speed_rate_/2){
		Uint16 old_x=x_, old_y=y_;
		if(ai_) ai_->move();

		setFieldInMap(old_x, old_y);
	}
	// update animation
}

extern Fonts g_font;

void Creature::draw(SDL_Surface *window){
	int x=x_-sur_left_.width()+CELL_SIZE/2;
	int y=y_-sur_left_.height()+CELL_SIZE/2;

	Surface sur=0, sur_s=0;
	switch(d_){
		case UP: sur=sur_up_; sur_s=sur_up_s_; break;
		case RIGHT: sur=sur_right_; sur_s=sur_right_s_; break;
		case DOWN: sur=sur_down_; sur_s=sur_down_s_; break;
		case LEFT: sur=sur_left_; sur_s=sur_left_s_; break;
		case BURNED: sur=sur_burned_; break;
	}
	if(sur_s.GetSurface())
		draw_surface(x, y, sur_s.GetSurface(), window);
	draw_surface(x, y, sur.GetSurface(), window);
	// TODO debug
	draw_pixel(window, x_, y_, Color::red);
	sur = get_text(g_font[10],
		("["+x2string(x_/CELL_SIZE)+","+x2string(y_/CELL_SIZE)+","+x2string(this->getZ())+"] "
			+x2string(speed_rate_)+":"+x2string(speed_diff_)).c_str(),
		Color::yellow);
	draw_surface(x-CELL_SIZE, y, sur.GetSurface(), window);
	//*/
}
