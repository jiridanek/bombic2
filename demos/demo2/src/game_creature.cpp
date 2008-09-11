
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
	speed_(speed), lives_(lives), ai_(AI::new_ai(this, ai)) {}

Creature::~Creature(){
	if(ai_)
		delete ai_;
}

void Creature::move(){
	Uint16 old_x=x_, old_y=y_;
	if(ai_) ai_->move();

	centerPosition(x_,y_);
	setFieldInMap(old_x/CELL_SIZE, old_y/CELL_SIZE, x_/CELL_SIZE, y_/CELL_SIZE);
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
	/*/ TODO debug
	draw_pixel(window, x_, y_, Color::red);
	sur = get_text(g_font[10],
		("["+x2string(x_/CELL_SIZE)+","+x2string(y_/CELL_SIZE)+","+x2string(this->getZ())+"]").c_str(),
		Color::yellow);
	draw_surface(x-CELL_SIZE, y, sur.GetSurface(), window);
	//*/
}
