
#include <iostream>
#include <string>
#include "SDL_lib.h"
#include "constants.h"
#include "game_intro.h"
#include "game_base.h"
#include "game.h"

using namespace std;


GameIntro::GameIntro(): gameBase_(0), game_(0) {}

GameIntro::~GameIntro(){
	if(gameBase_)
		delete gameBase_;
	if(game_)
		delete game_;
}

extern SDL_Surface *g_window;
extern Fonts g_font;

/** @details
 * Ukáže úvodní obrazovku levelu, vygeneruje konkrétní hru,
 * čeká na klávesu, spustí hru. Po skončení spustí hru znova TODO
 * @throw string Chybové hlášení pokud není inicializována hra.
 * @see new_game(), load_game();
 */
void GameIntro::show_screen(){
	if(!gameBase_)
		throw string("in GameIntro::show_screen(): no gameBase created");

	while(true){
		// uvodni obrazovka levelu
		clear_surface(Color::black, g_window);
		SDL_Flip(g_window);

		// vygenerovani nove hry
		if(game_) delete game_;
		game_ = new Game(*gameBase_);

		// pockame na klavesu, pri pokusu o ukonceni ukoncime
		SDL_Delay(500);
		if(get_event_isquit(SDLK_ESCAPE))
// 		if(wait_event_isquit(SDLK_ESCAPE))
			return;

		// jinak hrajeme
		game_->play(g_window);
		// TODO check jak hra skoncila
	}
}


/// Inicializace nové hry.
void GameIntro::new_game(Uint16 episode, Uint16 players){
	if(gameBase_)
		delete gameBase_;
	gameBase_ = new GameBase(players, "map_forest_debug");

	while(players--){
		gameBase_->set_player(players, 1, 1, 1, 0);
	}
}

/// Inicializace.
void GameIntro::load_game(Uint16 episode, Uint16 level,
	const PlayerConfig & player1, const PlayerConfig & player2,
	const PlayerConfig & player3, const PlayerConfig & player4){
		// TODO
}

