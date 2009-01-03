/** @file game_intro.h
 * Obstarání hry.
 * Game_intro.h obsahuje třídy obstarávající hru v epizodách.
 * Postup vytvoření hry je následující:
 * pomocí GameBase se jednou naloadují struktury z xml souborů,
 * pomocí Game se z této předpřipravené struktury vygeneruje hra a její mapa.
 * Game obstarává dobu od narození po úmrtí hráče nebo jeho vítězství,
 * poté se vracíme do GameIntro abychom rozhodli co dál a vygenerovali novou mapu.
 */

#ifndef GAME_INTRO_H
#define GAME_INTRO_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include "SDL_lib.h"
#include "game_base.h"
#include "game_tools.h"
#include "game.h"
#include "game_player.h"

#define GAME_INTRO_PADDING 10
/** Obal pro samotnou hru.
 * Třída GameIntro řeší, jaké kolo se bude hrát, kdo vyhrál či prohrál
 * a zastřešuje tvorbu mapy z XML souborů až po vygenerování konkrétního levelu.
 * Tvoří rozhraní mezi menu a hrou. Udržuje informace o rezehraném kole,
 * stavu hráčů atd.
 * @see GameBase, GameTools, Game.
 */
class GameIntro {
	public:
		/// Bez inicializace.
		GameIntro();
		/// Destrukce Game a GameBase.
		~GameIntro();
		/// Zobrazení intro screen bez inicializace.
		void show_screen();
		/// Inicializace nové hry.
		void new_game(Uint16 episode, Uint16 players);
		/// Inicializace uložené hry.
		void load_game(std::string & filename);
		/// Uložení rozehrané hry.
		void save_game(std::string & filename);
		/// Je vytvořená hra.
		bool is_game();
	private:
		/// Základ pro konkrétní level.
		GameBase * gameBase_;
		/// Nadstavbové věci pro hru.
		GameTools * gameTools_;
		/// Konkrétní hra.
		Game * game_;

		/// Vykreslení intro obrazovky.
		void draw_(SDL_Surface * window, Uint16 level, bool after=false);
		/// Vyčkání na odmáčknutí intro obrazovky.
		bool wait_(SDL_Surface * window, Uint16 level, bool after=false);

		/// Načtení levelů.
		void load_levels_(Uint16 episode);
		/// Získání obrázku.
		Surface & get_cur_image_(Uint16 level, bool after=false);

		/// Mapa a obrázek levelu.
		typedef struct{ std::string map, img, img_after, text; } level_t;
		/// Seznam levelů.
		std::vector< level_t > levels_;

		typedef std::pair< std::string, Surface> image_t;
		/// Surface příslušící jménu souboru.
		image_t image_;

		Uint16 cur_episode_, ///< Aktuální epizoda.
			cur_level_, ///< Aktuální level.
			players_count_; ///< Počet hráčů.
};

#endif
