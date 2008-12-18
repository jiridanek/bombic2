
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
		if(!gameBase_ || game_){
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
				delete game_;
				game_ = 0;
			}
		}

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
				case SDL_KEYUP:
					if(key==SDLK_ESCAPE)
						return;
					if(key==SDLK_RETURN || key==SDLK_KP_ENTER
					|| key==SDLK_SPACE || key==SDLK_RCTRL)
						break;
				default:
					continue;
			}
			break;
		}

		// vygenerovani nove hry z pripraveneho zakladu
		if(game_) delete game_;
		game_ = new Game(*gameBase_, gameTools_);

		// hrajeme
		game_->play(g_window);
		// hra skoncila uspesne => dalsi kolo
		if(game_->success())
			++cur_level_;
		else {
			delete game_;
			game_ = 0;
		}
	}
	delete game_;
	game_ = 0;

}


/// Inicializace nové hry.
void GameIntro::new_game(Uint16 episode, Uint16 players){
	cur_episode_ = episode;
	cur_level_ = 0;
	players_count_ = players;

	load_levels_(episode);

	if(gameBase_){
		delete gameBase_;
		gameBase_ = 0;
	}
}

/// Inicializace.
void GameIntro::load_game(std::string & filename){
	delete game_;
	game_ = 0;

	TiXmlDocument doc;
	TiXmlElement *rootEl, *el;
	std::string el_name;
	try {
		// otevreni souboru
		rootEl = TiXmlRootElement(doc, filename, "game", false);
		// level
		readAttr(rootEl, "episode", cur_episode_);
		readAttr(rootEl, "level", cur_level_);
		load_levels_(cur_episode_);
		// pocet hracu
		Uint16 count;
		for(count = 4 ; count>0 ; --count ){
			el_name = "player"+x2string(count-1);
			if(rootEl->FirstChildElement(el_name.c_str()))
				break;
		}
		if(count==0)
			TiXmlError(filename, "missing element player0");
		players_count_ = count;
		// nová hra
		if(gameBase_) delete gameBase_;
		gameBase_ = new GameBase(
			players_count_, levels_[cur_level_].map);
		PlayerProperties prop;
		for(Uint16 i = 0 ; i < players_count_ ; ++i){
			el_name = "player"+x2string(i);
			el = subElement(rootEl, el_name.c_str());
			readAttr(el, "lives", prop.lives);
			readAttr(el, "flames", prop.flames);
			readAttr(el, "bombs", prop.bombs);
			readAttr(el, "boots", prop.boots);
			gameBase_->set_player(i, prop);
		}
	}
	catch(const std::string & err){
		TiXmlError(filename, err);
	}
}

void GameIntro::save_game(std::string & filename){
	// vytvorit dokument
	TiXmlDocument doc;
	doc.InsertEndChild(TiXmlDeclaration("1.0", "UTF-8", "yes"));
	// element game
	TiXmlNode *node;
	TiXmlElement *el, *rootEl;
	node = doc.InsertEndChild(TiXmlElement("game"));
	if(!node)
		TiXmlError("can't create root element");
	rootEl = node->ToElement();
	rootEl->SetAttribute("episode", cur_episode_);
	rootEl->SetAttribute("level", cur_level_);
	// elementy hracu
	std::string el_name;
	PlayerProperties prop;
	for(Uint16 i = 0 ; i < players_count_ ; ++i){
		el_name = "player"+x2string(i);
		node = rootEl->InsertEndChild(TiXmlElement(el_name.c_str()));
		if(!node)
			TiXmlError("can't create element "+el_name);
		el = node->ToElement();
		// atributy
		gameBase_->get_player(i, prop);
		el->SetAttribute("lives", prop.lives);
		el->SetAttribute("flames", prop.flames);
		el->SetAttribute("bombs", prop.bombs);
		el->SetAttribute("boots", prop.boots);
	}
	// ulozit do souboru
	try {
		TiXmlSaveDocument(doc, filename);
	}
	catch(const std::string & err){
		TiXmlError(filename, err);
	}
}

bool GameIntro::is_game(){
	return gameBase_!=0 && cur_level_ < levels_.size();
}

void GameIntro::load_levels_(Uint16 episode){
	string filename("levels");

	// nacteni hodnot z xml
	TiXmlDocument doc;
	TiXmlElement *el;
	try {
		el = TiXmlRootElement(doc, filename, "levels", false);
		el = el->FirstChildElement("episode");
		while(episode-- && el)
			el = el->NextSiblingElement("episode");
		if(!el)
			throw string("too few episodes");
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
			throw string("too few levels in episode");
	}
	catch(const string & err){
		TiXmlError(filename, err);
	}
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
		TiXmlError("levels", "Unable to load "+img_name);
	return image_.second;
}

