
#include <iostream>
#include <list>
#include <vector>
#include <string>
#include "SDL_lib.h"
#include "game.h"

/*************** class Game ******************************/
/// Inicializace hry.
Game::Game(Uint8 players_count, const std::string & mapname){
// 	bool deathmatch, bool creatures, bool bombsatend){

}

/// Nastavení parametrů hráče.
void Game::set_player(Uint8 player_num, Uint8 lives,
	Uint8 bombs, Uint8 flames, Uint8 boots){
}
/// Spuštění hry.
void Game::play(){
	std::cout << "Play" << std::endl;
}
/// Info o ukončení hry.
bool Game::success() const{
	return true;
}
/// Info o hráči.
void Game::player(Uint8 player_num, Uint8 & lives,
	Uint8 & bombs, Uint8 & flames, Uint8 & boots) const{
}


/*
/// Seznam dynamických objektů mapy.
std::list<DynamicMO> dynamicMOs_;
/// Seznam statických objektů mapy.
std::vector<StaticMO> staticMOs_;
/// Dvourozměrné pole mapy se seznamem objektů na něm položených.
std::vector< std::vector< std::list< MapObject* > > > map_array_;
/*************** END OF class Game ******************************/
