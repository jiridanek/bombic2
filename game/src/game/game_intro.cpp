
#include <string>
#include <helpers/agar.h>
#include <helpers/sdl.h>
#include <helpers/tixml.h>
#include <constants.h>
#include <language.h>
#include "game_intro.h"
#include "base.h"
#include "tools.h"
#include "game.h"
#include "player.h"

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
extern Fonts * g_font;

/** @details
 * Ukáže úvodní obrazovku levelu, vygeneruje konkrétní hru,
 * čeká na vstup od uživatele, spustí hru.
 * Po skončení hry ukáže podle jejího výsledku obrazovku smrti
 * nebo obrazovku po levelu (je-li).
 * Po uživatelově vstupu spustí hru znovu, nebo přechází do dalšího levelu.
 * Při neúspěšném čekání na uživatelův vstup (zavření okna, typicky stisk ESC)
 * ukončí základní smyčku.
 * @throw string Chybové hlášení pokud není inicializována hra.
 * @see new_game(), load_game(), wait_();
 */
void GameIntro::show_screen(){
	if(!gameTools_)
		throw string("in GameIntro::show_screen(): no gameTools created");

	Uint16 old_level = cur_level_;
	while(cur_level_ < levels_.size()){
		// uvodni obrazovka levelu
		draw_(g_window, cur_level_, level_image_t::BEFORE);
		// vygenerovani noveho levelu
		if(!gameBase_ || game_){
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

		if(!wait_(g_window, cur_level_, level_image_t::BEFORE))
			return;

		// vygenerovani nove hry z pripraveneho zakladu
		if(game_) delete game_;
		game_ = new Game(*gameBase_, gameTools_);

		// hrajeme
		game_->play(g_window);
		// vysledek hry
		if(game_->success()) {
			// hra skoncila uspesne => dalsi kolo
			++cur_level_;
		} else {
			delete game_;
			game_ = 0;
		}
		// koncova obrazovka levelu
		if(old_level!=cur_level_){
			if(!levels_[old_level].img_after.empty()){
				draw_(g_window, old_level, level_image_t::AFTER);
				if(!wait_(g_window, old_level, level_image_t::AFTER))
					return;
			}
			old_level = cur_level_;
		} else {
			// level se nezmenil => hrac umrel
			draw_(g_window, cur_level_, level_image_t::DEATH);
			if(!wait_(g_window, cur_level_, level_image_t::DEATH))
				return;
		}
	}
	delete game_;
	game_ = 0;
}

void GameIntro::draw_(SDL_Surface * window, Uint16 level, level_image_t::purpose purpose){
	// pozadi
	clear_surface(Color::black, window);
	// obrazek levelu
	Surface sur = get_cur_image_(level, purpose);
	int x = (window->w-sur.width())/2, y = (window->h-sur.height())/2;
	draw_surface(x, y, sur.getSurface(), window);
	// stin textu
	const char * text = 0;
	switch(purpose){
		case level_image_t::BEFORE:
			text = LANG_GAME(levels_[level].text.c_str(), LANG_BEFORE_LEVEL);
			break;
		case level_image_t::AFTER:
			text = LANG_GAME(levels_[level].text.c_str(), LANG_AFTER_LEVEL);
			break;
		case level_image_t::DEATH:
			text = LANG_GAME_DEATH(cur_episode_death_text_.c_str());
			break;
		default:
			throw string("unhandled level_image_t::purpose");
	}
	sur = get_multiline_text((*g_font)[17], text, Color::black);
	draw_surface(x +GAME_INTRO_PADDING+1, y +GAME_INTRO_PADDING+1,
		sur.getSurface(), window);
	// text levelu
	sur = get_multiline_text((*g_font)[17], text, Color::white);
	draw_surface(x +GAME_INTRO_PADDING, y +GAME_INTRO_PADDING,
		sur.getSurface(), window);
	// zobrazit v okne
	SDL_Flip(window);
}

bool GameIntro::wait_(SDL_Surface * window, Uint16 level, level_image_t::purpose purpose){
	// pockame na klavesu, pri pokusu o ukonceni ukoncime
	SDLKey key;
	while(true){
		switch(wait_event(key)){
			case SDL_VIDEORESIZE:
				// uvodni obrazovka levelu
				draw_(g_window, level, purpose);
				continue;
			case SDL_QUIT:
				AG_Quit();
				return false;
			case SDL_KEYUP:
				if(key==SDLK_ESCAPE)
					return false;
				if(key==SDLK_RETURN || key==SDLK_KP_ENTER
				|| key==SDLK_SPACE || key==SDLK_RCTRL)
					return true;
			default:
				continue;
		}
	}
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
	if(game_){
		delete game_;
		game_ = 0;
	}
}

/// Inicializace.
void GameIntro::load_game(std::string & filename){
	if(game_){
		delete game_;
		game_ = 0;
	}

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
		// najdu element pozadovane epizody
		while(episode-- && el)
			el = el->NextSiblingElement("episode");
		if(!el)
			throw string("too few episodes");
		// ulozi klicovy text pro epizodu
		readAttr(el, "death_text", cur_episode_death_text_);
		// ulozeni vsech levelu do seznamu
		levels_.clear();
		level_t level;
		for(el = el->FirstChildElement("level") ; el ;
					el = el->NextSiblingElement("level")){
			readAttr(el, "map", level.map);
			readAttr(el, "img", level.img_before);
			readAttr(el, "death", level.img_death);
			if(!readAttr(el, "after", level.img_after, false))
				level.img_after = "";
			readAttr(el, "text", level.text);
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

Surface & GameIntro::get_cur_image_(Uint16 level,
		level_image_t::purpose purpose){
	string img_name;
	switch(purpose){
		case level_image_t::BEFORE:
			img_name = levels_[level].img_before;
			break;
		case level_image_t::AFTER:
			img_name = levels_[level].img_after;
			break;
		case level_image_t::DEATH:
			img_name = levels_[level].img_death;
			break;
		default:
			throw string("unhandled level_image_t::purpose");
	}
	if(level_image_.name == img_name)
		return level_image_.sur;

	// zahodit stare jmeno
	level_image_.name.clear();
	// odalokovat
	level_image_.sur = 0;
	// naalokovat novy
	if(!locate_file("", img_name, img_name))
		TiXmlError("levels", "Can't locate image file "+img_name);
	level_image_.sur = IMG_Load(img_name.c_str());
	if(!level_image_.sur.getSurface())
		TiXmlError("levels", "Unable to load "+img_name);
	// v image_ uz mame platny obrazek, vlozime tam i jeho jmeno
	level_image_.name = img_name;
	return level_image_.sur;
}

