
#include <iostream>
#include <string>
#include "agar_helper.h"
#include "SDL_lib.h"
#include "constants.h"
#include "tixml_helper.h"
#include "game_intro.h"
#include "game_base.h"
#include "game_tools.h"
#include "game.h"
#include "game_player.h"

using namespace std;


GameIntro::GameIntro(): gameBase_(0), gameTools_(new GameTools), game_(0) {}

GameIntro::~GameIntro(){
	if(gameBase_)
		delete gameBase_;
	if(gameTools_)
		delete gameTools_;
	if(game_)
		delete game_;
}

extern SDL_Surface *g_window;

/** @details
 * Ukáže úvodní obrazovku levelu, vygeneruje konkrétní hru,
 * čeká na klávesu, spustí hru. Po skončení spustí hru znova TODO
 * @throw string Chybové hlášení pokud není inicializována hra.
 * @see new_game(), load_game();
 */
void GameIntro::show_screen(){
	if(!gameTools_)
		throw string("in GameIntro::show_screen(): no gameTools created");

	Uint16 old_level = cur_level_;
	while(cur_level_ < levels_.size()){
		// uvodni obrazovka levelu
		clear_surface(Color::black, g_window);
		draw_center_surface(get_cur_image_().getSurface(), g_window);
		SDL_Flip(g_window);

		// vygenerovani noveho levelu
		if(old_level!=cur_level_ || !gameBase_){
			old_level = cur_level_;
			if(gameBase_) delete gameBase_;
			gameBase_ = new GameBase(
				players_count_, levels_[cur_level_].map);
			// pokud mam odkud, nastavim hrace
			if(game_){
				PlayerProperties prop;
				for(Uint16 i = 0 ; i < players_count_ ; ++i){
					if(game_->get_player(i, prop))
						gameBase_->set_player(i, prop);
				}
			}
		}

		// vygenerovani nove hry z pripraveneho zakladu
		if(game_) delete game_;
		game_ = new Game(*gameBase_, gameTools_);

		// pockame na klavesu, pri pokusu o ukonceni ukoncime
		SDLKey key;
		while(true){
			switch(wait_event(key)){
				case SDL_VIDEORESIZE:
					// uvodni obrazovka levelu
					clear_surface(Color::black, g_window);
					draw_center_surface(get_cur_image_().getSurface(), g_window);
					SDL_Flip(g_window);
					continue;
				case SDL_QUIT:
					AG_Quit();
					return;
				case SDL_KEYDOWN:
					if(key==SDLK_ESCAPE)
						return;
				default:
					break;
			}
			break;
		}

		SDL_Delay(500);
		// hrajeme
		game_->play(g_window);
		// hra skoncila uspesne => dalsi kolo
		if(game_->success())
			++cur_level_;
	}
}


/// Inicializace nové hry.
void GameIntro::new_game(Uint16 episode, Uint16 players){
	cur_level_ = 0;
	players_count_ = players;

	load_levels_(episode);

	if(gameBase_){
		delete gameBase_;
		gameBase_ = 0;
	}
	if(game_){
		delete game_;
		game_ = 0;
	}
}

/// Inicializace.
void GameIntro::load_game(Uint16 episode, Uint16 level,
	const PlayerProperties & player1, const PlayerProperties & player2,
	const PlayerProperties & player3, const PlayerProperties & player4){

}

bool GameIntro::is_game(){
	return gameBase_!=0 && cur_level_ < levels_.size();
}

void GameIntro::load_levels_(Uint16 episode){
	string filename("levels");

	// nacteni hodnot z xml
	TiXmlDocument doc;
	TiXmlElement *el;
	el = TiXmlRootElement(doc, filename, "levels", false);
	el = el->FirstChildElement("episode");
	while(episode-- && el)
		el = el->NextSiblingElement("episode");
	if(!el)
		TiXmlError(filename, "too few episodes");
	// ulozeni vsech levelu do seznamu
	levels_.clear();
	level_t level;
	for(el = el->FirstChildElement("level") ; el ;
				el = el->NextSiblingElement("level")){
		readAttr(el, "map", level.map);
		readAttr(el, "img", level.img);
		levels_.push_back(level);
	}
	// kontrola na pocet levelu
	if(cur_level_ >= levels_.size())
		TiXmlError(filename, "too few levels in episode");
}

Surface & GameIntro::get_cur_image_(){
	string img_name = levels_[cur_level_].img;
	if(image_.first==img_name)
		return image_.second;
	image_.first = img_name;
	// odalokovat
	image_.second = 0;
	// naalokovat novy
	image_.second = IMG_Load(img_name.c_str());
	if(!image_.second.getSurface())
		throw "GameIntro::get_cur_image_(): Unable to load "+img_name;
	return image_.second;
}

