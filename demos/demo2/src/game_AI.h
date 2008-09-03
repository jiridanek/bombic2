/** @file game_AI.h
 * Umělá inteligence.
 * Game_AI.h obsahuje třídu obstarávající umělou inteligenci.
 */
#ifndef GAME_AI_H
#define GAME_AI_H

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
/*
class Crature
{

	void update()
	{
		// ......
		if (m_AI != NULL) m_AI->update();
	}
	void set_AI(AI *ai);

private:
	AI *m_AI; // delete in destructor
};



class AI
{
	void init(Creature *creature);
	virtual void update();

private:
	Crature *m_creature;

};


class AI_housenka : public AI
{
	virtual void update();
};


Creature housenka;
housenka.set_AI(new AI_housenka);
*/


class AI {
	public:
		/// Nastaví nové hodnoty souřadnicím.
		static DIRECTION step(int x, int y, DIRECTION direction, Uint16 speed, Uint16 ai);
		/// Inteligence ovládaná klávesnicí. TODO
		static DIRECTION from_keyboard(Uint16 & x, Uint16 & y, Uint16 speed,
			SDLKey up, SDLKey right, SDLKey down, SDLKey left);
		/// Umělá inteligence vyspělosti 0.
		static DIRECTION ai0();
		/// Umělá inteligence vyspělosti 1.
		static DIRECTION ai1();
		/// Pomocná funkce pro ai1, zjištuje, jestli se můžeme posunout daným směrem.
		static bool ai1_checkfield(Uint16 i);
		/// Inicializace proměnných.
		static void initialize(int x, int y, DIRECTION direction, Uint16 speed);
		/// Pomocná struktura pro otočení a souřadnice při posunu.
		typedef struct{ DIRECTION d; Uint16 x, y; } Direction_XY;
	private:
		/// Souřadnice v políčkách mapy.
		static Uint16 x_, y_;
		/// Uložení směru a souřadnic pro každé otočení.
		static std::vector<Direction_XY > move_;
		/// Stav klávesnice.
		static Uint8 *keystate_;
};

#endif
