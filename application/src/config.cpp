
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

/**
 * @return Pointer na jedinou instanci třídy Config.
 * @throw string Chybová hláška, pokud není zatím vytvořena instance třídy Config.
 */
Config* Config::get_instance(){
	if(!myself_ptr_)
		throw string("in Config::get_instance(): no Config instance created.");
	return myself_ptr_;
}

/** @details
 * Inicializuje klávesy hráčů,
 * nahrává konfiguraci hry.
 * @throw string Chybová hláška, pokud již je instance třídy Config vytvořena.
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
 * Zruší instanci třídy Config.
 */
Config::~Config(){
	myself_ptr_ = 0;
}

/** @details
 * Načte konfiguraci ze souboru CONFIG_FILENAME.
 */
void Config::load_configuration_(){
	TiXmlDocument doc;
	TiXmlElement *root_el;
	string filename(CONFIG_FILENAME);
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

/** @details
 * Načte z XML souboru nastavení hry.
 * Konkrétně načítá:
 *	* Rychlost hry
 *	* Odhad exploze
 *	* Rozdělit obrazovku
 *	* Celoobrazovkový mód
 *	* Povolit zvuk
 *	* Jazyk
 *
 * @param rootEl root element xml souboru
 * @throw string Chybové hlášení pokud není rychlost ve stanovených mezích.
 */
void Config::load_properties_(TiXmlElement * rootEl){
	readAttr(rootEl, "speed", speed_);
	if(speed_ < CONFIG_SPEED_MIN || speed_ > CONFIG_SPEED_MAX)
		throw "attribute speed must be integer between "+
			x2string(CONFIG_SPEED_MIN)+
			" and "+ x2string(CONFIG_SPEED_MAX);
	readAttr(rootEl, "visible_presumption", visible_presumption_);
	readAttr(rootEl, "split_screen", split_screen_);
	readAttr(rootEl, "fullscreen", fullscreen_);
	readAttr(rootEl, "sound", sound_);
	rootEl = subElement(rootEl, "languages");
	readAttr(rootEl, "default", language_);
	LANG->set_language_(language_);
	language_t lang;
	for(rootEl = subElement(rootEl, "language"); rootEl!=0 ;
			rootEl = rootEl->NextSiblingElement("language") ){
		readAttr(rootEl, "name", lang.name);
		readAttr(rootEl, "show", lang.show);
		languages_.push_back(lang);
	}
}

/** @details
 * Načte z XML souboru nastavení kláves hráčů.
 * @param rootEl root element xml souboru
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
			if(!set_key_action_(num, name2action_(action_name),
					keyNames_.name2key(key_name)))
				cerr << "Warning: in config file conflicts key for action: "
					<< el_name << ", " << action_name << endl;
			playerEl = playerEl->NextSiblingElement("bind");
		}
	}
	players_changed_ = false;
}

/** @details
 * Nastaví klávesu pro akci hráče.
 * @param player_num číslo hráče, jehož nastavení měníme
 * @param action akce, jíž přiřazujeme klávesu
 * @param key klávesa
 * @return True pokud nedošlo ke konfliktu, jinak false.
 * @see set_key_action_()
 */
bool Config::set_key_action_(Uint16 player_num, KEY_ACTIONS action,
			SDLKey key){
	Uint16 pl; KEY_ACTIONS ac;
	return set_key_action_(player_num, action, key, pl, ac);
}

/** @details
 * Nastaví klávesu pro akci hráče.
 * Kontroluje jestli došlo ke konfliktu s jiným hráčem,
 * pokud ano, konfliktní hráč přijde o své nastavení.
 * Nastavovaná akce dostane vždy svou klávesu výhradně.
 * @param player_num číslo hráče, jehož nastavení měníme
 * @param action akce, jíž přiřazujeme klávesu
 * @param key klávesa
 * @param conflict_pl číslo hráče, který je v konfliktu - výstupní parametr
 * @param conflict_ac akce hráče, která je v konfliktu - výstupní parametr
 * @return True pokud nedošlo ke konfliktu, jinak false.
 */
bool Config::set_key_action_(Uint16 player_num, KEY_ACTIONS action,
			SDLKey key, Uint16 & conflict_pl, KEY_ACTIONS & conflict_ac){
	players_changed_ = true;
	for(Uint16 pl=0 ; pl<players_.size() ; ++pl){
		for(Uint16 ac=0 ; ac<KEY_ACTIONS_COUNT ; ++ac){
			if(players_[pl][ac] == key){
				players_[pl][ac] = SDLK_FIRST;
				conflict_pl = pl;
				conflict_ac = static_cast<KEY_ACTIONS>(ac);
				players_[player_num][action] = key;
				return false;
			}
		}
	}
	players_[player_num][action] = key;
	return true;
}

/** @details
 * @param name název akce
 * @return Akce příslušící zadanému názvu.
 * @throw string Chybová hláška, pokud názvu neodpovídá žádná akce.
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

/** @details
 * @param action akce
 * @return Název příslušící zadané akci.
 * @throw string Chybová hláška, pokud akci neumíme přiřadit název.
 */
const char * Config::action2name_(KEY_ACTIONS action){
	switch(action){
		case KEY_UP: return "up";
		case KEY_RIGHT: return "right";
		case KEY_DOWN: return "down";
		case KEY_LEFT: return "left";
		case KEY_PLANT: return "plant";
		case KEY_TIMER: return "timer";
		default:
			throw string("Config::action2name_(): unhandled key_action");
			return 0;
	}
}

/**
 * @param player_num číslo hráče
 * @param action akce
 * @return Klávesa přiřazená akci hráče.
 */
SDLKey Config::player(Uint16 player_num, KEY_ACTIONS action) const {
	return players_[player_num][action];
}

/**
 * @return Perioda vypočtená z konstantně přednastavené rychlosti
 * a odchylky rychlosti z dynamické konfigurace.
 */
Uint16 Config::move_period() const {
	return MOVE_PERIOD - speed_ +
		( CONFIG_SPEED_MIN + CONFIG_SPEED_MAX )/2;
}

/** @details
 * Uloží konfiguraci do souboru CONFIG_FILENAME.
 * @see set_properties_(), set_players_()
 */
void Config::save_configuration_(){
	TiXmlDocument doc;
	TiXmlElement *root_el;
	string filename(CONFIG_FILENAME);
	// nacteni XML souboru
	root_el = TiXmlRootElement(doc, filename, "config", false);
	try{
		set_properties_(root_el);
		set_players_(root_el);
		filename = CONFIG_FILENAME;
		TiXmlSaveDocument(doc, filename);
	}
	catch(const string & err){
		TiXmlError(filename, err);
	}
}

/** @details
 * Uloží do XML nastavení hry.
 * Konkrétně ukládá:
 *	* Rychlost hry
 *	* Odhad exploze
 *	* Rozdělit obrazovku
 *	* Celoobrazovkový mód
 *	* Povolit zvuk
 *	* Jazyk
 *
 * @param rootEl root element xml souboru
 */
void Config::set_properties_(TiXmlElement * rootEl){
	rootEl->SetAttribute("speed", speed_);
	rootEl->SetAttribute("visible_presumption", visible_presumption_ ? 1 : 0);
	rootEl->SetAttribute("split_screen", split_screen_ ? 1 : 0);
	rootEl->SetAttribute("fullscreen", fullscreen_ ? 1 : 0);
	rootEl->SetAttribute("sound", sound_ ? 1 : 0);

	rootEl = subElement(rootEl, "languages");
	rootEl->SetAttribute("default", language_.c_str());
}

/** @details
 * Uloží do XML nastavení kláves hráčů.
 * @param rootEl root element xml souboru
 */
void Config::set_players_(TiXmlElement * rootEl){
	if(!players_changed_) return;
	string el_name;
	TiXmlElement * playerEl;
	TiXmlElement new_bind("bind");
	for(Uint16 player_num=0 ; player_num<players_.size() ; ++player_num ){
		el_name = "player"+x2string(player_num);
		playerEl = subElement(rootEl, el_name.c_str());
		playerEl->Clear();
		for(Uint16 action_num=0 ;
				action_num<players_[player_num].size() ;
				++action_num){
			new_bind.SetAttribute("action", action2name_(
				static_cast<KEY_ACTIONS>(action_num) ));
			new_bind.SetAttribute("key", keyNames_.key2name(
				players_[player_num][action_num]) );
			playerEl->InsertEndChild(new_bind);
		}
	}
	players_changed_ = false;
}

/*************** END OF class Config ******************************/

/********************** class KeyNames ****************************/

/** @details
 * Inicializuje mapu kláves.
 */
KeyNames::KeyNames(){
	SDLKey key;
	for(key = SDLK_FIRST ; key != SDLK_LAST;
				key = static_cast<SDLKey>(key+1)){
		names_keys_[SDL_GetKeyName(key)] = key;
	}
	names_keys_.erase("unknown key");
}

/**
 * @param name název klávesy
 * @return Klávesa odpovídající názvu,
 * nebo SDLK_FIRST když taková klávesa neexistuje.
 */
SDLKey KeyNames::name2key(const std::string & name){
	names_keys_t::iterator it = names_keys_.find(name);
	if(it==names_keys_.end())
		return SDLK_FIRST;
	return it->second;
}

/**
 * @param key klávesa
 * @return Název zadané klávesy.
 */
const char* KeyNames::key2name(SDLKey key){
	return SDL_GetKeyName(key);
}


/*************** END OF class KeyNames ****************************/

