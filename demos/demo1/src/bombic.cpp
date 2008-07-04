
#include <iostream>
#include <vector>
#include <string>

using namespace std;

#include "bombic.h"
#include "tinyxml.h"

/*********************** class Bombic *******************/

// konstruktor vytvori z xml souboru instanci bombice
// nacte z disku jeho obrazky
Bombic::Bombic(Uint8 fps_global): fps_global_(fps_global),
			step_(DOWN,0), next_(0), x_(0), y_(0), speed_(1.5),
			// defaultne bez animace, kazdopadne musi byt >0
			fps_up_(1), fps_down_(1), fps_right_(1), fps_left_(1), fps_burned_(1){

	// nacteni hodnot z xml
	TiXmlDocument doc("xml/creature.xml");
	if(!doc.LoadFile())
		TiXmlError_("nelze otevřít XML soubor, nebo není syntakticky správný");

	TiXmlElement *creature;

	creature= doc.FirstChildElement("creature");
	if(!creature)
		TiXmlError_("root element musí být <creature ...>");

	// atributy creature
	string str;

	switch(QueryStringAttribute(creature, "name", &str)){
		case TIXML_SUCCESS:
			if(str!="bombic")
				TiXmlError_("xml soubor nespecifikuje bombiče");
			break;
		case TIXML_NO_ATTRIBUTE:
			TiXmlError_("chybějící atribut name v <creature ...>");
			break;
	}
	switch(QueryStringAttribute(creature, "src", &str)){
		case TIXML_SUCCESS:
			images_src_.push_back(SDL_LoadBMP(str.c_str()));
			if(!images_src_.back())
				TiXmlError_("hodnota atributu src není cesta k souboru s BMP");
			// nastavim pruhlednost
			set_transparent_color(images_src_.back(), Colors::transparent());
			break;
		case TIXML_NO_ATTRIBUTE:
			TiXmlError_("chybějící atribut src v <creature ...>");
			break;
	}
	switch(QueryStringAttribute(creature, "shadow_src", &str)){
		case TIXML_SUCCESS:
			images_src_.push_back(SDL_LoadBMP(str.c_str()));
			if(!images_src_.back())
				TiXmlError_("hodnota atributu src_shadow není cesta k souboru s BMP");
			// nastavim pruhlednost
			set_transparent_color(images_src_.back(), Colors::transparent());
			break;
	}
	switch(creature->QueryIntAttribute("height", &height_)){
		case TIXML_SUCCESS:
			if(height_<=0)
				TiXmlError_("atribut height musí mít kladnou hodnotu");
			break;
		case TIXML_WRONG_TYPE:
			TiXmlError_("atribut height v <creature ...> není typu int");
			break;
		case TIXML_NO_ATTRIBUTE:
			TiXmlError_("chybějící atribut height v <creature ...>");
			break;
	}
	switch(creature->QueryIntAttribute("width", &width_)){
		case TIXML_SUCCESS:
			if(width_<=0)
				TiXmlError_("atribut width musí mít kladnou hodnotu");
			break;
		case TIXML_WRONG_TYPE:
			TiXmlError_("atribut width v <creature ...> není typu int");
			break;
		case TIXML_NO_ATTRIBUTE:
			TiXmlError_("chybějící atribut width v <creature ...>");
			break;
	}

	// vnorene tagy
	TiXmlElement *subEl;

	subElement_(creature, UP);
	subElement_(creature, DOWN);
	subElement_(creature, LEFT);
	subElement_(creature, RIGHT);
	subElement_(creature, BURNED);

}

// element subEl je povinny
// fce precte jeho atributy a pripadne vnorenou animaci a udela cache obrazku
void Bombic::subElement_(TiXmlElement *Element, directions_t direction){
	TiXmlElement *subEl= Element->FirstChildElement(direction2str_(direction).c_str());
	if(!subEl)
		TiXmlError_("chybějící element <"
				+direction2str_(direction)+" ...>");
	// atributy
	loadItem_(subEl, direction);

	// animace
	int rate;
	string err_rate=("atribut rate v <"+direction2str_(direction)+" ...><animation ...>");

	TiXmlElement *animEl= subEl->FirstChildElement("animation");
	if(!animEl) return; // nemame animaci
	switch(animEl->QueryIntAttribute("rate", &rate)){
		case TIXML_SUCCESS:
			if(rate<=0)
				TiXmlError_(err_rate+" musí mít kladnou hodnotu");
			break;
		case TIXML_WRONG_TYPE:
			TiXmlError_(err_rate+" není typu int");
			break;
		case TIXML_NO_ATTRIBUTE:
			TiXmlError_(err_rate+" je povinný a chybí");
			break;
	}
	// vsechny slozky animace
	TiXmlElement *animItem= animEl->FirstChildElement("animation_item");
	if(!animItem)
		TiXmlError_("alespoň jeden element <animation_item ...> v <"
				+direction2str_(direction)+" ...> v <animation ...> je povinný");
	while(animItem){
		loadItem_(animItem, direction);
		animItem= animItem->NextSiblingElement("animation_item");
	}
	// ulozit rate
	switch(direction){
		case UP: fps_up_=rate; break;
		case DOWN: fps_down_=rate; break;
		case LEFT: fps_left_=rate; break;
		case RIGHT: fps_right_=rate; break;
		case BURNED: fps_burned_=rate; break;
	}
}

