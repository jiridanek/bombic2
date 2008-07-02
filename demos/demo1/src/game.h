/** @file game.h
 * Samotná hra.
 * Game.h obsahuje stejnojmennou třídu obstarávající konkrétní hru jako celek
 * a jí podřízené třídy jako například mapObject.
 */

#include <iostream>
#include <list>
#include <vector>
#include <string>
#include "SDL_lib.h"
#include "constants.h"

/** Obecný objekt hry.
 * Čistě virtuální třída zastřešující veškeré objekty v mapě.
 */
class MapObject{
	public:
		/// Vykreslení.
		virtual void draw()=0;
};

/** Dynamický objekt hry.
 * Objekt v mapě, který během hry vzniká, zaniká
 * nebo se v mapě pohybuje. Má také možnost animace.
 */
class DynamicMO: public MapObject{
	public:
		/// Pohyb.
		virtual void move()=0;
};

/** Statický objekt hry.
 * Objekt v mapě, který je v mapě od začátku do konce hry bez pohybu,
 * pouze s možností animace.
 */
class StaticMO: public MapObject{
	public:
};

/** Konkrétní hra.
 * Instance třídy Game obstarává jednu konkrétní hru.
 * Může to být deathmatch nebo běžný level.
 * Ve svých strukturách uchovává statické i dynamické objekty hry.
 * Stará se o hru samotnou, její spuštění, správné časování a ukončení.
 * Podává informace o typu ukončení hry.
 * @see GameIntro, DeathmatchIntro.
 */
class Game {
	public:
		/// Inicializace hry.
		Game(Uint8 players_count, const std::string & mapname);
// 			bool deathmatch=false, bool creatures=true, bool bombsatend=false);
		/// Nastavení parametrů hráče.
		void set_player(Uint8 player_num, Uint8 lives,
			Uint8 bombs, Uint8 flames, Uint8 boots);
		/// Spuštění hry.
		void play();
		/// Info o ukončení hry.
		bool success() const;
		/// Info o hráči.
		void player(Uint8 player_num, Uint8 & lives,
			Uint8 & bombs, Uint8 & flames, Uint8 & boots) const;
	private:
		/// Seznam dynamických objektů mapy.
		std::list<DynamicMO*> dynamicMOs_;
		/// Seznam statických objektů mapy.
		std::vector<StaticMO*> staticMOs_;
		/// Dvourozměrné pole mapy se seznamem objektů na něm položených.
		std::vector< std::vector< std::list< MapObject* > > > map_array_;

};

/** Zed.
 * Statický objekt, který neshoří, a nejde přes něj chodit,
 * většinou ani létat.
 * Plamen se o zed zarazí.
 */
class Wall: public StaticMO{
	public:
		/// Vykreslení.
		void draw();
};

/** Objekt ležící na zemi.
 * Statický objekt sloužící jako lokální změna pozadí mapy.
 * Nehoří, jde přes něj chodit i létat, letí přes něj plamen.
 */
class Floorobject: public StaticMO{
	public:
		/// Vykreslení.
		void draw();
};

/** Pozadí mapy.
 * Statický objekt jako podklad hrací plochy.
 * Lze přes něj chodit, létat, letí přes něj plamen.
 * Po výbuchu bomby může změnit vzhled.
 */
class Background: public StaticMO{
	public:
		/// Vykreslení.
		void draw();
};

/** Postava Bombiče.
 * Dynamický objekt, který vytváří bomby,
 * zabíjí, umírá, sbírá bonusy, chodí.
 */
class Bomber: public DynamicMO{
	public:
		/// Pohyb.
		void move();
		/// Vykreslení.
		void draw();
};

/** Nestvůra.
 * Dynamický objekt, který zabíjí, umírá, chodí nebo létá.
 * Disponuje umělou inteligencí.
 */
class Creature: public DynamicMO{
	public:
		/// Pohyb.
		void move();
		/// Vykreslení.
		void draw();
};

/** Bedna.
 * Dynamický objekt, vytvořený na začátku hry.
 * Při zasažení plamenem shoří, vytváří bonus,
 * plamen přes něj neletí, nelze přes něj chodit ani létat.
 */
class Box: public DynamicMO{
	public:
		/// Pohyb.
		void move();
		/// Vykreslení.
		void draw();
};

/** Bomba.
 * Dynamický objekt vytvářený především bombičem.
 * Vytváří plamen, může se hýbat.
 */
class Bomb: public DynamicMO{
	public:
		/// Pohyb.
		void move();
		/// Vykreslení.
		void draw();
};

/** Plamen bomby.
 * Dynamický objekt vytvářený především bombou.
 * Zabíjí, sbírá (spaluje) bonusy, nehýbe se.
 */
class Flame: public DynamicMO{
	public:
		/// Pohyb.
		void move();
		/// Vykreslení.
		void draw();
};

/** Bonus.
 * Dynamický objekt. Hoří, nechá se sbírat bombičem.
 */
class Bonus: public DynamicMO{
	public:
		/// Pohyb.
		void move();
		/// Vykreslení.
		void draw();
};
