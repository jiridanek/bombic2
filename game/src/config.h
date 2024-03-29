/** @file config.h
 * Konfigurace hry.
 * Config.h obsahuje stejnojmennou třídu obstarávající konfiguraci hry
 * a jiné pomocné třídy.
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <map>
#include <vector>
#include <string>
#include <helpers/tixml.h>
#include <helpers/sdl.h>
#include <language.h>
#include <singleton.h>

/// Akce, které se dají vyvolat klávesami hráče.
enum KEY_ACTIONS { KEY_UP, KEY_RIGHT, KEY_DOWN, KEY_LEFT, KEY_PLANT, KEY_TIMER };
#define KEY_ACTIONS_COUNT 6

/** Pojmenování kláves.
 * Třída obstarávající převod mezi
 * klávesovými konstantami a názvy kláves.
 */
class KeyNames {
	public:
		/// Připravení jmen kláves.
		KeyNames();
		/// Převod jména na klávesu.
		SDLKey name2key(const std::string & name);
		/// Převod klávesy na jméno.
		const char* key2name(SDLKey key);
	private:
		typedef std::map< std::string, SDLKey> names_keys_t;
		names_keys_t names_keys_;

};

#define CONFIG_FILENAME "config.xml"
#define CONFIG_SPEED_MIN 1
#define CONFIG_SPEED_MAX 9

#define CONFIG SINGLETON_SHORTCUT(Config)

/** Konfigurace hry.
 * Instance třídy Config obstarává konfiguraci hry od jejího naloadování,
 * managování a ukládání.
 * Config může vytvořit pouze jedinou instanci,
 * která je typicky vytvořena při startu aplikace a zrušena při ukončení.
 * Implementuje nástroje,
 * kterými tuto skutečnost hlídá a poskytuje svůj pointer globálně,
 * prostřednictvím fce Config::get_instance().
 * Ve svých strukturách uchovává nastavení různých částí hry.
 * Přes jednoduchý interface toto nastavení poskytuje.
 * @warning Musí být inicializován až po window_init().
 */
class Config {
	friend class MenuOptions;
	friend class MenuOptionsControls;
	friend class BonusIllnessConfused;

	SINGLETON_DEFINITION(Config)

	public:

		/// Inicializace konfigu ze souboru.
		Config();
		/// Destruktor.
		~Config();
		/// Zvolený jazyk.
		const std::string & language() const { return language_; }
		/// Viditelná předpověď výbuchu.
		bool visible_presumption() const { return visible_presumption_; }
		/// Rozdělení obrazovky v multiplayeru.
		bool split_screen() const { return split_screen_; }
		/// Rychlost hry.
		Uint16 speed() const { return speed_; }
		/// Perioda pohybu.
		Uint16 move_period() const;
		/// Zapnutý/vypnutý celoobrazovkový režim.
		bool fullscreen() const { return fullscreen_; }
		// Zapnutý/vypnutý zvuk.
		bool sound() const { return sound_; }
		/// Nastavená klávesa.
		SDLKey player(Uint16 player_num, KEY_ACTIONS action) const;

	private:
		/// Typ seznamu hráčů a jejich kláves.
		typedef std::vector< std::vector< SDLKey > > players_t;
		/// Seznam kláves hráčů.
		players_t players_;
		/// True pokud se zmenily klávesy hráčů.
		bool players_changed_;

		typedef struct { std::string name, show; } language_t;
		std::vector<language_t> languages_;
		std::string language_;
		Language language_class_;

		Uint16 speed_;
		bool visible_presumption_, split_screen_, fullscreen_, sound_;

		/// Pro pojmenování kláves.
		KeyNames keyNames_;
		/// Načte konfiguraci z XML.
		void load_configuration_();
		/// Načte vlastnosti hry z XML.
		void load_properties_(TiXmlElement * rootEl);
		/// Načte klávesy hráčů z XML.
		void load_players_(TiXmlElement * rootEl);
		/// Nastaví klávesu akci.
		bool set_key_action_(Uint16 player_num, KEY_ACTIONS action,
			SDLKey key);
		/// Nastaví klávesu akci.
		bool set_key_action_(Uint16 player_num, KEY_ACTIONS action,
			SDLKey key, Uint16 & conflict_pl, KEY_ACTIONS & conflict_ac);

		/// Převede název akce na akci.
		KEY_ACTIONS name2action_(const std::string & name);
		/// Převede akci na její název.
		const char * action2name_(KEY_ACTIONS action);
		/// Uloží konfiguraci do souboru.
		void save_configuration_();
		/// Nastaví vlastnosti hry do XML.
		void set_properties_(TiXmlElement * rootEl);
		/// Nastaví klávesy hráčů do XML.
		void set_players_(TiXmlElement * rootEl);

};

#endif
