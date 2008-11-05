/** @file game_bonus_application.h
 * Aplikace bonusu na hráče.
 * Game_bonus_application.h obsahuje třídy obstarávající aplikaci bonusu na hráče.
 * Pro přidání bonusu s jménem NAME je třeba udělat následující kroky:
 *	* přidat třídu BonusNAME
 *	* implementovat BonusNAME::BonusNAME(Player * player)
 *	* implementovat BonusNAME::name() { return "bonus_NAME"; }
 *	* do BonusApplication::new_bonus() přidat větev s BonusNAME
 *	* přidat do třídy Player třídu BonusNAME jako friend
 */
#ifndef GAME_BONUS_APPLICATION_H
#define GAME_BONUS_APPLICATION_H

#include <iostream>
#include "SDL_lib.h"
#include "constants.h"
#include "game_player.h"


/** Aplikace bonusu.
 * Bonusy se chovají ve hře tak, že jsou to objekty v mapě (Bonus),
 * když je hráč sebere, objekt v mapě (Bonus) zaniká a vzniká
 * některý z potomků BonusApplication, který je aplikací na konkrétního hráče.
 */
class BonusApplication {
	public:
		/// Vytvoří instanci bonusu podle jména.
		static BonusApplication * new_bonus(
				const std::string & bonus_name, Player * player);
		/// Constructor.
		BonusApplication(Player * player);
		/// Vykreslení do panelu.
		virtual void draw_panel(SDL_Surface *window) const {}
		/// Aktualizace stavu.
		virtual bool update() { return true; }
		/// Jméno bonusu z XML.
		static const char * name()
			{ return ""; }
		/// Destructor.
		virtual ~BonusApplication() {};
	protected:
		/// Pointer hráče, na který je aplikovaný bonus.
		Player * player_;
};

class BonusFlame: public BonusApplication {
	public:
		/// Vytvoření.
		BonusFlame(Player * player);
		/// Jméno bonusu z XML.
		static const char * name()
			{ return "bonus_flame"; }
};

class BonusBomb: public BonusApplication {
	public:
		/// Vytvoření.
		BonusBomb(Player * player);
		/// Jméno bonusu z XML.
		static const char * name()
			{ return "bonus_bomb"; }
};


class BonusMegabomb: public BonusApplication {
	#define BONUS_MEGABOMB_COUNT 2
	public:
		/// Vytvoření.
		BonusMegabomb(Player * player);
		/// Jméno bonusu z XML.
		static const char * name()
			{ return "bonus_megabomb"; }
};

class BonusKicker: public BonusApplication {
	public:
		/// Vytvoření.
		BonusKicker(Player * player);
		/// Jméno bonusu z XML.
		static const char * name()
			{ return "bonus_kicker"; }
};

class BonusSlider: public BonusApplication {
	public:
		/// Vytvoření.
		BonusSlider(Player * player);
		/// Jméno bonusu z XML.
		static const char * name()
			{ return "bonus_slider"; }
};

class BonusFireman: public BonusApplication {
	#define BONUS_FIREMAN_PERIODS (10000/MOVE_PERIOD)
	public:
		/// Vytvoření.
		BonusFireman(Player * player);
		/// Vykreslení do panelu.
		virtual void draw_panel(SDL_Surface *window) const;
		/// Aktualizace stavu.
		virtual bool update();
		/// Jméno bonusu z XML.
		static const char * name()
			{ return "bonus_fireman"; }
		/// Destructor.
		virtual ~BonusFireman();
	protected:
		Uint16 remaining_periods_;
};

#endif
