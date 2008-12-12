
#include <string>
#include <utility>
#include "SDL_lib.h"
#include "tixml_helper.h"
#include "stl_helper.h"
#include "constants.h"
#include "game_tools.h"
#include "game_mapobjects.h"
#include "game_bomb.h"
#include "game_flame.h"
#include "game_presumption.h"

using namespace std;

/******************* class GameTools ******************************/

GameTools::GameTools(){
	positioned_surface_t empty_positioned_sur = {0, 0, 0};
	// pripravim seznam pro pozicovane obrazky
	panels_.insert(panels_.end(), 4, empty_positioned_sur);
	bonuses_.insert(bonuses_.end(),
		GAMETOOLS_BONUSES_COUNT, empty_positioned_sur);

	TiXmlDocument doc;
	TiXmlElement *root_el;
	string filename("gametools");
	// nacteni XML souboru
	root_el = TiXmlRootElement(doc, filename, "gametools", false);
	try{
		// nacteni zdrojoveho surface
		Surface sur_src= load_src_surface(root_el);
		// nacteni plamenu
		load_flame_(subElement(root_el, "flame"), sur_src);
		// nacteni bomb
		load_bombs_(subElement(root_el, "bombs"), sur_src);
		// nacteni panelu
		load_panels_(subElement(root_el, "panels"), sur_src);
		// nacteni zmensenin bonusu
		load_bonuses_(subElement(root_el, "bonuses"), sur_src);
	}
	catch(const string & err){
		TiXmlError(filename, err);
	}
}

void GameTools::load_flame_(TiXmlElement *flameEl, const Surface & sur_src){
	readAttr(flameEl, "timeperiod", flame_period_);

	load_subEl_animation(flameEl, "top", flame_top_, sur_src);
	load_subEl_animation(flameEl, "bottom", flame_bottom_, sur_src);
	load_subEl_animation(flameEl, "topbottom", flame_topbottom_, sur_src);
	load_subEl_animation(flameEl, "left", flame_left_, sur_src);
	load_subEl_animation(flameEl, "right", flame_right_, sur_src);
	load_subEl_animation(flameEl, "leftright", flame_leftright_, sur_src);
	load_subEl_animation(flameEl, "cross", flame_cross_, sur_src);
}

void GameTools::load_bombs_(TiXmlElement *bombsEl, const Surface & sur_src){
	readAttr(bombsEl, "speed", bomb_speed_);

	load_subEl_animation(bombsEl, "normal", bomb_normal_, sur_src);
	load_subEl_animation(bombsEl, "mega", bomb_mega_, sur_src);
	load_subEl_animation(bombsEl, "presumption", presumption_, sur_src);
}


void GameTools::load_panels_(TiXmlElement *panelsEl, const Surface & sur_src){
	// sirska vyska
	Uint16 width, height;
	readAttr(panelsEl, "width", width);
	readAttr(panelsEl, "height", height);

	string el_name;
	for(Uint16 count=0 ; count < panels_.size() ; ++count ){
		el_name = "player"+x2string(count);

		load_subEl_surface(panelsEl, el_name.c_str(), panels_[count].sur,
				width, height, sur_src);
		set_transparency(panels_[count].sur.getSurface(), 128);
		switch(count){
			case 0:
				panels_[0].x = 0;
				panels_[0].y = 0;
				break;
			case 1:
				panels_[1].x = width;
				panels_[1].y = height;
				break;
			case 2:
				panels_[2].x = 0;
				panels_[2].y = height;
				break;
			case 3:
				panels_[3].x = width;
				panels_[3].y = 0;
		}
	}
}

void GameTools::load_bonuses_(TiXmlElement *bonusesEl, const Surface & sur_src){
	// sirska vyska
	Uint16 width, height;
	readAttr(bonusesEl, "width", width);
	readAttr(bonusesEl, "height", height);

	TiXmlElement * el;
	char * bonuses_names[] = GAMETOOLS_BONUSES_NAMES;
	for(Uint16 i =0 ; i< GAMETOOLS_BONUSES_COUNT ; ++i){
		load_subEl_surface(bonusesEl, bonuses_names[i],
				bonuses_[i].sur, width, height, sur_src);
		el = subElement(bonusesEl, bonuses_names[i]);
		readAttr(el, "draw_x", bonuses_[i].x);
		readAttr(el, "draw_y", bonuses_[i].y);
	}
}

Flame* GameTools::flame_top(Uint16 x, Uint16 y) const {
	return new Flame(flame_top_, x*CELL_SIZE, y*CELL_SIZE, flame_period_);
}

Flame* GameTools::flame_bottom(Uint16 x, Uint16 y) const {
	return new Flame(flame_bottom_, x*CELL_SIZE, y*CELL_SIZE, flame_period_);
}

Flame* GameTools::flame_topbottom(Uint16 x, Uint16 y) const {
	return new Flame(flame_topbottom_, x*CELL_SIZE, y*CELL_SIZE, flame_period_);
}

Flame* GameTools::flame_left(Uint16 x, Uint16 y) const {
	return new Flame(flame_left_, x*CELL_SIZE, y*CELL_SIZE, flame_period_);
}

