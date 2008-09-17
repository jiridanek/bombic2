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
 * Třída je postavena na tom, že objekt stojící na nějakém políčku v nějaké mapě
 * využívající nějakou inteligenci potřebuje v jednom kroku vědět (podle své inteligence)
 * nové souřadnice. Na ty se zeptá své inteligence.
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

class AI_fromKeyboard : public AI {
	public:
		/// Zavolá konstruktor AI
		AI_fromKeyboard(Creature *creature);
		/// Hýbne nestvůrou.
		virtual void move();
		/// Destruktor.
		virtual ~AI_fromKeyboard() {};
	private:
		Uint8 *keystate_;
};


#endif
