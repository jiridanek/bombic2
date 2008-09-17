/*
 * SDL_lib
 * je nadstavba nad knihovnou SDL, SDL_image, SDL_gfx, SDL_ttf
 * vykresluje jednoduche geometricke utvary, pismo, nacita obrazky...
 */

#include "SDL_lib.h"
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <utility>

// deklarace barev
namespace Color{
	const SDL_Color transparent = {255,0,255}; // pruhledna

	const SDL_Color white = {255,255,255}; // bila
	const SDL_Color yellow = {255,255,0}; // zluta
	const SDL_Color aqua = {0,255,255}; // azurova
	const SDL_Color red = {255,0,0}; // cervena
	const SDL_Color lime = {0,255,0}; // svetle_zelena
	const SDL_Color blue = {0,0,255}; // modra
	const SDL_Color black = {0,0,0}; // cerna
	const SDL_Color green = {0,128,0}; // zelena
	const SDL_Color maroon = {128,0,0}; // hneda
	const SDL_Color olive = {128,128,0}; // olivova (vyblita)
	const SDL_Color purple = {128,0,128}; // fialova
	const SDL_Color gray = {128,128,128}; // seda
	const SDL_Color navy = {0,0,128}; // tmave_modra
	const SDL_Color teal = {0,128,128}; // modro_zelena
	const SDL_Color silver = {192,192,192}; // svetle_seda
};


/******************* class Fonts *********************/

// pouze nastavi jmeno souboru s fontem
Fonts::Fonts(char* filename): fontFile(filename), fontMap() {
}

TTF_Font* Fonts::operator[](unsigned int size){
	// vratim font velikosti size
	// pokud ho nenajdu tak ho vytvorim
	fontMap_t::const_iterator it;
	it= fontMap.find(size);
	// zjistim jestli uz byla velikost vytvorena
	if(it==fontMap.end()) return add(size);
	return it->second; // vracim jiz vytvoreny font
}

Fonts::~Fonts(){
	// zrusim vsechny vytvorene velikosti fontu
	fontMap_t::iterator it;
	for(it= fontMap.begin() ; it!=fontMap.end() ; ++it){
		TTF_CloseFont(it->second);
		it->second = 0;
	}
}

TTF_Font* Fonts::add(unsigned int size){
	// prida velikost pisma pokud jeste neni vytvorena
	// prvne ji vytvorim
	TTF_Font* font = TTF_OpenFont(fontFile.c_str(), size);
	if(!font){
		std::cerr << "Unable to open font: " << TTF_GetError() << std::endl;
		throw 1;
	}
	// a ulozim do mapy
	fontMap.insert(std::make_pair(size, font));
	return font;
}

/******** END of class Fonts ************************/

/******** class Surface *****************************/


Surface::Surface() : surface_(0){
	// Create a new reference
	references_ = new Uint16(1);
}

Surface::Surface(SDL_Surface *sur_SDL):
				surface_(sur_SDL){
	// Create a new reference
	references_ = new Uint16(1);
}

Surface::Surface(const Surface & sur):
			surface_(sur.surface_), references_(sur.references_){
	++(*references_);
}

Surface::~Surface(){
	decrement_();
}

Uint16 Surface::decrement_(){
	// Decrement the reference count
	// if reference become zero delete the data
	if( --(*references_) == 0){
		delete references_;
		if(surface_){
			SDL_FreeSurface(surface_);
			surface_=0;
		}
	}
	return *references_;
}

Surface & Surface::operator= (const Surface & sur){
	// Assignment operator
	if(this != &sur){ // Avoid self assignment
		decrement_();
		// Copy the data and reference pointer
		// and increment the reference count

		surface_ = sur.surface_;
		references_ = sur.references_;
		++(*references_);
	}
	return *this;
}

Surface & Surface::operator= (SDL_Surface * sur_SDL){
	// Assignment operator
	if(this->surface_ != sur_SDL){ // Avoid self assignment
		decrement_();
		// Copy the data and reference pointer
		// and create the reference count

		surface_ = sur_SDL;
		references_ = new Uint16(1);
	}
	return *this;
}
// TODO zmenit na getSurface()
SDL_Surface* Surface::GetSurface() const {
	return surface_;
}
Uint16 Surface::width() const {
	return surface_->w;
}
Uint16 Surface::height() const {
	return surface_->h;
}

