
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
	if(speed_ < 1 || speed_ > CONFIG_SPEED_MAX)
		throw "attribute speed must be integer between 1 and "+ x2string(CONFIG_SPEED_MAX);
	readAttr(rootEl, "fullscreen", fullscreen_);
	readAttr(rootEl, "sound", sound_);
}

/**
 */
void Config::load_players_(TiXmlElement * rootEl){
	string el_name, key_name, action_name;
	TiXmlElement * playerEl;
	for(Uint16 num=0 ; num<players_.size() ; ++num ){
		el_name = "player"+x2string(num);
		playerEl = subElement(rootEl, el_name.c_str());
		playerEl = subElement(playerEl, "bind");
		while(playerEl){
			readAttr(playerEl, "action", action_name);
			readAttr(playerEl, "key", key_name);
			set_key_action_(num, name2action_(action_name),
					keyNames_.name2key(key_name));
			playerEl = playerEl->NextSiblingElement("bind");
		}
	}
}

/**
 */
void Config::set_key_action_(Uint16 player_num, KEY_ACTIONS action, SDLKey key){
	if( static_cast<Uint16>(action) >= players_[player_num].size()
	|| key >= SDLK_LAST )
		return;
	players_[player_num][action] = key;
}

/**
 */
KEY_ACTIONS Config::name2action_(const std::string & name){
	if(name=="up") return KEY_UP;
	if(name=="right") return KEY_RIGHT;
	if(name=="down") return KEY_DOWN;
	if(name=="left") return KEY_LEFT;
	if(name=="plant") return KEY_PLANT;
	if(name=="timer") return KEY_TIMER;
	throw string("Config::name2action(): unhandled key_action");
	return KEY_TIMER;
}


SDLKey Config::player(Uint16 player_num, KEY_ACTIONS action) const {
	if(static_cast<Uint16>(action) >= players_[player_num].size() )
		return SDLK_FIRST;
	return players_[player_num][action];
}

Uint16 Config::move_period() const {
	return MOVE_PERIOD + CONFIG_SPEED_MAX/2 - speed_;
}

/*************** END OF class Config ******************************/

/********************** class KeyNames ****************************/

KeyNames::KeyNames(){
	SDLKey key;
	for(key = SDLK_FIRST ; key != SDLK_LAST;
				key = static_cast<SDLKey>(key+1)){
		names_keys_[SDL_GetKeyName(key)] = key;
	}
	names_keys_.erase("unknown key");
}

SDLKey KeyNames::name2key(const std::string & name){
	names_keys_t::iterator it = names_keys_.find(name);
	if(it==names_keys_.end())
		return SDLK_FIRST;
	return it->second;
}

const char* KeyNames::key2name(SDLKey key){
	return SDL_GetKeyName(key);
}


/*************** END OF class KeyNames ****************************/