void Bombic::loadItem_(TiXmlElement *subEl, directions_t direction){
	int x, y, shadow_x=-1, shadow_y=-1;
	string err_x("atribut x v <"+direction2str_(direction)+" ...>");
	string err_y("atribut y v <"+direction2str_(direction)+" ...>");
	string err_shadow_x("atribut shadow_x v <"+direction2str_(direction)+" ...>");
	string err_shadow_y("atribut shadow_y v <"+direction2str_(direction)+" ...>");
	switch(subEl->QueryIntAttribute("x", &x)){
		case TIXML_SUCCESS:
			if(x<0)
				TiXmlError_(err_x+" musi mit nezapornou hodnotu");
			break;
		case TIXML_WRONG_TYPE:
			TiXmlError_(err_x+" neni typu int");
			break;
		case TIXML_NO_ATTRIBUTE:
			TiXmlError_(err_x+" je povinny a chybi");
			break;
	}
	switch(subEl->QueryIntAttribute("y", &y)){
		case TIXML_SUCCESS:
			if(y<0)
				TiXmlError_(err_y+" musi mit nezapornou hodnotu");
			break;
		case TIXML_WRONG_TYPE:
			TiXmlError_(err_y+" neni typu int");
			break;
		case TIXML_NO_ATTRIBUTE:
			TiXmlError_(err_y+" je povinny a chybi");
			break;
	}
	// vytvorit surface
	SDL_Surface *sur= create_transparent_surface(width_, height_, false);
	SDL_Rect rect={x,y,width_,height_};
	SDL_BlitSurface(images_src_[0], &rect, sur, 0);

	SDL_Surface *shadow_sur;
	if(is_shadow_() && direction!=BURNED){
		switch(subEl->QueryIntAttribute("shadow_x", &shadow_x)){
			case TIXML_SUCCESS:
				if(shadow_x<0)
					TiXmlError_(err_shadow_x+" musi mit nezapornou hodnotu");
				break;
			case TIXML_WRONG_TYPE:
				TiXmlError_(err_shadow_x+" neni typu int");
				break;
			case TIXML_NO_ATTRIBUTE:
				TiXmlError_(err_shadow_x+" je povinny a chybi");
				break;
		}
		switch(subEl->QueryIntAttribute("shadow_y", &shadow_y)){
			case TIXML_SUCCESS:
				if(shadow_y<0)
					TiXmlError_(err_shadow_y+" musi mit nezapornou hodnotu");
				break;
			case TIXML_WRONG_TYPE:
				TiXmlError_(err_shadow_y+" neni typu int");
				break;
			case TIXML_NO_ATTRIBUTE:
				TiXmlError_(err_shadow_y+" je povinny a chybi");
				break;
		}
		// vytvorit surface se stinem
		shadow_sur= create_transparent_surface(width_, height_, true);
		rect.x=shadow_x; rect.y=shadow_y;
		SDL_BlitSurface(images_src_[1], &rect, shadow_sur, 0);
	}
	// ulozit do cache
	switch(direction){
		case UP:
			images_up_.push_back(sur);
			if(is_shadow_())
				images_shadow_up_.push_back(shadow_sur);
			break;
		case DOWN:
			images_down_.push_back(sur);
			if(is_shadow_())
				images_shadow_down_.push_back(shadow_sur);
			break;
		case LEFT:
			images_left_.push_back(sur);
			if(is_shadow_())
				images_shadow_left_.push_back(shadow_sur);
			break;
		case RIGHT:
			images_right_.push_back(sur);
			if(is_shadow_())
				images_shadow_right_.push_back(shadow_sur);
			break;
		case BURNED:
			images_burned_.push_back(sur);
			break;
	}
}

