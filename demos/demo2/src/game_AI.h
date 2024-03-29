/** @file game_AI.h
 * Umělá inteligence.
 * Game_AI.h obsahuje třídy obstarávající umělou inteligenci.
 */
#ifndef GAME_AI_H
#define GAME_AI_H

#include <iostream>
#include <vector>
#include "SDL_lib.h"
#include "game.h"
#include "game_mapobjects.h"
#include "game_creature.h"

/** Umělá inteligence.
 * AI je abstraktní třída, která implementuje vlastnosti a metody
 * společné pro většinu úrovní umělých inteligencí.
 * systém úrovní umělých inteligencí není úplně plochý,
 * každá úroveň implementuje stěžejní funkce pro rozhodnutí tahu trochu jinak.
 */
class AI {
	public:
		/// Vytvoří umělou inteligenci.
		static AI* new_ai(Creature *creature, Uint16 intelligence);
		/// Uloží rodičovskou nestvůru.
		AI(Creature *creature);
		/// Hýbne nestvůrou.
		virtual void move()=0;
		/// Pomocná struktura pro otočení a souřadnice při posunu.
		typedef struct{ DIRECTION d; Uint16 x, y; } position_t;
		/// Destruktor.
		virtual ~AI() {};
	protected:
		/// Nestvůra, které inteligence patří.
		Creature *creature_;
		/// Seznam pozic pro každý krok.
		std::vector<position_t > positions_;
		/// Uloží cílové pozice pro každé otočení.
		void updatePositions();
		/// Nastaví nestvůře novou pozici.
		void setPosition(position_t & position);
		/// Partikulárně centruje souřadnici dostředu políčka.
		void AI::centerCoordinate(Uint16 & coordinate, Sint8 sign);
};

/** Umělá inteligence nulté úrovně.
 * AI_0 je naprosto tupá úroveň.
 * Jde pořád rovně, když narazí, náhodně se rozhodne kudy dál.
 * Vůbec neřeší blížící se výbuch nebo dostižení hráče.
 */
class AI_0 : public AI {
	public:
		/// Zavolá konstruktor AI
		AI_0(Creature *creature);
		/// Hýbne nestvůrou.
		virtual void move();
		/// Destruktor.
		virtual ~AI_0() {};
	private:
		/// Zjistí, zda je možné vstoupit na políčko.
		bool checkfield_(const position_t & position);
};

/** Umělá inteligence první úrovně.
 * AI_1 je velice tupá úroveň, sice nepředvídatelně mění směr,
 * nicméně vůbec neřeší blížící se výbuch nebo cílené dostižení hráče.
 */
class AI_1 : public AI {
	public:
		/// Zavolá konstruktor AI
		AI_1(Creature *creature);
		/// Hýbne nestvůrou.
		virtual void move();
		/// Destruktor.
		virtual ~AI_1() {};
	private:
		/// Zjistí, zda je možné vstoupit na políčko.
		bool checkfield_(const position_t & position);
};

/** NEumělá inteligence.
 * AI_fromKeayboard je inteligence, která je řízena vstupem z klávesnice.
 * Pokud se nic neděje, nestvůrou (hráčem) nehýbá.
 * Řeší, aby uživatel svou interakcí chodil pouze tudy, kudy může.
 */
class AI_fromKeyboard : public AI {
	public:
		/// Zavolá konstruktor AI
		AI_fromKeyboard(Creature *creature);
		/// Hýbne nestvůrou.
		virtual void move();
		/// Destruktor.
		virtual ~AI_fromKeyboard() {};
	private:
		/// Stav klávesnice.
		Uint8 *keystate_;
};


#endif