Flame* GameTools::flame_right(Uint16 x, Uint16 y) const {
	return new Flame(flame_right_, x*CELL_SIZE, y*CELL_SIZE, flame_period_);
}

Flame* GameTools::flame_leftright(Uint16 x, Uint16 y) const {
	return new Flame(flame_leftright_, x*CELL_SIZE, y*CELL_SIZE, flame_period_);
}

Flame* GameTools::flame_cross(Uint16 x, Uint16 y) const {
	return new Flame(flame_cross_, x*CELL_SIZE, y*CELL_SIZE, flame_period_);
}

Bomb* GameTools::bomb_normal(Uint16 x, Uint16 y,
				Uint16 flamesize, bool timer) const {
	return new Bomb(bomb_normal_,
		x*CELL_SIZE+CELL_SIZE/2, y*CELL_SIZE+CELL_SIZE/2,
		flamesize, bomb_speed_, timer);
}

Bomb* GameTools::bomb_mega(Uint16 x, Uint16 y,
				Uint16 flamesize, bool timer) const {
	return new MegaBomb(bomb_mega_,
		x*CELL_SIZE+CELL_SIZE/2, y*CELL_SIZE+CELL_SIZE/2,
		flamesize, bomb_speed_, timer);
}

Presumption* GameTools::presumption(Uint16 x, Uint16 y, Uint16 to_end) {
	return new Presumption( &presumption_,
		x*CELL_SIZE, y*CELL_SIZE, to_end);
}

Uint16 GameTools::get_panel_x_(Uint16 player_num, const SDL_Rect & rect){
	return panels_[player_num].x
		? rect.x+rect.w - panels_[player_num].x
		: rect.x+panels_[player_num].x;
}
Uint16 GameTools::get_panel_y_(Uint16 player_num, const SDL_Rect & rect){
	return panels_[player_num].y
		? rect.y+rect.h - panels_[player_num].y
		: rect.y+panels_[player_num].y;
}

void GameTools::draw_panel_player(SDL_Surface * window, const SDL_Rect & rect,
			Uint16 player_num, Uint16 lives, Uint16 flames, Uint16 bombs,
			Uint16 megabombs, bool slider, bool kicker){
	// panel
	draw_surface(
		get_panel_x_(player_num, rect), get_panel_y_(player_num, rect),
		panels_[player_num].sur.getSurface(), window);
	// pocet zivotu
	Surface text = text_(x2string(lives)+"x");
	draw_surface(
		get_panel_x_(player_num, rect) + 8,
		get_panel_y_(player_num, rect) + 4,
		text.getSurface(), window);

	// plameny
	text = text_(x2string(flames)+"x");
	draw_surface(
		get_panel_x_(player_num, rect) + bonuses_[MEGABOMB].x +
			bonuses_[MEGABOMB].sur.width()*3/2 - text.width()/2,
		get_panel_y_(player_num, rect) + bonuses_[SHIELD].y +
			bonuses_[SHIELD].sur.height()/2 - text.height()/2,
		text.getSurface(), window);
	// normalni bomby
	if(!megabombs){
		text = text_(x2string(bombs)+"x");
		draw_surface(
			get_panel_x_(player_num, rect) + bonuses_[MEGABOMB].x +
				bonuses_[MEGABOMB].sur.width()*3/2 - text.width()/2,
			get_panel_y_(player_num, rect) + bonuses_[MEGABOMB].y +
				bonuses_[MEGABOMB].sur.height()/2 - text.height()/2,
			text.getSurface(), window);
	}
	// megabomby
	if(megabombs)
		draw_panel_bonus(window, rect, player_num,
			MEGABOMB, x2string(megabombs)+"x");
	// posilani
	if(slider)
		draw_panel_bonus(window, rect, player_num,
			SLIDER);
	// kopani
	if(kicker)
		draw_panel_bonus(window, rect, player_num,
			KICKER);
}

void GameTools::draw_panel_bonus(SDL_Surface * window, const SDL_Rect & rect,
			Uint16 player_num, BONUSES bonus, const std::string & val){
	// obrazek
	draw_surface(
		 get_panel_x_(player_num, rect) + bonuses_[bonus].x,
		 get_panel_y_(player_num, rect) + bonuses_[bonus].y,
		bonuses_[bonus].sur.getSurface(), window);
	// hodnota
	if(!val.empty()){
		Surface text = text_(val);
		draw_surface(
			get_panel_x_(player_num, rect) + bonuses_[bonus].x +
				bonuses_[bonus].sur.width()*3/2 - text.width()/2,
			get_panel_y_(player_num, rect) + bonuses_[bonus].y +
				bonuses_[bonus].sur.height()/2 - text.height()/2,
			text.getSurface(), window);
	}
}

const Surface & GameTools::text_(const std::string & text){
	texts_t::iterator it;
	it= texts_.find(text);
	// zjistim jestli uz byl text vytvoren
	if(it==texts_.end()){
		extern Fonts * g_font;
		// vytvorim surface
		Surface sur = get_text( (*g_font)[GAMETOOLS_BONUSES_FONT_SIZE],
					text.c_str(), Color::white);
		// a ulozim do mapy
		it = texts_.insert(it, make_pair(text, sur));
	}
	return it->second; // vracim jiz vytvoreny text
}

/************** END OF class GameTools ******************************/

