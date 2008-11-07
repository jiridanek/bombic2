
#include <iostream>
#include "SDL_lib.h"
#include "constants.h"
#include "game.h"
#include "game_mapobjects.h"
#include "game_bomb.h"
#include "game_flame.h"
#include "game_presumption.h"
#include "game_background.h"

/**
 * @param anim animace
 * @param x souřadnice středu v pixelech
 * @param y souřadnice středu v pixelech
 * @param period délka života bomby v sekundách
 */
Bomb::Bomb(const Animation & anim, Uint16 x, Uint16 y,
					Uint16 flamesize, bool timer):
	DynamicMO(x, y),
	anim_(anim), explodes_(false), timer_(timer), flamesize_(flamesize) {

	create_presumptions_();
}

/** @details
 * Posouvá animaci sám,
 * kontroluje jestli nejsme na konci,
 * jestli nejsme v plamenech,
 * vytváří plameny.
 * @return Vrací TRUE pokud se má objekt zahodit.
 * @see explode()
 */
bool Bomb::move(){
	if( ( !timer_ && anim_.update() )
	|| Game::get_instance()->field_withObject(x_/CELL_SIZE, y_/CELL_SIZE, FLAME))
		explode();
	return explodes_;
}

/** @details
 * Nastaví flag pro výbuch.
 * Vytvoří plameny s ohledem na okolí.
 * Ohlídá si, aby běžela pouze jednou,
 * protože nemá smysl vytvářet plameny dvakrát.
 */
void Bomb::explode(){
	// explode() muze bezet jen jednou
	if(explodes_) return;
	// vyhodit predpoved, ted tam bude plamen
	remove_presumptions_();

	explodes_ = true;
	Game * game = Game::get_instance();
	Uint16 x = x_/CELL_SIZE,
		y = y_/CELL_SIZE;

	game->insert_object(x, y,
		game->tools->flame_cross(x, y) );

	Uint16 i, dir, next_x, next_y;
	Sint16 factor_x, factor_y;
	Flame* flame;
	bool field_withBomb;
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
			// zarazi se na zdi a konec
			if(game->field_withObject(next_x, next_y, WALL))
				break;
			// priprava noveho plamene
			if(field_withBomb
					= game->field_withObject(next_x, next_y, BOMB)){
				flame = game->tools->flame_cross(next_x, next_y);
			}
			else switch(dir){
				case 0: flame= i==flamesize_
					? game->tools->flame_bottom(next_x, next_y)
					: game->tools->flame_topbottom(next_x, next_y);
					break;
				case 2: flame= i==flamesize_
					? game->tools->flame_top(next_x, next_y)
					: game->tools->flame_topbottom(next_x, next_y);
					break;
				case 1: flame= i==flamesize_
					? game->tools->flame_right(next_x, next_y)
					: game->tools->flame_leftright(next_x, next_y);
					break;
				case 3: flame= i==flamesize_
					? game->tools->flame_left(next_x, next_y)
					: game->tools->flame_leftright(next_x, next_y);
					break;
			}
			// vlozeni plamene do hry
			game->insert_object(next_x, next_y, flame);
			// zarazi se za bednou nebo bombou (obecne veci ktere maji bouchnout ale nemaji plamen)
			if(game->field_withObject(next_x, next_y, BOX)
			|| field_withBomb)
				break;
		}
	}
	// spali policko na kterym byla
	Background * bg = static_cast<Background *>(
		game->field_getObject(x, y, BACKGROUND) );
	if(bg) bg->set_burned();
}

/**
 */
void Bomb::remove_timer(){
	timer_=false;
}

/**
 */
void Bomb::create_presumptions_(){
	Game * game = Game::get_instance();
	Uint16 x = x_/CELL_SIZE,
		y = y_/CELL_SIZE;

	Uint16 i, dir, next_x, next_y;
	Sint16 factor_x, factor_y;

	Presumption* presumption = game->tools->presumption(x, y);
	game->insert_object(x, y, presumption);
	presumptions_.push_back(presumption);

	// pres vsechny smery
	for(dir=0 ; dir<4 ; ++dir){
		factor_x = factor_y = 0;
		if(dir%2)
			factor_x = 2-dir;
		else
			factor_y = 1-dir;;
		// pro velikost plamene
		for(i=1 ; i<=flamesize_ ; ++i){
			// souradnice pro novou presumpci
			next_x = x+ i*factor_x;
			next_y = y+ i*factor_y;
			// zarazi se pred zdi nebo pred bednou
			if(game->field_withObject(next_x, next_y, WALL)
			|| game->field_withObject(next_x, next_y, BOX))
				break;
			// priprava nove presumpce
			presumption = game->tools->presumption(next_x, next_y);
			// vlozeni presumpce do hry
			game->insert_object(next_x, next_y, presumption);
			// vlozeni do seznamu presumpci
			presumptions_.push_back(presumption);
			// zarazi se za bombou
			if(game->field_withObject(next_x, next_y, BOMB))
				break;
		}
	}
}

/**
 */
void Bomb::move_presumptions_(){
}

/** @details
 * Vyhodí všechny presumpce bomby.
 * @see Game::remove_object()
 */
void Bomb::remove_presumptions_(){
	Game * game = Game::get_instance();
	for(Uint16 i=0 ; i<presumptions_.size() ; ++i){
		game->remove_object(presumptions_[i]);
	}
	presumptions_.clear();
}

/**
 * @param window surface okna pro vykreslení
 */
void Bomb::draw(SDL_Surface *window, const SDL_Rect & rect){
	anim_.draw(window,
		x_-anim_.width()/2 +rect.x, y_-anim_.height()/2 +rect.y);
}

/** @details
 * Nechává posun animace na Bomb::move().
 */
void Bomb::update(){
}
