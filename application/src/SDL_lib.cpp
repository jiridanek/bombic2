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
#include "constants.h"
#include "agar_helper.h"
#include "tixml_helper.h"

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

/**
 * @param filename název souboru s fontem
 */
Fonts::Fonts(const char* filename): fontFile(filename), fontMap() {
}

/**
 * @param size požadovaná velikost písma
 * @return pointer na vytvořený font
 */
TTF_Font* Fonts::operator[](Uint16 size){
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

/**
 * @param size velikost fontu pro vytvoření
 * @return pointer na vytvořený font
 */
TTF_Font* Fonts::add(Uint16 size){
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

/**
 * @param sur_SDL SDL_Surface, ze kterého chceme vytvořit nový Surface.
 */
Surface::Surface(SDL_Surface *sur_SDL):
				surface_(sur_SDL){
	// Create a new reference
	references_ = new Uint16(1);
}

/**
 * @param sur Surface, který chceme klonovat.
 */
Surface::Surface(const Surface & sur):
			surface_(sur.surface_), references_(sur.references_){
	++(*references_);
}

Surface::~Surface(){
	decrement_();
}

/** @details
 * Snížit počet referencí, pokud dosáhnou na nulu,
 * dealokovat vnitřní SDL_Surface
 * @return nový počet referencí
 * @see SDL_FreeSurface
 */
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

/**
 * @param sur Surface, který chceme klonovat
 * @return vytvořený Surface
 */
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

/**
 * @param sur_SDL SDL_Surface, ze kterého chceme vytvořit nový Surface.
 * @return vytvořený Surface
 */
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
	std::cerr << "use getSurface instead of GetSurface" << std::endl;
	return surface_;
}
SDL_Surface* Surface::getSurface() const {
	return surface_;
}
Uint16 Surface::width() const {
	return !surface_ ? 0 : surface_->w;
}
Uint16 Surface::height() const {
	return !surface_ ? 0 : surface_->h;
}

/******** END of class Surface **********************/

/*************** class Animation *********************/

Animation::Animation(): cur_frame_(0), draw_shadow_(0),
			frame_period_(1000), last_access_(0), run_counter_(0) {
	frames_.push_back(Surface());
}

/**
 * @see Animation::initialize(), Animation::loadItem_()
 */
Animation::Animation(TiXmlElement* el, Uint16 width, Uint16 height,
			const Surface & sur_src, const Surface & sur_shadow_src) {
	initialize(el, width, height, sur_src, sur_shadow_src);
}

/** @details
 * Nastavuje inicializační hodnoty, parsuje XML element a
 * vytváří z něj animaci.
 * @param el XML element obsahující definici animace.
 * Minimálně musí obsahovat atributy x,y.
 * Typicky obsahuje podelement animation(rate="") s podelementy animation_item(x,y)
 * @param width šířka každého framu animace
 * @param height výška každého framu animace
 * @param sur_src zdrojový obrázek, ze kterého se vyřeže animace
 * @param sur_shadow_src zdrojový obrýzek, ze kterého se vyřežou stíny,
 * defaultně bez stínů.
 * @see Animation::loadItem_()
 */
const Animation & Animation::initialize(TiXmlElement* el,
			Uint16 width, Uint16 height,
			const Surface & sur_src, const Surface & sur_shadow_src){
	// nastaveni defaultnich hodnot
	frames_.clear(); shadow_frames_.clear();
	frame_period_ = 1000; cur_frame_ = last_access_ = run_counter_ = 0;
	draw_shadow_ = sur_shadow_src.getSurface()!=0;

	if(!el) throw std::string("missing element");

	// pruhlednost animace
	Uint16 transparency;
	if(readAttr(el, "transparency", transparency, false)){
		if(transparency > SDL_ALPHA_OPAQUE)
			throw std::string("attribute transparency must be lesser than 255");
		transparency_ = static_cast<Uint8>(transparency);
	}
	else transparency_ = SDL_ALPHA_OPAQUE;

	// atributy
	loadItem_(el, width, height, sur_src, sur_shadow_src);

	// animace
	el= el->FirstChildElement("animation");
	if(!el) return *this; // nemame animaci

	// ulozit rate
	Uint16 frame_rate;
	readAttr(el, "rate", frame_rate);
	if(frame_rate==0)
		throw std::string("attribute rate must be greater than 0");
	frame_period_ /= frame_rate;

	// vsechny slozky animace
	el= el->FirstChildElement("animation_item");
	if(!el)
		throw std::string("missing element <animation_item>");
	while(el){
		loadItem_(el, width, height, sur_src, sur_shadow_src);
		el= el->NextSiblingElement("animation_item");
	}
	return *this;
}

Animation::Animation(const Animation & anim):
	frames_(anim.frames_), shadow_frames_(anim.shadow_frames_),
	cur_frame_(0), draw_shadow_(anim.draw_shadow_),
	frame_period_(anim.frame_period_), last_access_(0), run_counter_(0) {}

const Animation & Animation::operator=(const Animation & anim){
	if(&anim!=this){
		frames_ = anim.frames_; shadow_frames_ = anim.shadow_frames_;
		cur_frame_ = 0; draw_shadow_ = anim.draw_shadow_;
		frame_period_ = anim.frame_period_; last_access_ = run_counter_ = 0;
	}
	return *this;
}

/** @details
 * Parsuje XML element a vytváří z něj konkrétní jeden frame.
 * @param el XML element <animation_item x,y,(shadow_x, shadow_y)>
 * obsahující definici jednoho framu.
 * @param width šířka framu animace
 * @param height výška framu animace
 * @param sur_src zdrojový obrázek, ze kterého se vyřízne frame
 * @param sur_shadow_src zdrojový obrýzek, ze kterého se vyřízne stín.
 * @see readAttr()
 */
void Animation::loadItem_(TiXmlElement* el, Uint16 width, Uint16 height,
			const Surface & sur_src, const Surface & sur_shadow_src){

	// atributy pro surface
	Uint16 x, y;
	readAttr(el, "x", x);
	readAttr(el, "y", y);

	// vytvorit surface
	Surface sur= create_transparent_surface(width, height, false);
	SDL_Rect rect={x,y,width,height};
	SDL_BlitSurface(sur_src.getSurface(), &rect, sur.getSurface(), 0);
	if(transparency_<SDL_ALPHA_OPAQUE)
		set_transparency(sur.getSurface(), transparency_);
	frames_.push_back(sur);

	if(!draw_shadow_) return;

	// atributy pro shadow
	readAttr(el, "shadow_x", x);
	readAttr(el, "shadow_y", y);

	// vytvorit surface
	sur= create_transparent_surface(width, height, true);
	rect.x=x; rect.y=y;
	SDL_BlitSurface(sur_shadow_src.getSurface(), &rect, sur.getSurface(), 0);
	shadow_frames_.push_back(sur);
}

/** @details
 * Nastaví počítadlo na počáteční frame,
 * vynuluje dobu od posledního posunu framu.
 */
void Animation::reset(){
	cur_frame_=0;
	last_access_=0;
}

/** @details
 * Nastaví počítadlo na náhodný frame,
 * vynuluje dobu od posledního posunu framu.
 */
void Animation::random(){
	cur_frame_= rand()%frames_.size();
	last_access_=0;
}

/** @details
 * Zaručí správnou rychlost animace.
 * @return TRUE, pokud se vracíme na začátek animace, jinak FALSE.
 */
bool Animation::update(){
	bool at_end = false;
	for(last_access_+=MOVE_PERIOD;
		last_access_>=frame_period_; last_access_-=frame_period_){
		++cur_frame_;
		if(cur_frame_>=frames_.size()){
			cur_frame_%= frames_.size();
			++run_counter_;
			at_end=true;
		}
	}
	return at_end;
}

/**
 * @return Pořadí běhu po následujícím update.
 * @see update()
 */
Uint16 Animation::run_num() const {
	if( (last_access_+MOVE_PERIOD) >= frame_period_
	&&  (cur_frame_+1) >= frames_.size())
			return run_counter_+1;
	return run_counter_;
}

/**
 * @return Počet period do konce aktuálního běhu animace.
 */
Uint16 Animation::periods_to_end() const {
	return ( frame_period_*(frames_.size()-cur_frame_) - last_access_ )
					/ MOVE_PERIOD;
}

/**
 * @param window surface okna pro vykreslení
 * @param x cílová souřadnice pro vykreslení
 * @param y cílová souřadnice pro vykreslení
 */
void Animation::draw(SDL_Surface* window, Uint16 x, Uint16 y) const {
	if(draw_shadow_){
		draw_surface(x, y, shadow_frames_[cur_frame_].getSurface(), window);
	}
	draw_surface(x, y, frames_[cur_frame_].getSurface(), window);
}

Uint16 Animation::height() const {
	return frames_[0].height();
}

Uint16 Animation::width() const {
	return frames_[0].width();
}

/******** END of class Animation *********************/


/******** SDL_lib functions **************************/

/** @details
 * Inicializuje SDL, nastavuje video mode a velikost okna,
 * inicializuje TTF. Při úspěchu nastavuje zrušení SDL a TTF při ukončení programu.
 * Při neúspěchu vypisuje chybovou hlášku a ukončí program.
 * @see SDL_Init(), SDL_Quit(), TTF_Init(), TTF_Quit()
 * @see exit, atexit
 * @param pWindow pointer na pointer na SDL_Surface okna
 * @param win_w šířka okna
 * @param win_h výška okna
 * @param caption titulek okna
 */
void window_init(SDL_Surface ** pWindow, int win_w, int win_h, const char *caption){
	// Inicializace SDL
	if(SDL_Init(SDL_INIT_VIDEO) == -1){
		std::cerr << "Unable to initialize SDL: " << SDL_GetError() << std::endl;
		exit(1);
	}
	atexit(SDL_Quit);

	// Vytvori okno s definovanymi vlastnostmi
	*pWindow = SDL_SetVideoMode(win_w, win_h, 0,
			SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_RESIZABLE);
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

/**
 * @param font pointer na písmo, kterým se má psát
 * @param str text, který se má vypsat
 * @param color barva textu
 * @return Vrací vytvořené SDL_Surface s textem.
 * @see TTF_RenderUTF8_Blended()
 */
SDL_Surface* get_text(TTF_Font* font, const char* str, SDL_Color color){
        return TTF_RenderUTF8_Blended(font, str, color);
}

/**
 * @param w šířka surface
 * @param h výška surface
 * @param color barva pro vyplnění
 * @return Vrací vytvořené SDL_Surface.
 */
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
/**
 * @param sur surface, kterému chceme nastavit průhlednou barvu
 * @param color průhledná barva
 * @see SDL_SetColorKey()
 */
void set_transparent_color(SDL_Surface *sur, SDL_Color color){
	if(!sur) return;
	SDL_SetColorKey(sur, SDL_SRCCOLORKEY,
		SDL_MapRGB(sur->format, color.r, color.g, color.b));
}

/**
 * @param sur surface, kterému chceme nastavit průhlednost
 * @param alpha míra průhlednosti (0 - průhledné, 255 - neprůhledné)
 * @see SDL_SetAlpha()
 */
void set_transparency(SDL_Surface *sur, Uint8 alpha){
	if(!sur) return;
	SDL_SetAlpha(sur, SDL_SRCALPHA | SDL_RLEACCEL, alpha);
}

/**
 * @param w šířka surface
 * @param h výška surface
 * @param transparent pokud TRUE, bude nastavena poloprůhlednost surface
 * @return Vrací vytvořené SDL_Surface.
 */
SDL_Surface* create_transparent_surface(Uint16 w, Uint16 h, bool transparent){
	// vytvorim surface
	SDL_Surface *sur=create_surface(w, h, Color::transparent);
	// nastavim transparentni barvu
	set_transparent_color(sur, Color::transparent);
	//nastavim pruhlednost
	if(transparent)
		set_transparency(sur, 128);
	return sur;
}


/**
 * @param x souřadnice pro vykreslení
 * @param y souřadnice pro vykreslení
 * @param surface_src zdroj
 * @param surface_dst cíl
 */
void draw_surface(int x, int y, SDL_Surface* surface_src, SDL_Surface* surface_dst){
	// vykresli surface_src do surface_dst
	if(surface_src && surface_dst){
		SDL_Rect rect={x,y,surface_src->w,surface_src->h};
		SDL_BlitSurface(surface_src, 0, surface_dst, &rect);
	}
}

/**
 * @param surface_src zdroj
 * @param surface_dst cíl
 */
void draw_center_surface(SDL_Surface* surface_src, SDL_Surface* surface_dst){
	if(surface_src && surface_dst){
		draw_surface(
			(surface_dst->w - surface_src->w)/2,
			(surface_dst->h - surface_src->h)/2,
			surface_src, surface_dst);
	}
}

/**
 * @param color barva vyplnění
 * @param surface cíl
 */
void clear_surface(SDL_Color color, SDL_Surface* surface){
	// vybarvi surface barvou color
	if(surface){
		SDL_FillRect(surface, 0, SDL_MapRGB(surface->format,
			color.r,color.g,color.b) );
	}
}

/**
 * @param surface cíl
 * @param x souřadnice vykreslovaného pixelu
 * @param y souřadnice
 * @param color barva pixelu
 */
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

/**
 * @param surface cíl
 * @param x1 počáteční souřadnice úsečky
 * @param y1 počáteční souřadnice
 * @param x2 koncové souřadnice úsečky
 * @param y2 koncové souřadnice úsečky
 * @param color barva úsečky
 */
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

/** @details
 * Počká od času last tak dlouho abych vykresloval
 * fps obrázků za sekundu
 * vráti aktualní čas ukončení teto fce
 * poznamka: muze špatně fungovat pokud aplikace běží
 * déle než ~49.7 dní kvůli přetečení Uint32
 * @param last čas posedního volání této fce
 * @param fps počet framu za sekundu kterých chceme docílit
 * @return Vrací aktuální čas použitelný pro další volání.
 */
Uint32 SDL_fps(Uint32 last, Uint32 fps){
	Uint32 time_elipse= (fps==0) ? 2000 : 1000/fps;
	Uint32 from_last= SDL_GetTicks()-last;
	if(time_elipse>from_last) SDL_Delay(time_elipse-from_last);

	return SDL_GetTicks();
}

/**************** funkce obstaravajici udalosti *********************/

static bool interesting_event_(SDL_Event event, SDLKey & key){
	switch(event.type){
		// Klavesnice
		case SDL_KEYDOWN:
			switch(event.key.keysym.sym){
				// tyto klavesy se chovaji odlisne
				case SDLK_NUMLOCK:
				case SDLK_CAPSLOCK:
					return false;
				default:
					key = event.key.keysym.sym;
					return true;
			}
			break;
		// resize okna
		case SDL_VIDEORESIZE:
			AG_ResizeDisplay(event.resize.w, event.resize.h);
			return true;
		// Pozadavek na ukonceni
		case SDL_QUIT:
			return true;
	}
	return false;
}

/** @details
 * Pokud narazí na nějakou zajímavou událost, vrací její typ,
 * když je to událost doplněná o klávesu, změní ji.
 * @param key vrácená hodnota klávesy
 * @return Vrací typ zajímavé události,
 * nebo SDL_USEREVENT když žádná zajímavá událost nenastala.
 */
Uint8 get_event(SDLKey & key){
	SDL_Event event;
	while(SDL_PollEvent(&event)){
		if(interesting_event_(event, key))
			return event.type;
	}
	return SDL_USEREVENT;
}

/** @details
 * Čeká, na zajímavou událost.
 * @param key vrácená hodnota klávesy
 * @return Vrací typ zajímavé události nebo SDL_USEREVENT při problemu.
 */
Uint8 wait_event(SDLKey & key){
	SDL_Event event;
	while(SDL_WaitEvent(&event)){
		if(interesting_event_(event, key))
			return event.type;
	}
	std::cerr << "waitevent cant wait" << std::endl;
	// problem pri cekani na udalost
	return SDL_USEREVENT;
}

/**
 * @return (double) náhodné číslo z intervalu [0,1]
 */
double SDL_Rand(){
	double random= rand()+1;
		random= rand()/random;
	return (random<=1) ? random : 1/random;
}
/********* END of SDL_lib functions ********************/
