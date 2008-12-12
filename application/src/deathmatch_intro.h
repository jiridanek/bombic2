/** @file deathmatch_intro.h
 * Obstarání deathmatche.
 * Deathmatch_intro.h obsahuje třídy obstarávající hru v deathmatchi.
 * Postup vytvoření hry je následující:
 * pomocí GameBase se jednou naloadují struktury z xml souborů,
 * pomocí Game se z této předpřipravené struktury vygeneruje hra a její mapa.
 * Game obstarává dobu od narození po úmrtí hráče nebo jeho vítězství,
 * poté se vracíme do DeathmatchIntro abychom udělili body a vygenerovali novou hru.
 */

#ifndef DEATHMATCH_INTRO_H
#define DEATHMATCH_INTRO_H

#include <string>
#include <vector>
#include "SDL_lib.h"
#include "game_base.h"
#include "game_tools.h"
#include "game.h"

/** Obal pro samotnou hru.
 * Třída DeathmatchIntro řeší, udělování bodů, kdo vyhrál či prohrál
 * a zastřešuje tvorbu mapy z XML souborů až po vygenerování konkrétního levelu.
 * Tvoří rozhraní mezi menu a deathmatchem. Udržuje informace o rozehraném matchi,
 * skóre hráčů atd.
 * @see GameBase, GameTools, Game.
 */
class DeathmatchIntro: public GameBaseLoader {
	public:
		/// Typ seznamu jmen bonusů.
		typedef GameBase::bonuses_t bonuses_t;

		/// Bez inicializace.
		DeathmatchIntro(Uint16 players_count,
			const std::string & mapname, const bonuses_t & bonuses,
			Uint16 wins, bool creatures, bool bombsatend);
		/// Destrukce Game a GameBase.
		~DeathmatchIntro();
		/// Zobrazení intro screen.
		void show_screen();

	private:
		/// Základ pro konkrétní level.
		GameBase * gameBase_;
		/// Nadstavbové věci pro hru.
		GameTools * gameTools_;
		/// Konkrétní hra.
		Game * game_;

		/// Počet vítězství pro vyhrání matche.
		Uint16 wins_;
		/// Násilné ukončení matche.
		bool bombsatend_;
		/// Skóre hráčů.
		std::vector<Uint16> score_;

		/// Načte z XML věci pro deathmatch.
		void load_deathmatch_tools_(Uint16 players_count);
		void load_tool_(TiXmlElement * rootEl,
			const char* subEl_name, const Surface & sur_src);
		/// Věci pro kreslení.
		std::vector<Animation> tools_;
		/// Získání obrázku.
		Surface & get_image_(Uint8 index);
		/// Jména souborů s pozadím.
		std::vector<std::string> intro_;
		/// Dvojice jméno grafického souboru a surface z něj loadovaný.
		typedef std::pair< std::string, Surface> image_t;
		/// Surface příslušící jménu souboru.
		image_t image_;

};

#endif
