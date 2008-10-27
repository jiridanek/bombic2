
#include <iostream>
#include <vector>
#include <string>
#include "SDL_lib.h"
#include "tixml_helper.h"
// #include "stl_helper.h"
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
 * @param players_count počet hráčů
 * @param mapname název mapy
 */
Config::Config(const std::string & filename) {
	if(myself_ptr_)
		throw string("in Config constructor: another Config instance created.");
	myself_ptr_ = this;
	try{
		// pripravit prostor pro ctyry hrace
		players_t::value_type empty_keys;
		empty_keys.insert(empty_keys.end(), KEY_ACTIONS_COUNT, SDLK_FIRST);
		players_.insert(players_.end(), 4, empty_keys);
		// naloadovat konfig z XML
		// load_configuration_();
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


/*************** END OF class Config ******************************/
