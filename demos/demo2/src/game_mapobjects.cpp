

#include <iostream>
#include "SDL_lib.h"
#include "constants.h"
#include "game.h"
#include "game_mapobjects.h"



/** @details
 * TODO
 */
void DynamicMO::setFieldInMap(Uint16 old_x, Uint16 old_y, Uint16 x, Uint16 y){
	if(old_x==x && old_y==y) return;
	// vyhozeni ze stareho policka
	Game::map_array_t::value_type::value_type::iterator it;
	it= Game::map_array_[old_x][old_y].end();
	// TODO find
	while(it!= Game::map_array_[old_x][old_y].begin()){
		--it;
		if(*it==this){
			Game::map_array_[old_x][old_y].erase(it);
			break;
		}
	}
	// vlozeni na nove policko
	Game::map_array_[x][y].push_back(this);
}

/** @details
 * TODO
 */
void DynamicMO::centerPosition(Uint16 & x, Uint16 & y){
	// nahore
	if(y%CELL_SIZE+3<CELL_SIZE/2){
		if(!Game::field_canGoOver(x/CELL_SIZE,y/CELL_SIZE-1))
			y+=1;
	}
	// dole
	else if(y%CELL_SIZE-1>CELL_SIZE/2){
		if(!Game::field_canGoOver(x/CELL_SIZE,y/CELL_SIZE+1))
			y-=3;
	}
	// vlevo
	if(x%CELL_SIZE+2<CELL_SIZE/2){
		if(!Game::field_canGoOver(x/CELL_SIZE-1,y/CELL_SIZE))
			x+=2;
	}
	// vpravo
	else if(x%CELL_SIZE-2>CELL_SIZE/2){
		if(!Game::field_canGoOver(x/CELL_SIZE+1,y/CELL_SIZE))
			x-=2;
	}
}