// zahodi nactene obrazky
Bombic::~Bombic(){
	destroy_(images_left_);
	destroy_(images_shadow_left_);
	destroy_(images_right_);
	destroy_(images_shadow_right_);
	destroy_(images_up_);
	destroy_(images_shadow_up_);
	destroy_(images_down_);
	destroy_(images_shadow_down_);
	destroy_(images_burned_);
	destroy_(images_src_);
}
void Bombic::destroy_(images_t & images_){
	images_t::iterator it;
	for(it=images_.begin() ; it!=images_.end() ; ++it){
		SDL_FreeSurface(*it);
	}
}
// pohne s bombicem podle stisknutych klaves a rozmeru okna
void Bombic::move(Uint8 *keystate, SDL_Surface *window){
	// kvuli podteceni
	if(next_) --next_;
	if(step_.first==BURNED){
		if(!next_){
			next_=fps_global_/fps_burned_;
			if(step_.second<images_burned_.size())
				step_.second++;
		}
		return;
	}
	if(keystate[SDLK_LEFT]){
		if(x_>0) x_-=speed_;
		if(step_.first!=LEFT || !next_){
			next_=fps_global_/fps_left_;
			step_.first=LEFT;
			step_.second++;
		}
		return;
	}
	if(keystate[SDLK_RIGHT]){
		if(x_<window->w-width_) x_+=speed_;
		if(step_.first!=RIGHT || !next_){
			next_=fps_global_/fps_right_;
			step_.first=RIGHT;
			step_.second++;
		}
		return;
	}
	if(keystate[SDLK_UP]){
		if(y_>0) y_-=speed_;
		if(step_.first!=UP || !next_){
			next_=fps_global_/fps_up_;
			step_.first=UP;
			step_.second++;
		}
		return;
	}
	if(keystate[SDLK_DOWN]){
		if(y_<window->h-height_) y_+=speed_;
		if(step_.first!=DOWN || !next_){
			next_=fps_global_/fps_down_;
			step_.first=DOWN;
			step_.second++;
		}
		return;
	}
	if(keystate[SDLK_k]){
		kill();
	}
}
// zabije bombice
void Bombic::kill(){
	if(step_.first!=BURNED){
		step_.first=BURNED;
		step_.second=0;
	}
}
// nakresli bombice do okna
void Bombic::draw(SDL_Surface *window){
	SDL_Surface *sur, *shadow_sur=0;
	// kdyz se nehyba stoji
	if(!next_) step_.second=0;

	switch(step_.first){
		case LEFT:
			step_.second %= images_left_.size();
			sur=images_left_[step_.second];
			if(is_shadow_())
				shadow_sur=images_shadow_left_[step_.second];
			break;
		case RIGHT:
			step_.second %= images_right_.size();
			sur=images_right_[step_.second];
			if(is_shadow_())
				shadow_sur=images_shadow_right_[step_.second];
			break;
		case UP:
			step_.second %= images_up_.size();
			sur=images_up_[step_.second];
			if(is_shadow_())
				shadow_sur=images_shadow_up_[step_.second];
			break;
		case DOWN:
			step_.second %= images_down_.size();
			sur=images_down_[step_.second];
			if(is_shadow_())
				shadow_sur=images_shadow_down_[step_.second];
			break;
		case BURNED:
			if(step_.second >= images_burned_.size())
				return;
			sur=images_burned_[step_.second];
			break;
	}

	// vykreslit stin
	if(shadow_sur)
		draw_surface(static_cast<int>(x_), static_cast<int>(y_), shadow_sur, window);
	//vykreslit postavu
	draw_surface(static_cast<int>(x_), static_cast<int>(y_), sur, window);
}

// vraci string s nazvem strany (a tagu)
const string Bombic::direction2str_(directions_t direction) const{
	switch(direction){
		case UP: return "up";
		case DOWN: return "down";
		case LEFT: return "left";
		case RIGHT: return "right";
		case BURNED: return "burned";
		default: return "";
	}
}

bool Bombic::is_shadow_() const{
	return images_src_.size()>1;
}

int Bombic::QueryStringAttribute(TiXmlElement* El,
			const char* name, string* outValue){
	const char* pStr;
	pStr=El->Attribute(name);
	if(!pStr) return TIXML_NO_ATTRIBUTE;
	(*outValue)=pStr;
	return TIXML_SUCCESS;
}
/********************* end of class Bombic ***************/
