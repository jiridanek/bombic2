/** @file game_AI.h
 * Umělá inteligence.
 * Game_AI.h obsahuje třídy obstarávající umělou inteligenci.
 */
#ifndef GAME_AI_H
#define GAME_AI_H

#include <vector>
#include <queue>
#include <utility>
#include "SDL_lib.h"
#include "game_mapobjects.h"
#include "game_creature.h"
#include "game_player.h"


/** Umělá inteligence.
 * AI je abstraktní třída, která implementuje vlastnosti a metody
 * společné pro většinu úrovní umělých inteligencí.
 * systém úrovní umělých inteligencí není úplně plochý,
 * každá úroveň implementuje stěžejní funkce pro rozhodnutí tahu trochu jinak.
 */
class AI {
	public:
		/// Vytvoří umělou inteligenci.
		static AI* new_ai(Creature *creature, Sint16 intelligence);
		/// Okopíruje umělou inteligenci.
		static AI* new_ai(Creature *creature, const AI * ai);
		/// Uloží rodičovskou nestvůru.
		explicit AI(Creature *creature);
		/// Hýbne nestvůrou.
		virtual void move()=0;
		/// Typ inteligence.
		virtual Sint16 type() const =0;
		/// Pomocná struktura pro otočení a souřadnice při posunu.
		typedef struct{
			DIRECTION d; Uint16 x, y; bool isBlocked;
			bool field_withObject(isTypeOf & isType){
				return GAME->field_withObject(
					x / CELL_SIZE, y / CELL_SIZE, isType);
			}
		} position_t;
		/// Destruktor.
		virtual ~AI() {}
	protected:
		/// Nestvůra, které inteligence patří.
		Creature *creature_;
		/// Rychlost příšery.
		Uint16 getCreatureSpeed();
		/// Výčtový typ pro indexování positions_.
		enum PositionIndex {
			POS_STAY, POS_STRAIGHT, POS_RIGHT,
			POS_BACK, POS_LEFT, POS_LAST };
		/// Seznam pozic pro každý krok.
		std::vector<position_t > positions_;
		/// Inicializuje seznam pozic.
		void initPositions();
		/// Uloží cílové pozice pro každé otočení.
		void updatePositions();
		/// Nastaví nestvůře novou pozici.
		void setPosition(position_t & position);
		/// Komplexně centruje pozici na políčku.
		void centerCoordinates(
			position_t & position, bool isMainX, Sint8 sign);
		/// Partikulárně centruje souřadnici dostředu políčka.
		void centerCoordinate(Uint16 & coordinate, Sint8 sign);
		/// Zjistí, zda je možné vstoupit na políčko.
		bool checkField(
			const position_t & position, const isTypeOf & isBlocking);
};

/** Umělá inteligence nulté úrovně (bez pohybu).
 * AI_0 je inteligence pro příšery, které se nepohybují.
 */
class AI_0 : public AI {
	public:
		/// Zavolá konstruktor AI
		explicit AI_0(Creature *creature);
		/// Hýbne nestvůrou.
		virtual void move();
		/// Typ inteligence.
		virtual Sint16 type() const
			{ return 0; }
		/// Destruktor.
		virtual ~AI_0() {}

};

/**
 * Chození rovně I.
 * Jde pořád rovně, když narazí, náhodně se rozhodne kudy dál.
 * Vůbec neřeší blížící se výbuch nebo dostižení hráče.
 */
class AI_1 : public AI {
	public:
		/// Nastaví příšeru a definuje co je blokující.
		explicit AI_1(Creature *creature);
		/// Hýbne nestvůrou.
		virtual void move();
		/// Typ inteligence.
		virtual Sint16 type() const
			{ return 1; }
		/// Destruktor.
		virtual ~AI_1() {}
	protected:
		/// Nastaví příšeru a blokující predikát.
		AI_1(Creature *creature, isTypeOf & isBlocking);
		/// Najde index pozice, na kterou bychom měli jít.
		virtual PositionIndex findPosIndex(isTypeOf & isBlocking);
		/// Podle isBlocking_ chodí pořád rovně.
		PositionIndex findPosIndexToWalkStraight_(
			isTypeOf & isBlocking);

		isTypeOf & isBlocking_;
};

/**
 * Chození rovně II.
 * Stejně jako AI_1, ale dává pozor na plameny.
 * Neřeší blížící se výbuch ani hráče.
 */
class AI_2 : public AI_1 {
	public:
		/// Zavolá konstruktor AI
		explicit AI_2(Creature *creature);
		/// Typ inteligence.
		virtual Sint16 type() const
			{ return 2; }
		/// Destruktor.
		virtual ~AI_2() {}
};

/**
 * Chození rovně III.
 * Stejně jako AI_1, ale dává pozor na plameny a presumpce.
 * Neřeší pouze hráče. Sám nevleze do plamene ani do presumpce plamene.
 */
class AI_3 : public AI_1 {
	public:
		/// Zavolá konstruktor AI
		explicit AI_3(Creature *creature);
		/// Hýbne nestvůrou.
		virtual void move();
		/// Typ inteligence.
		virtual Sint16 type() const
			{ return 3; }
		/// Destruktor.
		virtual ~AI_3() {}
	protected:
		isTypeOf & isBad_;
};

#define AI_4_MIN_DISTANCE_WALKED_STRAIGHT (CELL_SIZE*4)

/** Náhodné chození I.
 * Náhodně mění směr, je však definováno, jakou vzdálenost musí ujít rovně.
 * (Aby se nestalo, že se bude motat na místě.
 * Vůbec neřeší blížící se výbuch nebo dostižení hráče.
 */
