/** @file shaker.h
 * Třesení hrou.
 * Obsahuje třídu GameShaker obstarávající třesení hrou.
 */

#ifndef GAME_SHAKER_H
#define GAME_SHAKER_H

#include <helpers/sdl.h>

/// Maximální počet pixelů, o které můžeme pohled posunout (do čtyř stran).
#define GAME_SHAKER_MAX_DIFF 4
/// Intenzita třesení.
#define GAME_SHAKER_VOLUME 6

/** Třesení hrou.
 * Třída GameShaker poskytuje metodu, jak může hra (Game)
 * kdykoli získat souřadnice posunutí pohledu hráče,
 * často (ale pravidelně) tyto souřadnice měnit,
 * a tím docílit efektu třesení hry.
 *
 * Tento efekt je buď spuštěn jednorázově po nějakou (krátkou) dobu,
 * nebo je spuštěn až do následného zastavení. Tyto metody se mezi sebou mohou
 * kombinovat.
 */
class GameShaker {
	public:
		/// Inicializace.
		GameShaker();
		/// Obnovení souřadnic.
		void update();
		/// Jednorázové zatřesení hrou.
		void shake(Uint16 delay);
		/// Spuštění třesení.
		void start();
		/// Zastavení třesení.
		void stop();
		/// Souřadnice posunutí pohledu.
		Sint16 getDiffX();
		/// Souřadnice posunutí pohledu.
		Sint16 getDiffY();
		/// Zda-li se aktuálně má třást.
		bool isShaking();
	private:
		/// Doba, po kterou se ještě bude třást.
		Uint16 delay_;
		/// Jestli je dlouhodobé třesení spuštěno.
		bool started_;
		/// Souřadnice posunutí.
		Sint16 diff_x_;
		/// Souřadnice posunutí.
		Sint16 diff_y_;

		/// Výpočet nové souřadnice posunutí.
		Sint16 getNewDiff();
};

#endif