/******** END of class Surface **********************/

/******** SDL_lib functions **************************/

void window_init(SDL_Surface ** pWindow, int win_w, int win_h, const char *caption){
	// Inicializace SDL
	if(SDL_Init(SDL_INIT_VIDEO) == -1){
		std::cerr << "Unable to initialize SDL: " << SDL_GetError() << std::endl;
		exit(1);
	}
	atexit(SDL_Quit);

	// Vytvori okno s definovanymi vlastnostmi
	*pWindow = SDL_SetVideoMode(win_w, win_h, 0, SDL_HWSURFACE | SDL_DOUBLEBUF);
	if(*pWindow == NULL){
		std::cerr << "Unable to set video: " << SDL_GetError() << std::endl;
		exit(1);
	}
	// Inicializace TTF
	if(TTF_Init() == -1){
		std::cerr << "Unable to initialize TTF: " << TTF_GetError() << std::endl;
		exit(1);
	}
	atexit(TTF_Quit);

	// titulek okna
	SDL_WM_SetCaption(caption, 0);
}

SDL_Surface* get_text(TTF_Font* font, const char* str, SDL_Color color){
        return TTF_RenderUTF8_Blended(font, str, color);
}

// vytvori surface potrebne velikosti a barvy
SDL_Surface* create_surface(Uint16 w, Uint16 h, SDL_Color color){
	SDL_Surface* sur;
	// vytvorim surface
	sur= SDL_CreateRGBSurface(
				SDL_HWSURFACE, w, h, 32, 0,0,0,0);
	// vyplnim barvou
	SDL_FillRect(sur, 0, SDL_MapRGB(sur->format,
				color.r,color.g,color.b) );
	// vratim
	return sur;
}
void set_transparent_color(SDL_Surface *sur, SDL_Color color){
	if(!sur) return;
	SDL_SetColorKey(sur, SDL_SRCCOLORKEY,
		SDL_MapRGB(sur->format, color.r, color.g, color.b));
}
// vytvori surface potrebne velikosti a nastavi prusvitnou barvu
SDL_Surface* create_transparent_surface(Uint16 w, Uint16 h, bool transparent){
	// transparentni barva
// 	SDL_Color trans_color= Colors::transparent();
	// vytvorim surface
	SDL_Surface *sur=create_surface(w, h, Color::transparent);
	// nastavim transparentni barvu
	set_transparent_color(sur, Color::transparent);
	//nastavim pruhlednost
	if(transparent)
		SDL_SetAlpha(sur, SDL_SRCALPHA | SDL_RLEACCEL, 128);
	return sur;
}


void draw_surface(int x, int y, SDL_Surface* surface_src, SDL_Surface* surface_dst){
	// vykresli surface_src do surface_dst
	if(surface_src && surface_dst){
		SDL_Rect rect={x,y,surface_src->w,surface_src->h};
		SDL_BlitSurface(surface_src, 0, surface_dst, &rect);
	}
}

// vykresli src do dst vycentrovane
void draw_center_surface(SDL_Surface* surface_src, SDL_Surface* surface_dst){
	if(surface_src && surface_dst){
		draw_surface(
			(surface_dst->w - surface_src->w)/2,
			(surface_dst->h - surface_src->h)/2,
			surface_src, surface_dst);
	}
}

void clear_surface(SDL_Color color, SDL_Surface* surface){
	// vybarvi surface barvou color
	if(surface){
		SDL_FillRect(surface, 0, SDL_MapRGB(surface->format,
			color.r,color.g,color.b) );
	}
}

void draw_pixel(SDL_Surface* surface, int x, int y, SDL_Color color){
	// nakresli do `surface` pixel na souradnice [x,y] barvy `color`
	if(!surface) return;

	if(x>=surface->w || y>=surface->h || x<0 || y<0)
		return; // Presahuje rozmery surface

	Uint32 Ucolor = SDL_MapRGB(surface->format, color.r, color.g, color.b);

	switch(surface->format->BytesPerPixel) {
		case 1:{ // Assuming 8-bpp
			Uint8 *bufp;

			bufp = (Uint8 *) surface->pixels + y*surface->pitch + x;
			*bufp = Ucolor;
		}
		break;
		case 2:{ // Probably 15-bpp or 16-bpp
			Uint16 *bufp;
			bufp = (Uint16 *) surface->pixels + y*surface->pitch/2 + x;
			*bufp = Ucolor;
		}
		break;
		case 3:{ // Slow 24-bpp mode, usually not used
			Uint8 *bufp;

			bufp = (Uint8 *) surface->pixels + y*surface->pitch + x*surface->format->BytesPerPixel;
			*(bufp+surface->format->Rshift/8) = color.r;
			*(bufp+surface->format->Gshift/8) = color.g;
			*(bufp+surface->format->Bshift/8) = color.b;
		}
		break;
		case 4:{ // Probably 32-bpp
			Uint32 *bufp;
			bufp = (Uint32 *) surface->pixels + y*surface->pitch/4 + x;
			*bufp = Ucolor;
		}
		break;
	}
}

