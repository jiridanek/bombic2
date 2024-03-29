/** @file bonus_application.h
 * Aplikace bonusu na hráče.
 * Obsahuje třídu @c BonusApplication a jeji potomky obstarávající aplikaci bonusu na hráče.
 * Pro přidání bonusu s jménem NAME je třeba udělat následující kroky:
 *	* přidat položku NAME do BonusApplication::TYPE
 *	* přidat do BonusApplication::new_bonus() větev s BonusNAME
 *	* přidat třídu BonusNAME
 *	* implementovat BonusNAME::BonusNAME(Player * player)
 *	* implementovat BonusNAME::name() { return "bonus_NAME"; }
 *	* implementovat BonusNAME::type() { return NAME; }
 *	* přidat do třídy Player třídu BonusNAME jako friend
 */
#ifndef GAME_BONUS_APPLICATION_H
#define GAME_BONUS_APPLICATION_H

#include <iostream>
#include <helpers/sdl.h>
#include <constants.h>
#include "player.h"


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
		/// Typy aplikací bonusů.
		enum TYPE { EMPTY, FLAME, BOMB, MEGABOMB, KICKER, SLIDER,
			FIREMAN, TIMER, SHIELD, SPEED, LIVE, ILLNESS, OTHERS_ILLNESS };
		/// Constructor.
		BonusApplication(Player * player);
		/// Vykreslení do panelu.
		virtual void draw_panel(SDL_Surface *window,
				const SDL_Rect & rect) const {}
		/// Aktualizace stavu.
		virtual bool update() { return true; }
		/// Jméno bonusu z XML.
		static const char * name()
			{ return ""; }
		/// Typ bonusu.
		virtual TYPE type() const
			{ return EMPTY; }
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
		/// Typ bonusu.
		virtual TYPE type() const
			{ return FLAME; }
};

class BonusBomb: public BonusApplication {
	public:
		/// Vytvoření.
		BonusBomb(Player * player);
		/// Jméno bonusu z XML.
		static const char * name()
			{ return "bonus_bomb"; }
		/// Typ bonusu.
		virtual TYPE type() const
			{ return BOMB; }

};


class BonusMegabomb: public BonusApplication {
	#define BONUS_MEGABOMB_COUNT 2
	public:
		/// Vytvoření.
		BonusMegabomb(Player * player);
		/// Jméno bonusu z XML.
		static const char * name()
			{ return "bonus_megabomb"; }
		/// Typ bonusu.
		virtual TYPE type() const
			{ return MEGABOMB; }
};

class BonusSpeed: public BonusApplication {
	public:
		/// Vytvoření.
		BonusSpeed(Player * player);
		/// Jméno bonusu z XML.
		static const char * name()
			{ return "bonus_speed"; }
		/// Typ bonusu.
		virtual TYPE type() const
			{ return SPEED; }
};

class BonusLive: public BonusApplication {
	public:
		/// Vytvoření.
		BonusLive(Player * player);
		/// Jméno bonusu z XML.
		static const char * name()
			{ return "bonus_live"; }
		/// Typ bonusu.
		virtual TYPE type() const
			{ return LIVE; }
};

class BonusKicker: public BonusApplication {
	public:
		/// Vytvoření.
		BonusKicker(Player * player);
		/// Jméno bonusu z XML.
		static const char * name()
			{ return "bonus_kicker"; }
		/// Typ bonusu.
		virtual TYPE type() const
			{ return KICKER; }
};

class BonusSlider: public BonusApplication {
	public:
		/// Vytvoření.
		BonusSlider(Player * player);
		/// Jméno bonusu z XML.
		static const char * name()
			{ return "bonus_slider"; }
		/// Typ bonusu.
		virtual TYPE type() const
			{ return SLIDER; }
};

class BonusShield: public BonusApplication {
	#define BONUS_SHIELD_PERIODS (20000/MOVE_PERIOD)
	public:
		/// Vytvoření.
		BonusShield(Player * player);
		/// Vykreslení do panelu.
		virtual void draw_panel(SDL_Surface *window,
				const SDL_Rect & rect) const;
		/// Aktualizace stavu.
		virtual bool update();
		/// Jméno bonusu z XML.
		static const char * name()
			{ return "bonus_shield"; }
		/// Typ bonusu.
		virtual TYPE type() const
			{ return SHIELD; }
		/// Destructor.
		virtual ~BonusShield();
	protected:
		Uint16 remaining_periods_;
};

