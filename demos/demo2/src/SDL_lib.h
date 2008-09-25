/*
 * SDL_lib
 * je nadstavba nad knihovnou SDL, SDL_image, SDL_gfx, SDL_ttf
 * vykresluje jednoduche geometricke utvary, pismo, nacita obrazky...
 */

#ifndef SDL_LIB_
#define SDL_LIB_

#include "SDL.h"
#include "SDL_ttf.h"
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <cstdlib> // For rand()

namespace Color{
	extern const SDL_Color transparent;

	extern const SDL_Color white;
	extern const SDL_Color yellow;
	extern const SDL_Color aqua;
	extern const SDL_Color red;
	extern const SDL_Color lime;
	extern const SDL_Color blue;
	extern const SDL_Color black;
	extern const SDL_Color green;
	extern const SDL_Color maroon;
	extern const SDL_Color olive;
	extern const SDL_Color purple;
	extern const SDL_Color gray;
	extern const SDL_Color navy;
	extern const SDL_Color teal;
	extern const SDL_Color silver;
};

class Fonts {
	public:
		// pouze nastavi jmeno souboru s fontem
		Fonts(char* filename);
		// vratim font velikosti size
		TTF_Font* operator[](unsigned int size);
		// zrusim vsechny vytvorene velikosti fontu
		~Fonts();
	private:
		// prida velikost pisma pokud jeste neni vytvorena
		TTF_Font* add(unsigned int size);
		// nazev souboru s fontem
		std::string fontFile;
		// typ mapy, uklada se dvojice <size,vytvoreny font>
		typedef std::map<unsigned int, TTF_Font* > fontMap_t;
		// vsechny vytvorene velikosti fontu
		fontMap_t fontMap;
};

class Surface {
	public:
		Surface();
		Surface(const Surface & sur);
		Surface(SDL_Surface *sur_SDL);
		~Surface();

		Surface & operator= (const Surface & sur);
		Surface & operator= (SDL_Surface *sur_SDL);

		SDL_Surface* GetSurface() const;
		Uint16 width() const;
		Uint16 height() const;
	private:
		Uint16 decrement_();

		SDL_Surface* surface_;
		Uint16* references_;
};

// forward
class TiXmlElement;

/**
 * Animace jednoho jevu, jako posloupnost obrázků.
 * Inicializace XML elementem a zdrojovým surface,
 * nebo čistým okopírováním jiné animace.
 * Interface obsahuje ještě metodu na resetování stavu animace,
 * update stavu animace a vykreslení aktuálního obrázku do okna.
 * @throw string Při chybě v načítání xml vypouští výjimky s chybovým hlášením.
 * @see Surface
 */
class Animation {
	public:
		/// Bez inicializace.
		Animation();
		/// Inicializace z XML a zdrojového surface.
		Animation(TiXmlElement* el, Uint16 width, Uint16 height,
			const Surface & sur_src, const Surface & sur_shadow_src=0);
		/// Inicializace z XML a zdrojového surface.
		const Animation & Animation::initialize(TiXmlElement* el,
					Uint16 width, Uint16 height,
					const Surface & sur_src, const Surface & sur_shadow_src);
		/// Okopírování animace.
		Animation(const Animation & anim);
		/// Okopírování animace.
		const Animation & operator=(const Animation & anim);

		/// Nastavení prvního obrázku jako aktuální.
		void reset();
		/// Update stavu animace (typicky nastavení dalšího framu)
		bool update();
		/// Vykreslení aktuálního framu.
		void draw(SDL_Surface* window, Uint16 x, Uint16 y) const;
		/// Výška animace.
		Uint16 height() const;
		/// Šířka animace.
		Uint16 width() const;
	private:
		/// Uloží konkrétní surface zadané v elementu.
		void Animation::loadItem_(TiXmlElement* el, Uint16 width, Uint16 height,
				const Surface & sur_src, const Surface & sur_shadow_src);

		typedef std::vector<Surface> frames_t;
		/// Seznam framů a jejich stínů.
		frames_t frames_, shadow_frames_;
		/// Index dalšího framu.
		frames_t::size_type next_frame_;
		/// Vykreslovat stíny.
		bool draw_shadow_;
		/// Délka zobrazení jednoho framu.
		Uint16 frame_period_, last_access_;

};

// Inicializace SDL a TTF, vytvori okno o zadanych rozmerech a zahlavi
void window_init(SDL_Surface ** pWindow, int win_w, int win_h, const char *caption);


// surface s textem str v barve color a fontem font
SDL_Surface* get_text(TTF_Font* font, const char* str, SDL_Color color);

// vytvori surface potrebne velikosti a barvy
SDL_Surface* create_surface(Uint16 w, Uint16 h, SDL_Color color);
void set_transparent_color(SDL_Surface *sur, SDL_Color color);
SDL_Surface* create_transparent_surface(Uint16 w, Uint16 h, bool transparent=false);

// vykresli surface_src do surface_dst
void draw_surface(int x, int y, SDL_Surface* surface_src, SDL_Surface* surface_dst);

// vykresli src do dst vycentrovane
void draw_center_surface(SDL_Surface* surface_src, SDL_Surface* surface_dst);

// vymaze misto v surface_dst na kterem by byl surface_src
void clear_surface(int x, int y, SDL_Color color,
			SDL_Surface* surface_src, SDL_Surface* surface_dst);

// nakresli do `surface` pixel na souradnice [x,y] barvy `color`
void draw_pixel(SDL_Surface* surface, int x, int y, SDL_Color color);

// pres parametrickou rovnici usecky
void draw_line(SDL_Surface* surface, int x1, int y1, int x2, int y2, SDL_Color color);

// pocka od last tak dlouho abych vykresloval fps obrazku za sekundu
// vrati aktualni cas ukonceni teto fce
// pro fps==0 bude vykreslovat obrazek jednou za dve sekundy
Uint32 SDL_fps(Uint32 last, Uint32 fps);

// vraci hodnotu zmacknute klavesy
// nebo SDLK_FIRST pokud nebyla klavesa zmacknuta
// nebo SDLK_LAST pri zadosti o ukonceni
SDLKey get_event();
// ceka na udalost
SDLKey wait_event();

// vrati true kdyz byla zadost o ukonceni, jinak false
// vrati take true pokud udalost vyvolala klavesa key
bool get_event_isquit(SDLKey key= SDLK_LAST);
// tato fce navic ceka na nejakou udalost
// vrati take true pokud udalost vyvolala klavesa key
bool wait_event_isquit(SDLKey key= SDLK_LAST);

// vraci pseudonahodne cislo od nuly do jednicky vcetne
double SDL_Rand();

#endif
