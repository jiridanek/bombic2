/** @file game_AI.h
 * Umělá inteligence.
 * Game_AI.h obsahuje třídy obstarávající umělou inteligenci.
 */
#ifndef GAME_AI_H
#define GAME_AI_H

#include <vector>
#include <queue>
#include <utility>
#include <map>
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
		/// Zavolá konstruktor AI_4, nastaví ostatní parametry.
		AI_6(Creature *creature, Uint16 minDistance);
		/// Predikát odhalující špatné políčko.
		isTypeOf & isBad_;
};

/**
 * Útok na krátko.
 * Utilita pro umělou inteligence, která neřeší pohyb, pouze kontroluje
 * políčka kolem sebe a když na nich objeví hráče, může rychle zaútočí.
 * Užitečné v kombinaci s nějakým potomkem AI.
 */
class AI_ShortAttack {
	public:
		explicit AI_ShortAttack(Creature * creature);
	protected:
		/// Obnoví pozici cíle.
		void updateTargetPosition();
		/// Zda našel cíl (má na co útočit).
		bool targetFound() const
			{ return targetFound_; }
		/// Vytvoří útočící pozici.
		AI::position_t createPositionToAttack_();
	private:
		/// Rodičovská příšera.
		Creature * creature_;
		/// Aktuální pozice rodičovské příšery.
		AI::position_t currPosition_;
		/// Pokusí se nastavit pozici cíle.
		bool trySetTargetPosition_(Uint16 field_x, Uint16 field_y,
			Sint16 relative_x, Sint16 relative_y);
		/// Zda našel cíl.
		bool targetFound_;
		/// Relativní pozice cíle v mapě.
		Sint16 target_relative_x_;
		/// Relativní pozice cíle v mapě.
		Sint16 target_relative_y_;
};

/**
 * Chození rovně II s útokem nablízko.
 * Stejně jako AI_2, pokud ale objeví ve své blízkosti hráče,
 * zaútočí na něj.
 */
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

/**
 * Náhodné chození III s útokem nablízko.
 * Stejně jako AI_6, pokud ale objeví ve své blízkosti hráče,
 * zaútočí na něj.
 */
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

#define AI_9_MIN_DISTANCE_WALKED_STRAIGHT (CELL_SIZE/2)
#define AI_9_MAX_TRACE_DEPTH 10

/**
 * Náhodné chození III s defenzivním prohledáváním.
 * Stejně jako AI_6, pokud je ale v nebezpečí, použije prohledávání
 * mapy do šířky a najde nejbližší bezpečné políčko.
 */
class AI_9 : public AI_6 {
	public:
		/// Zavolá konstruktor AI_6
		explicit AI_9(Creature *creature);
		/// Hýbne nestvůrou.
		virtual void move();
		/// Typ inteligence.
		virtual Sint16 type() const
			{ return 9; }
		/// Destruktor.
		virtual ~AI_9() {}
	protected:
		/// Typ rozlišující chtěné a nechtěné.
		enum wanted_t { WANTED, UNWANTED };
		/// Typ pro souřadnice políčka.
		typedef field_t traceField_t;
		/// Typ pro hodnotu políčka trasovacího pole.
		typedef struct traceValue_t {
			enum traced_t { NO_TRACE, CANT_OVER, CAN_OVER };

			traceValue_t(){
				traced_ = NO_TRACE;
			}
			bool wasNotTraced(){
				return traced_==NO_TRACE;
			}
			bool canGoOver(){
				return traced_==CAN_OVER;
			}
			bool cannotGoOver(){
				return traced_==CANT_OVER;
			}
			void setCannotGoOver(){
				traced_ = CANT_OVER;
			}
			Uint16 getDepth(){
				return depth_;
			}
			void setDepth(Uint16 depth){
				traced_ = CAN_OVER;
				depth_ = depth;
			}
			void setFromField(const traceField_t & fromField){
				fromField_ = fromField;
			}
			const traceField_t & getFromField(){
				return fromField_;
			}

			private:
				Uint16 depth_;
				traced_t traced_;
				traceField_t fromField_;
		} traceValue_t;

