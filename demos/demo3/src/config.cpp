
#include <iostream>
#include <vector>
#include <string>
#include "SDL_lib.h"
#include "tixml_helper.h"
#include "stl_helper.h"
#include "constants.h"
#include "config.h"

using namespace std;

/*************** class Config ******************************/
Config* Config::myself_ptr_ = 0;

Config* Config::get_instance(){
	if(!myself_ptr_)
		throw string("in Config::get_instance(): no Config instance created.");
	return myself_ptr_;
}

/**
 * @param filename název souboru s konfigurací
 */
Config::Config() {
	if(myself_ptr_)
		throw string("in Config constructor: another Config instance created.");
	myself_ptr_ = this;
	try{
		// pripravit prostor pro ctyry hrace
		players_t::value_type empty_keys;
		empty_keys.insert(empty_keys.end(), KEY_ACTIONS_COUNT, SDLK_FIRST);
		players_.insert(players_.end(), 4, empty_keys);
		// naloadovat konfig z XML
		load_configuration_();
	}
	catch(...){
		myself_ptr_ = 0;
		throw;
	}
}

/** @details
 * Vynuluje myself_pointer_.
 */
Config::~Config(){
	myself_ptr_ = 0;
}

/**
 */
void Config::load_configuration_(){
	TiXmlDocument doc;
	TiXmlElement *root_el;
	string filename("config");
	// nacteni XML souboru
	root_el = TiXmlRootElement(doc, filename, "config", false);
	try{
		load_properties_(root_el);
		load_players_(root_el);
	}
	catch(const string & err){
		TiXmlError(filename, err);
	}
}

/**
 */
void Config::load_properties_(TiXmlElement * rootEl){
	readAttr(rootEl, "language", language_);
	// TODO check lang
	readAttr(rootEl, "visible_presumption", visible_presumption_);
	readAttr(rootEl, "speed", speed_);
	// TODO check speed
	readAttr(rootEl, "fullscreen", fullscreen_);
	readAttr(rootEl, "sound", sound_);
}

/**
 */
void Config::load_players_(TiXmlElement * rootEl){
	string el_name;
	for(Uint16 num=0 ; num<4 ; ++num ){
		el_name = "player"+x2string(num+1);
		// TODO invers pro char *SDL_GetKeyName(SDLKey key);
		/*
		// nacteni hrace
		wallsEl= wallsEl->FirstChildElement("wall");
		rootEl = TiXmlRootElement(doc, filename, "creature", true);
		// zdrojovy obrazek
		sur_src = load_src_surface_(rootEl);
		sur_src_s = load_src_surface_(rootEl, "shadow_src", false);

		// vyska a sirska obrazku
		readAttr(rootEl, "height", height);
		readAttr(rootEl, "width", width);
		*/
	}
}

/*************** END OF class Config ******************************/