class BonusFireman: public BonusApplication {
	#define BONUS_FIREMAN_PERIODS (10000/MOVE_PERIOD)
	public:
		/// Vytvoření.
		BonusFireman(Player * player);
		/// Vykreslení do panelu.
		virtual void draw_panel(SDL_Surface *window,
				const SDL_Rect & rect) const;
		/// Aktualizace stavu.
		virtual bool update();
		/// Jméno bonusu z XML.
		static const char * name()
			{ return "bonus_fireman"; }
		/// Typ bonusu.
		virtual TYPE type() const
			{ return FIREMAN; }
		/// Destructor.
		virtual ~BonusFireman();
	protected:
		Uint16 remaining_periods_;
};

class BonusTimer: public BonusApplication {
	#define BONUS_TIMER_PERIODS (30000/MOVE_PERIOD)
	public:
		/// Vytvoření.
		BonusTimer(Player * player);
		/// Vykreslení do panelu.
		virtual void draw_panel(SDL_Surface *window,
				const SDL_Rect & rect) const;
		/// Aktualizace stavu.
		virtual bool update();
		/// Jméno bonusu z XML.
		static const char * name()
			{ return "bonus_timer"; }
		/// Typ bonusu.
		virtual TYPE type() const
			{ return TIMER; }
		/// Destructor.
		virtual ~BonusTimer();
	protected:
		Uint16 remaining_periods_;
};

class BonusOthersIllness: public BonusApplication {
	public:
		/// Vytvoření.
		BonusOthersIllness(Player * player);
		/// Jméno bonusu z XML.
		static const char * name()
			{ return "bonus_othersillness"; }
		/// Typ bonusu.
		virtual TYPE type() const
			{ return OTHERS_ILLNESS; }
};

class BonusIllness: public BonusApplication {
	#define BONUS_ILLNESS_PERIODS (15000/MOVE_PERIOD)
	public:
		static BonusIllness * new_illness(Player * player);
		enum ILLNESS_TYPE {
			CONFUSED, STOP, SLOW, FAST,
			PLANT_CANNOT, PLANT_ONE, FLAME_ONE,
			SHAKER, LAST_};

		/// Vytvoření.
		BonusIllness(Player * player);
		/// Kontaminace dalšího hráče.
		virtual void contaminate(Player * player) const {
			new BonusIllness(player);
		}
		/// Vykreslení do panelu.
		virtual void draw_panel(SDL_Surface *window,
				const SDL_Rect & rect) const;
		/// Aktualizace stavu.
		virtual bool update();
		/// Jméno bonusu z XML.
		static const char * name()
			{ return "bonus_illness"; }
		/// Typ bonusu.
		virtual TYPE type() const
			{ return ILLNESS; }
		/// Destructor.
		virtual ~BonusIllness() {};
	protected:
		Uint16 remaining_periods_;
};

class BonusIllnessConfused: public BonusIllness {
	public:
		BonusIllnessConfused(Player * player);
		virtual void contaminate(Player * player) const {
			new BonusIllnessConfused(player);
		}
		virtual ~BonusIllnessConfused();
};

class BonusIllnessStop: public BonusIllness {
	public:
		BonusIllnessStop(Player * player);
		virtual void contaminate(Player * player) const {
			new BonusIllnessStop(player);
		}
		virtual ~BonusIllnessStop();
	protected:
		Uint16 old_speed_;
};

class BonusIllnessSlow: public BonusIllnessStop {
	public:
		BonusIllnessSlow(Player * player);
		virtual void contaminate(Player * player) const {
			new BonusIllnessSlow(player);
		}
};

class BonusIllnessFast: public BonusIllnessStop {
	public:
		BonusIllnessFast(Player * player);
		virtual void contaminate(Player * player) const {
			new BonusIllnessFast(player);
		}
};

class BonusIllnessPlantCannot: public BonusIllness {
	public:
		BonusIllnessPlantCannot(Player * player);
		virtual void contaminate(Player * player) const {
			new BonusIllnessPlantCannot(player);
		}
		virtual ~BonusIllnessPlantCannot();
	protected:
		Uint16 old_bombs_count_;
};

class BonusIllnessPlantOne: public BonusIllnessPlantCannot {
	public:
		BonusIllnessPlantOne(Player * player);
		virtual void contaminate(Player * player) const {
			new BonusIllnessPlantOne(player);
		}
};

class BonusIllnessFlameOne: public BonusIllness {
	public:
		BonusIllnessFlameOne(Player * player);
		virtual void contaminate(Player * player) const {
			new BonusIllnessFlameOne(player);
		}
		virtual ~BonusIllnessFlameOne();
	protected:
		Uint16 old_flamesize_;
};

class BonusIllnessShaker: public BonusIllness {
	public:
		BonusIllnessShaker(Player * player);
		virtual void contaminate(Player * player) const {
			new BonusIllnessShaker(player);
		}
		virtual ~BonusIllnessShaker();
};

#endif
