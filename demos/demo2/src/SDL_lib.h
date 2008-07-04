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

class Colors {
	public:
		static SDL_Color transparent();

		static SDL_Color white();
		static SDL_Color yellow();
		static SDL_Color aqua();
		static SDL_Color red();
		static SDL_Color lime();
		static SDL_Color blue();
		static SDL_Color black();
		static SDL_Color green();
		static SDL_Color maroon();
		static SDL_Color olive();
		static SDL_Color purple();
		static SDL_Color gray();
		static SDL_Color navy();
		static SDL_Color teal();
		static SDL_Color silver();
	private:
		static SDL_Color transparent_, white_, yellow_, aqua_,
				red_, lime_, blue_, black_, green_, maroon_,
				olive_, purple_, gray_, navy_, teal_, silver_;
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
		Surface(SDL_Surface *sur_SDL);
		Surface(const Surface & sur);
		~Surface();

		Surface & operator= (const Surface & sur);

		SDL_Surface* GetSurface();
	private:
		Uint16 decrement_();

		SDL_Surface* surface_;
		Uint16* references_;
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

#endif