void draw_line(SDL_Surface* surface, int x1, int y1, int x2, int y2, SDL_Color color){
	// pres parametrickou rovnici usecky
	// kazdy bod Z usecky AB se spocita
	// Z = A + (B-A)t
	// pro 0<= t <= 1
	int length, w=(x2-x1), h=(y2-y1), t;
	// length= sqrt(w^2+h^2);
		length= w*w + h*h;
		for( t=0 ; t*t<length ; ++t); length=t;
	// vhodne zdrobnely parametr vykresli dostatecne mnoho pixelu
	if(length){
		for( t=0 ; t<=length ; ++t )
			draw_pixel(surface, x1 + w*t/length, y1 + h*t/length ,color);
	} else	draw_pixel(surface, x1, y1 ,color);
}

Uint32 SDL_fps(Uint32 last, Uint32 fps){
	// pocka od casu last tak dlouho abych vykresloval
	// fps obrazku za sekundu
	// vrati aktualni cas ukonceni teto fce
	// poznamka: muze spatne fungovat pokud aplikace bezi
	// dele nez ~49.7 dni kvuli preteceni Uint32
	Uint32 time_elipse= (fps==0) ? 2000 : 1000/fps;
	Uint32 from_last= SDL_GetTicks()-last;
	if(time_elipse>from_last) SDL_Delay(time_elipse-from_last);
// 	else std::cout << time_elipse << "," << from_last << std::endl;
	return SDL_GetTicks();
}

/**************** funkce obstaravajici udalosti *********************/

SDLKey get_event(){
	SDL_Event event;

	while(SDL_PollEvent(&event)){
		switch(event.type){
			// Klavesnice
			case SDL_KEYDOWN:
				switch(event.key.keysym.sym){
					// tyto klavesy se chovaji odlisne
					case SDLK_NUMLOCK: ;
					case SDLK_CAPSLOCK: break;
					default: return event.key.keysym.sym;
				}
				break;
			// Pozadavek na ukonceni
			case SDL_QUIT:
				return SDLK_LAST;
		}
	}
	// zadna potrebna udalost
	return SDLK_FIRST;
}

bool get_event_isquit(SDLKey key){
	SDLKey eventkey;
	while(true){
		eventkey=get_event();
		if(eventkey==key || eventkey==SDLK_LAST)
			return true;
		if(eventkey==SDLK_FIRST)
			return false;
	}
}

// ceka na udalost a vraci stisknutou klavesu
// nebo SDLK_LAST pri pozadavk na ukonceni
SDLKey wait_event(){
	SDL_Event event;

	while(SDL_WaitEvent(&event)){
		switch(event.type){
			// Klavesnice
			case SDL_KEYDOWN:
				switch(event.key.keysym.sym){
					// tyto klavesy se chovaji odlisne
					case SDLK_NUMLOCK: ;
					case SDLK_CAPSLOCK: break;
					default: return event.key.keysym.sym;
				}
				break;
			// Pozadavek na ukonceni
			case SDL_QUIT:
				return SDLK_LAST;
		}
	}
	// problem pri cekani na udalost
	return SDLK_LAST;
}

// vycka na udalost a vraci true pokud to bylo ukonceni
// nebo zmacknuta klavesa key, jinak false
bool wait_event_isquit(SDLKey key){
	SDLKey eventkey;
	eventkey=wait_event();
	if(eventkey==key || eventkey==SDLK_LAST)
		return true;
	else
		return false;
}

double SDL_Rand(){
	double random= rand()+1;
		random= rand()/random;
	return (random<1) ? random : 1/random;
}
/********* END of SDL_lib functions ********************/
