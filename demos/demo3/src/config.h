/** @file config.h
 * Konfigurace hry.
 * Config.h obsahuje stejnojmennou třídu obstarávající konfíguraci hry.
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <iostream>
// #include <list>
#include <vector>
#include <string>
// #include <map>
#include "SDL_lib.h"
// #include "game_base.h"
// #include "game_mapobjects.h"
#include "tixml_helper.h"

/// Akce, které se dají vyvolat klávesami hráče.
enum KEY_ACTIONS { KEY_UP, KEY_RIGHT, KEY_DOWN, KEY_LEFT, KEY_PLANT, KEY_TIMER };
#define KEY_ACTIONS_COUNT 6

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
 */
class Config {
	public:
		/// Získat pointer na jedinou instanci třídy.
		static Config* get_instance();

		/// Inicializace konfigu ze souboru.
		Config(const std::string & filename);
		/// Destruktor.
		~Config();
		/// Kód zvoleného jazyka.
		const std::string & language() const { return language_; }
		/// Viditelná předpověď výbuchu.
		bool visible_presumtion() const { return visible_presumption_; }
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
		/// Pointer na jedinou instanci třídy.
		static Config* myself_ptr_;
		/// Typ seznamu hráčů a jejich kláves.
		typedef std::vector< std::vector< SDLKey > > players_t;
		/// Seznam kláves hráčů.
		players_t players_;
		std::string language_;
		Uint16 speed_;
		bool sound_, fullscreen_, visible_presumption_;


};

#endif
