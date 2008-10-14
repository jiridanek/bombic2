/** @file SDL_lib.h
 * SDL_lib
 * je nadstavba nad knihovnou SDL, SDL_image, SDL_ttf
 * vykresluje jednoduché geometrické útvary, písmo, načítá obrázky, animace...
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

/// Konstantní pojmenované barvy.
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

/** Písmo.
 * Jedna instance třídy Fonts obstarává jeden druh písma
 * ve všech jeho velikostech. Dynamicky různé velikosti tvoří podle toho
 * jak je o ně žádán. Před prvním voláním operatoru[]
 * musí být SDL fonty inicializovány pomocí TTF_Init().
 * @see window_init(), TTF_Init()
 */
class Fonts {
	public:
		/// Pouze nastaví jméno souboru s fontem.
		Fonts(char* filename);
		/// Konkrétní velikost fontu.
		TTF_Font* operator[](Uint16 size);
		/// Zrušení všech vytvořených velikostí fontu
		~Fonts();
	private:
		/// Přidání velikosti písma, pokud ješte není vytvořena.
		TTF_Font* add(Uint16 size);
		/// Název souboru s fontem.
		std::string fontFile;
		/// Typ mapování, ukládá se dvojice <size,vytvořený font>.
		typedef std::map<Uint16, TTF_Font* > fontMap_t;
		/// Všechny vytvořené velikosti fontu.
		fontMap_t fontMap;
};

/** Počítání referencí pro SDL_Surface.
 * Datový typ Surface obaluje strukturu SDL_Surface, poskytuje
 * bohatý interface pro kopírování, inicializaci a přetypování z SDL_Surface.
 * @see SDL_Surface
 */
class Surface {
	public:
		/// Bez inicializace.
		Surface();
  		/// Inicializace zkopírováním.
		Surface(const Surface & sur);
		/// Inicializace přímo pomocí SDL_Surface, přetypování.
		Surface(SDL_Surface *sur_SDL);
		/// Zrušení konkrétní instance.
		~Surface();
		/// Přiřazení zkopírováním.
		Surface & operator= (const Surface & sur);
		/// Přiřazení přímo SDL_Surface.
		Surface & operator= (SDL_Surface *sur_SDL);
		/// SDL_Surface konkrétní instance.
		SDL_Surface* GetSurface() const;
		/// Šířka surface.
		Uint16 width() const;
		/// Výška surface.
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
					const Surface & sur_src, const Surface & sur_shadow_src=0);
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
		Uint16 frame_period_,
		/// Doba od posledního posunu framu.
			last_access_;

};

/// Inicializace SDL a TTF, vytvoří okno o zadaných rozměrech a záhlaví.
void window_init(SDL_Surface ** pWindow, int win_w, int win_h, const char *caption);

/// Vytvoření surface s textem.
SDL_Surface* get_text(TTF_Font* font, const char* str, SDL_Color color);

/// Vytvoření surface.
SDL_Surface* create_surface(Uint16 w, Uint16 h, SDL_Color color);
/// Nastavení průhledné barvy pro surface.
void set_transparent_color(SDL_Surface *sur, SDL_Color color);
/// Vytvoření surface s průhlednou barvou (volitelně celé poloprůhledně).
SDL_Surface* create_transparent_surface(Uint16 w, Uint16 h, bool transparent=false);

/// Vykreslení surface_src do surface_dst.
void draw_surface(int x, int y, SDL_Surface* surface_src, SDL_Surface* surface_dst);

/// Vykreslení surface_src do surface_dst vycentrovaně.
void draw_center_surface(SDL_Surface* surface_src, SDL_Surface* surface_dst);

/// Vybarvení místa v surface_dst, na kterém by byl surface_src.
void clear_surface(SDL_Color color, SDL_Surface* surface);

/// Nakreslení pixelu do surface.
void draw_pixel(SDL_Surface* surface, int x, int y, SDL_Color color);

/// Nakreslení úsečky.
void draw_line(SDL_Surface* surface, int x1, int y1, int x2, int y2, SDL_Color color);

/// Frames per second zdrží program, aby nevykresloval zbytečně často.
Uint32 SDL_fps(Uint32 last, Uint32 fps);

/// Hodnota zmáčknuté klávesy, nebo SDL_FIRST (pokud není žádná ve frontě).
SDLKey get_event();
/// Hodnota zmáčknuté klávesy, čeká se, až bude nějaká ve frontě.
SDLKey wait_event();

/// Kontrola žádosti o ukončení.
bool get_event_isquit(SDLKey key= SDLK_LAST);
/// Kontrola žádosti o ukončení, čeká se na nějakou událost.
bool wait_event_isquit(SDLKey key= SDLK_LAST);

/// Pseudonáhodné číslo v intervalu [0,1].
double SDL_Rand();

#endif
