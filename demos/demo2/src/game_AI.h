/** @file game_AI.h
 * Umělá inteligence.
 * Game_AI.h obsahuje třídu obstarávající umělou inteligenci.
 */
#ifndef GAME_AI_
#define GAME_AI_

#include <iostream>
#include <vector>
#include "SDL_lib.h"
#include "game.h"
#include "game_mapobjects.h"

/** Umělá inteligence.
 * Třída je postavena na tom, že objekt stojící na nějakém políčku v nějaké mapě
 * využívající nějakou inteligenci potřebuje v jednom kroku vědět (podle své inteligence)
 * nové souřadnice. Na ty se zeptá své inteligence.
 */
class AI {
	public:
		/// Nastaví nové hodnoty souřadnicím.
		static DIRECTION step(int x, int y, DIRECTION direction, Uint8 ai);
		/// Umělá inteligence vyspělosti 0.
		static DIRECTION ai0();
		/// Umělá inteligence vyspělosti 1.
		static DIRECTION ai1();
		/// Inicializace move_;
		static void initialize();
		/// Pomocná struktura pro otočení a souřadnice při posunu.
		typedef struct{ DIRECTION d; Uint16 x, y; } Direction_XY;
	private:
		/// Souřadnice v políčkách mapy.
		static Uint16 x_, y_;
		/// Uložení směru a souřadnic pro každé otočení.
		static std::vector<Direction_XY > move_;
};

#endif
