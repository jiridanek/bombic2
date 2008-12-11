
#include <string>
#include "agar_helper.h"
#include "SDL_lib.h"
#include "constants.h"
#include "tixml_helper.h"
#include "deathmatch_intro.h"
#include "game_base.h"
#include "game_tools.h"
#include "game.h"

using namespace std;


DeathmatchIntro::DeathmatchIntro(Uint16 players_count,
		const std::string & mapname, const bonuses_t & bonuses,
		Uint16 wins, bool creatures, bool bombsatend):
	gameBase_( new GameBase(players_count, mapname, bonuses, creatures) ),
	gameTools_(new GameTools), game_(0),
	wins_(wins), bombsatend_(bombsatend) {

	// pripravi prostor pro skore hracu
	score_.insert(score_.end(), players_count, 0);
}

DeathmatchIntro::~DeathmatchIntro(){
	if(gameBase_)
		delete gameBase_;
	if(gameTools_)
		delete gameTools_;
	if(game_)
		delete game_;
}

extern SDL_Surface *g_window;

/** @details
 * Ukáže úvodní obrazovku matche, vygeneruje konkrétní hru,
 * čeká na klávesu, spustí hru. Po skončení upraví skóre a hraje se dál.
 */
void DeathmatchIntro::show_screen(){
	while(true){
		// uvodni obrazovka matche
		clear_surface(Color::black, g_window);
// 		draw_center_surface(get_cur_image_().getSurface(), g_window);
		SDL_Flip(g_window);

		// pockame na klavesu, pri pokusu o ukonceni ukoncime
		SDLKey key;
		while(true){
			switch(wait_event(key)){
				case SDL_VIDEORESIZE:
					// uvodni obrazovka levelu
					clear_surface(Color::black, g_window);
// 					draw_center_surface(get_cur_image_().getSurface(), g_window);
					SDL_Flip(g_window);
					continue;
				case SDL_QUIT:
					AG_Quit();
					return;
				case SDL_KEYUP:
					if(key==SDLK_ESCAPE)
						return;
				default:
					break;
			}
			break;
		}

		// vygenerovani nove hry z pripraveneho zakladu
		if(game_) delete game_;
		game_ = new Game(*gameBase_, gameTools_, true);

		// hrajeme
		game_->play(g_window);
		// TODO score

		delete game_;
		game_ = 0;
	}

	if(game_)
		delete game_;
	game_ = 0;

}