		/** Typ trasovacího pole.
		 * Je otázka jestli trasovací pole opravdu implementovat jako pole.
		 * Vzhledem k tomu, že málokdy se využije vetší kus celé mapy
		 * a vzhledem k tomu, že mapa může být i velká se uchyluji k dynamičtější
		 * struktuře std::map, kde klíč budou souřadnice políčka.
		 */
		typedef std::map<traceField_t, traceValue_t > traceArray_t;
		/// Typ fronty políček.
		typedef std::queue< traceField_t > fieldsQueue_t;
		/// Najde index pozice,
		/// na kterou máme jít abychom se dostali z nebezpečí.
		PositionIndex findPosIndexToWalkFromRisk_(isTypeOf & isBlocking);
		/// Ohodnotí trasovací pole (start rekurze).
		void evalTraceArray_( traceArray_t & traceArray,
			isTypeOf & isBlocking, isTypeOf & isInteresting,
			wanted_t interestArea);
		/// Cíl nalezen.
		bool targetFound() const
			{ return targetFound_; }
		/// Najde pomocí backtrackingu index výsledné pozice.
		PositionIndex findPosIndexUsingBacktracking_(
			traceArray_t & traceArray);
	private:
		/// Ohodnotí trasovací pole (krok rekurze).
		void recursiveEvalTraceArray_(
			traceArray_t & traceArray, fieldsQueue_t & fieldsQueue,
			isTypeOf & isBlocking, isTypeOf & isInteresting,
			wanted_t interestArea);
		/// Bylo-li nalezeno ve správném smyslu zajímavé políčko.
		bool interestingFound_(const traceField_t & field,
			isTypeOf & isInteresting, wanted_t interestingArea);
		/// Ohodnotí a vloží do fronty sousední políčka.
		void evalAndQueueNextFields_(traceArray_t & traceArray,
			fieldsQueue_t & fieldsQueue, isTypeOf & isBlocking);
		/// Posune souřadnice políčka.
		void moveFieldCoordinate_(traceField_t & field, DIRECTION dir);
		/// Cíl nalezen.
		bool targetFound_;
		/// Souřadnice cíle v mapě.
		traceField_t targetField_;

};

/**
 * Ofenzivní i defenzivním prohledávání.
 * Pokud je v nebezpečí, použije prohledávání
 * mapy do šířky a najde nejbližší bezpečné políčko.
 * Pokud v nebezpečí není, použije prohledávání mapy
 * aby našel cíl, na který by zaútočil.
 */
class AI_10 : public AI_9 {
	public:
		/// Zavolá konstruktor AI_9
		AI_10(Creature *creature);
		/// Typ inteligence.
		virtual Sint16 type() const
			{ return 10; }
		/// Destruktor.
		virtual ~AI_10() {}
	protected:
		/// Najde index pozice, na kterou bychom měli jít.
		virtual PositionIndex findPosIndex(isTypeOf & isBlocking);
		/// Najde index pozice,
		/// na kterou máme jít abychom zaútočili na hráče.
		PositionIndex findPosIndexToComeCloseToPlayer_(isTypeOf & isBlocking);
};

/** NEumělá inteligence pro hráče.
 * AI_fromKeyboard je inteligence, která je řízena vstupem z klávesnice.
 * Pokud se nic neděje, hráčem nehýbá.
 * Řeší, aby uživatel svou interakcí chodil pouze tudy, kudy může.
 * Chodí s hráčem čtyřmi směry, pokládá bomby, kope do nich,
 * odpaluje je.
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
		/// Obslouží klávesy pro pohyb.
		bool handleMoveKeys_(Player * player);
		/// Obslouží pokládání bomb.
		void handlePlantKey_(Player * player, bool playerMoved);
		/// Obslouží ruční odpalování bomb.
		void handleTimerKey_(Player * player);
		/// Vytvoří a nastaví novou pozici.
		bool trySetPosition_(Player * player, position_t & position);
};


#endif