class AI_4 : public AI_1 {
	public:
		/// Zavolá konstruktor AI
		explicit AI_4(Creature *creature);
		/// Hýbne nestvůrou.
		virtual void move();
		/// Typ inteligence.
		virtual Sint16 type() const
			{ return 4; }
		/// Destruktor.
		virtual ~AI_4() {}
	protected:
		/// Zavolá konstruktor AI, nastaví ostatní parametry.
		AI_4(Creature *creature, isTypeOf & isBlocking,
			Uint16 minDistance);
		/// Najde index pozice, na kterou bychom měli jít.
		virtual PositionIndex findPosIndex(isTypeOf & isBlocking);
		/// Najde náhodně pozici kam může jít.
		PositionIndex findPosIndexToWalkRandomly_(
			isTypeOf & isBlocking);

		void updateDistance_(position_t & position);

		/// Vzdálenost, jakou musí najednou ujít.
		Uint16 minDistanceWalkedStraight_;
		/// Vzdálenost ušlá od minulé změny směru.
		Uint16 distanceWalkedStraight_;
};

#define AI_5_MIN_DISTANCE_WALKED_STRAIGHT (CELL_SIZE*3)

/**
 * Náhodné chození II.
 * Stejně jako AI_4, ale dává pozor na plameny.
 * Neřeší blížící se výbuch ani hráče.
 */
class AI_5 : public AI_4 {
	public:
		/// Zavolá konstruktor AI_4
		explicit AI_5(Creature *creature);
		/// Typ inteligence.
		virtual Sint16 type() const
			{ return 5; }
		/// Destruktor.
		virtual ~AI_5() {}
};

#define AI_6_MIN_DISTANCE_WALKED_STRAIGHT (CELL_SIZE*2)

/**
 * Náhodné chození III.
 * Stejně jako AI_4, ale dává pozor na plameny a presumpce.
 * Neřeší pouze hráče. Sám nevleze do plamene ani do presumpce plamene.
 */
class AI_6 : public AI_4 {
	public:
		/// Zavolá konstruktor AI_4
		explicit AI_6(Creature *creature);
		/// Hýbne nestvůrou.
		virtual void move();
		/// Typ inteligence.
		virtual Sint16 type() const
			{ return 6; }
		/// Destruktor.
		virtual ~AI_6() {}
	protected:
		isTypeOf & isBad_;
};

/**
 * Útok na krátko.
 * Utilita pro umělou inteligence, která neřeší pohyb, pouze kontroluje
 * políčka kolem sebe a když na nich objeví hráče, může rychle zaútočí.
 */
class AI_ShortAttack {
	public:
		explicit AI_ShortAttack(Creature * creature);
	protected:
		/// Uloží pozici hráče pokud je nějaký poblíž.
		void updatePlayerPosition();
		/// Zda našel hráče.
		bool playerFound;
		/// Najde útočící pozici.
		AI::position_t createPositionToAttack_();
	private:
		Creature * creature_;
		AI::position_t currPosition;
		void setPlayerPosition(Uint16 field_x, Uint16 field_y,
			Sint16 relative_x, Sint16 relative_y);
		/// Relativní pozice hráče v mapě.
		Sint16 player_relative_x_;
		/// Relativní pozice hráče v mapě.
		Sint16 player_relative_y_;
};

class AI_7 : public AI_2, AI_ShortAttack {
	public:
		/// Zavolá konstruktor AI
		explicit AI_7(Creature *creature);
		/// Hýbne nestvůrou.
		virtual void move();
		/// Typ inteligence.
		virtual Sint16 type() const
			{ return 7; }
		/// Destruktor.
		virtual ~AI_7() {}
};

class AI_8 : public AI_6, AI_ShortAttack {
	public:
		/// Zavolá konstruktor AI
		explicit AI_8(Creature *creature);
		/// Hýbne nestvůrou.
		virtual void move();
		/// Typ inteligence.
		virtual Sint16 type() const
			{ return 8; }
		/// Destruktor.
		virtual ~AI_8() {}
};


#define AI_10_MAX_TRACE_DEPTH 10
#define AI_10_MAX_UPDATE_PERIOD CELL_SIZE

/** Umělá inteligence první úrovně.
 * AI_10 je velice inteligentní útočná úroveň,
 * řeší plameny, jejich předpovědi (a pozici hráče).
 */
class AI_10 : public AI {
	public:
		/// Zavolá konstruktor AI
		AI_10(Creature *creature);
		/// Hýbne nestvůrou.
		virtual void move();
		/// Typ inteligence.
		virtual Sint16 type() const { return 10; }
		/// Destruktor.
		virtual ~AI_10() {};
	private:
		typedef std::vector< std::vector< Sint16 > > trace_array_t;
		typedef std::queue< std::pair< Uint16, Uint16 > > fields_queue_t;

		trace_array_t empty_trace_array_;
		trace_array_t trace_array_;


		void update_trace_array_();
		void eval_trace_array_(fields_queue_t & fields_queue, bool in_danger);

		position_t & get_position_();
		position_t & get_random_position_();

		isTypeOf & isBlocking_, & isBad_;

		Uint16
			old_x_, ///< Moje stará souřadnice.
			old_y_, ///< Moje stará souřadnice.
			target_x_, ///< Souřadnice cíle.
			target_y_, ///< Souřadnice cíle.
			last_trace_update_; ///< Čítač od poslední update trasy.
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
		/// Typ inteligence.
		virtual Sint16 type() const { return -1; }
		/// Destruktor.
		virtual ~AI_fromKeyboard() {};
	private:
		/// Stav klávesnice.
		Uint8 *keystate_;
		/// Vytvoří a nastaví novou pozici.
		void makePosition(position_t & position,
			Sint16 factor_x, Sint16 factor_y);
};


#endif
