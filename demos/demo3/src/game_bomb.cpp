
#include <iostream>
#include "SDL_lib.h"
#include "constants.h"
#include "game.h"
#include "game_mapobjects.h"
#include "game_bomb.h"
#include "game_flame.h"

/**
 * @param anim animace
 * @param x souřadnice středu v pixelech
 * @param y souřadnice středu v pixelech
 * @param period délka života bomby v sekundách
 */
Bomb::Bomb(const Animation & anim, Uint16 x, Uint16 y, Uint16 period):
	DynamicMO(x, y),
	anim_(anim), explodes_(false) {}

/** @details
 * Posouvá animaci sám,
 * kontroluje jestli nejsme na konci,
 * jestli nejsme v plamenech,
 * vytváří plameny.
 * @return Vrací TRUE pokud se má objekt zahodit.
 */
bool Bomb::move(){
	if(anim_.update())
		explode();

	// TODO check flames
	return explodes_;
}

void Bomb::explode(){
	explodes_ = true;
	Game * game = Game::get_instance();
	Uint16 x = x_/CELL_SIZE,
		y = y_/CELL_SIZE;
	// TODO velikost plamene
	game->insert_object(x, y,
		game->tools->flame_cross(x, y) );
	game->insert_object(x-1, y,
		game->tools->flame_left(x-1, y) );
	game->insert_object(x+1, y,
		game->tools->flame_right(x+1, y) );
	game->insert_object(x, y-1,
		game->tools->flame_top(x, y-1) );
	game->insert_object(x, y+1,
		game->tools->flame_bottom(x, y+1) );
}

/**
 * @param window surface okna pro vykreslení
 */
void Bomb::draw(SDL_Surface *window){
	anim_.draw(window, x_-anim_.width()/2, y_-anim_.height()/2);
}

/** @details
 * Nechává posun animace na Bomb::move().
 */
void Bomb::update(){
}
