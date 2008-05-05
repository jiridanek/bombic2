#ifndef BOMBIC_
#define BOMBIC_

#include <iostream>
#include <vector>
#include <utility>
#include <string>

#include "SDL_lib.h"
#include "tinyxml.h"

class Bombic {
	public:
		// konstruktor vytvori z xml souboru instanci bombice
		// nacte z disku jeho obrazky
		Bombic(Uint8 fps_global);
		// zahodi nactene obrazky
		~Bombic();
		// pohne s bombicem podle stisknutych klaves a rozmeru okna
		void move(Uint8 *keystate, SDL_Surface *window);
		// zabije bombice
		void kill();
		// nakresli bombice do okna
		void draw(SDL_Surface *window);

	private:
		enum directions_t { LEFT, RIGHT, UP, DOWN, BURNED };
		const std::string direction2str_(directions_t direction) const;
		// cache obrazkovych animaci
		typedef std::vector<SDL_Surface*> images_t;
		images_t images_left_, images_right_, images_up_,
				images_down_, images_burned_, images_src_,
				images_shadow_left_, images_shadow_right_,
				images_shadow_up_, images_shadow_down_;
		// fps pro jednotlive animace
		Uint8 fps_left_, fps_right_, fps_up_,
			fps_down_, fps_burned_, fps_global_;
		// krok (faze) animace
		std::pair<directions_t, Uint16> step_;
		// za kolik tiku bude dalsi krok animace
		Uint16 next_;

		// vraci true pokud byl specifikovany stin
		bool is_shadow_() const;
		// spracuje vnorene elementy
		void subElement_(TiXmlElement* El, directions_t direction);
		void loadItem_(TiXmlElement *subEl, directions_t direction);

		// souradnice
		double x_, y_, speed_;
		int height_, width_;
		// vytiskne chybu a ukonci program
		template<typename T>
			void TiXmlError_(T error);
		// vraci uspech akce, nastavi outValue pokud byl nalezen atribut name
		int QueryStringAttribute(TiXmlElement* El,
				const char* name, std::string* outValue);
		// destruuje vsechny prvky z images_
		void destroy_(images_t & images_);
};

// vytiskne chybu a ukonci program
template<typename T> void Bombic::TiXmlError_(T error){
	cerr << "Chyba v XML souboru: " << error << endl;
	exit(1);
}

#endif
