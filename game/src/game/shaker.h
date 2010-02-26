/** @file game_shaker.h
 * Třesení hrou.
 * Game_shaker.h obsahuje třídu obstarávající třesení hrou.
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
 */
class GameShaker {
	public:
		/// Inicializace.
		GameShaker();
		/// Obnovení souřadnic.
		void update();
		/// Zatřesení hrou.
		void shake(Uint16 delay);
		/// Souřadnice posunutí pohledu.
		Sint16 getDiffX();
		/// Souřadnice posunutí pohledu.
		Sint16 getDiffY();
		/// Zda-li se aktuálně má třást.
		bool isShaking();
	private:
		/// Doba, po kterou se ještě bude třást.
		Uint16 delay_;
		/// Souřadnice posunutí.
		Sint16 diff_x_;
		/// Souřadnice posunutí.
		Sint16 diff_y_;

		/// Výpočet nové souřadnice posunutí.
		Sint16 getNewDiff();
};

#endif
