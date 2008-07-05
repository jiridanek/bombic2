
#include <iostream>
#include <list>
#include <vector>
#include <string>
#include "SDL_lib.h"
#include "tixml_helper.h"
#include "game.h"
#include "game_background.h"
#include "game_wall.h"
#include "game.h"
#include "game.h"

using namespace std;

/*************** class Game ******************************/
/// Inicializace hry.
Game::Game(Uint8 players_count, const std::string & mapname){
// 	bool deathmatch, bool creatures, bool bombsatend){
	load_map_(mapname);
}

void Game::load_map_(const std::string & mapname){
	// nacteni hodnot z xml
	TiXmlDocument doc;
	TiXmlElement *map_el;
	string filename(mapname);
	map_el = TiXmlRootElement(doc, filename, "map", true);

	// atributy
	int height, width;
	switch(map_el->QueryIntAttribute("height", &height)){
		case TIXML_SUCCESS:
			if(height<=2)
				TiXmlError(filename,"atribut height musí být větší než 2.");
			break;
		case TIXML_WRONG_TYPE:
			TiXmlError(filename,"atribut height není typu int.");
			break;
		case TIXML_NO_ATTRIBUTE:
			TiXmlError(filename,"chybějící atribut height v <map ...>.");
			break;
	}
	switch(map_el->QueryIntAttribute("width", &width)){
		case TIXML_SUCCESS:
			if(width<=2)
				TiXmlError(filename,"atribut width musí být větší než 2.");
			break;
		case TIXML_WRONG_TYPE:
			TiXmlError(filename,"atribut width není typu int.");
			break;
		case TIXML_NO_ATTRIBUTE:
			TiXmlError(filename,"chybějící atribut width v <map ...>.");
			break;
	}
	// vytvoreni zakladniho rozmeru mapy
	map_array_t::value_type column;
	map_array_t::value_type::value_type empty_list;
	column.insert(column.end(), height, empty_list);
	map_array_.insert(map_array_.end(), width, column);

	// nacteni pozadi
	string str;

	switch(QueryStringAttribute(map_el,"background", &str)){
		case TIXML_SUCCESS:
			// nacteni pozadi mapy
			load_background_(str);
			break;
		case TIXML_NO_ATTRIBUTE:
			TiXmlError(filename,"chybějící atribut background v <map ...>");
			break;
	}

	// nacteni zdi
	load_walls_(map_el->FirstChildElement("walls"));



}

void Game::load_background_(const std::string & bgname){
	// nacteni hodnot z xml
	TiXmlDocument doc;
	TiXmlElement *bg_el;
	string filename(bgname);
	bg_el = TiXmlRootElement(doc, filename, "background", true);

	// atributy pozadi
	string str;
	SDL_Surface *sur_src;

	switch(QueryStringAttribute(bg_el,"src", &str)){
		case TIXML_SUCCESS:
			sur_src=SDL_LoadBMP(str.c_str());
			if(!sur_src)
				TiXmlError(filename,"hodnota atributu src není cesta k souboru s BMP.");
			// nastavim pruhlednost
			set_transparent_color(sur_src, Colors::transparent());
			break;
		case TIXML_NO_ATTRIBUTE:
			TiXmlError(filename,"chybějící atribut name v <map ...>");
			break;
	}
	// atributy vnorenych elementu
	int toplapping;
	Surface sur1, sur2;
// 	map_array_::iterator col_it;
// 	map_array_::value_type::iterator field_it;
	Uint16 column, field, width, height;
	try{
		// pozadi
		sur1 = load_subEl_surface_(bg_el, "clean_bg", toplapping, sur_src);
		sur2 = load_subEl_surface_(bg_el, "burned_bg", toplapping, sur_src);
		for(column=0 ; column< map_array_.size() ; ++column){
			for(field = 0 ; field<map_array_[column].size() ; ++field){
				// vytvorit a ulozit do seznamu statickych objektu
				staticMOs_.push_back(new Background(sur1,sur2, column, field) );
				// ulozit do mapy na spravne policko
				map_array_[column][field].push_back(staticMOs_.back());
			}
		}
		// rohy
		sur1 = load_subEl_surface_(bg_el, "topleft", toplapping, sur_src);
		insert_wall_(sur1, 0, 0, 0);

		sur1 = load_subEl_surface_(bg_el, "topright", toplapping, sur_src);
		insert_wall_(sur1, 0,
				map_array_.size()-sur1.width()/CELL_SIZE, 0);

		sur1 = load_subEl_surface_(bg_el, "bottomleft", toplapping, sur_src);
		insert_wall_(sur1, toplapping, 0,
				map_array_[0].size()-(sur1.height()-toplapping)/CELL_SIZE);

		sur1 = load_subEl_surface_(bg_el, "bottomright", toplapping, sur_src);
		insert_wall_(sur1, toplapping,
				map_array_.size()-sur1.width()/CELL_SIZE,
				map_array_[0].size()-(sur1.height()-toplapping)/CELL_SIZE);

		// strany
		sur1 = load_subEl_surface_(bg_el, "top", toplapping, sur_src);
		for(column=0 ; column< map_array_.size() ; ++column){
			if(map_array_[column][0].back()->type()
					==string("Wall")) continue;
				insert_wall_(sur1, 0, column, 0);
		}

		sur1 = load_subEl_surface_(bg_el, "left", toplapping, sur_src);
		for(field = 0 ; field<map_array_[0].size() ; ++field){
			if(map_array_[0][field].back()->type()
				==string("Wall")) continue;
			insert_wall_(sur1, toplapping, 0, field);
		}

		sur1 = load_subEl_surface_(bg_el, "bottom", toplapping, sur_src);
		field= map_array_[0].size()-(sur1.height()-toplapping)/CELL_SIZE;
		for(column=0 ; column< map_array_.size() ; ++column){
			if(map_array_[column][field].back()->type()
				==string("Wall")) continue;
			insert_wall_(sur1, toplapping, column, field);
		}

		sur1 = load_subEl_surface_(bg_el, "right", toplapping, sur_src);
		column = map_array_.size()-sur1.width()/CELL_SIZE;
		for(field = 0 ; field<map_array_[column].size() ; ++field){
			if(map_array_[column][field].back()->type()
				==string("Wall")) continue;
			insert_wall_(sur1, toplapping, column, field);
		}

		SDL_FreeSurface(sur_src);
	}
	catch(string s){
		SDL_FreeSurface(sur_src);
		TiXmlError(filename,s);
	}
}

void Game::load_walls_(TiXmlElement *wallsEl){
	// nenalezen nepovinny tag walls
	if(!wallsEl) return;

	TiXmlDocument doc;
	TiXmlElement *rootEl;

	// nacteni atributu
	string name_def, filename, str;
	SDL_Surface *sur_SDL;
	Surface sur_def, sur;
	int toplapping_def, height_def, width_def,
		toplapping, height, width, x, y;
	// TODO overcrossing

	load_wall_(wallsEl, name_def, height_def, width_def);
	if(name_def.empty())
		TiXmlError("atribut name u <walls ...> chybí.");
	if(height_def<1) height_def=1;
	if(width_def<1) width_def=1;
	// nacteni defaultni zdi
	filename= name_def;
	rootEl = TiXmlRootElement(doc, filename, "wall", true);
	switch(QueryStringAttribute(rootEl,"src", &str)){
		case TIXML_SUCCESS:
			// zdrojovy  obrazek z disku
			sur_SDL=SDL_LoadBMP(str.c_str());
			if(!sur_SDL)
				TiXmlError(filename,"hodnota atributu src není cesta k souboru s BMP.");
			// nastavim pruhlednost
			set_transparent_color(sur_SDL, Colors::transparent());
			// vytvorim defaultni surface
try{
			sur_def= load_subEl_surface_(rootEl, "img", toplapping_def, sur_SDL);
			SDL_FreeSurface(sur_SDL);
}
			catch(string s){
				SDL_FreeSurface(sur_SDL);
				TiXmlError(filename,s);
			}
			break;
		case TIXML_NO_ATTRIBUTE:
			TiXmlError(filename,"chybějící atribut src");
			break;
	}
	// kontrola jestli koresponduji rozmery obrazku
	if(height_def!=(sur_def.height()-toplapping_def)/CELL_SIZE)
		TiXmlError(filename,"hodnota atributu height neodpovídá hodnotě v elementu <walls ...>");

	if(width_def!= sur_def.width()/CELL_SIZE)
		TiXmlError(filename,"hodnota atributu width neodpovídá hodnotě v elementu <walls ...>");
	// vytvoreni zdi
	wallsEl= wallsEl->FirstChildElement("wall");
	try{
		while(wallsEl){
			load_wall_(wallsEl, filename, height, width, x, y);
			if(height<1) height=height_def;
			if(width<1) width=width_def;
			// nacteni nove zdi
			if(!filename.empty() && filename!=name_def){
				rootEl = TiXmlRootElement(doc, filename, "wall", true);
				switch(QueryStringAttribute(rootEl,"src", &str)){
					case TIXML_SUCCESS:
						// zdrojovy  obrazek z disku
						sur_SDL=SDL_LoadBMP(str.c_str());
						if(!sur_SDL)
							TiXmlError(filename,"hodnota atributu src není cesta k souboru s BMP.");
						// nastavim pruhlednost
						set_transparent_color(sur_SDL, Colors::transparent());
						// vytvorim defaultni surface
						sur= load_subEl_surface_(rootEl, "img", toplapping, sur_SDL);
						SDL_FreeSurface(sur_SDL);
						break;
					case TIXML_NO_ATTRIBUTE:
						TiXmlError(filename,"chybějící atribut src");
						break;
				}
				// kontrola jestli koresponduji rozmery obrazku
				if(height!=(sur.height()-toplapping)/CELL_SIZE)
					TiXmlError(filename,"hodnota atributu height neodpovídá hodnotě v elementu <walls ...>");

				if(width!= sur.width()/CELL_SIZE)
					TiXmlError(filename,"hodnota atributu width neodpovídá hodnotě v elementu <walls ...>");
			}
			else{
				sur= sur_def;
				toplapping= toplapping_def;
			}
			insert_wall_(sur, toplapping, x, y);
			wallsEl= wallsEl->NextSiblingElement("wall");
		}
	}
	catch(string s){
		SDL_FreeSurface(sur_SDL);
		TiXmlError(filename,s);
	}

}

/**
 * @details
 * Naplní parametry mimo prvního hodnotami atributů.
 * Pokud atribut nenajde uloží do parametru -1 (nebo empty string).
 * @param wallEl element jehož hodnoty atributů hledáme
 * @param name hodnota stejnojmenneho atributu nebo prazdny string
 * @param height hodnota stejnojmenneho atributu nebo -1
 * @param width hodnota stejnojmenneho atributu nebo -1
 */
void Game::load_wall_(TiXmlElement *wallEl, std::string & name,
			int & height, int & width){
	if(!wallEl){
		name.erase();
		height = width = -1;
		return;
	}

	switch(QueryStringAttribute(wallEl,"name", &name)){
		case TIXML_SUCCESS:
			if(name.empty())
				TiXmlError("atribut name u <walls ...> nesmí být prázdný.");
			break;
		case TIXML_NO_ATTRIBUTE:
			name.erase();
			break;
	}

	switch(wallEl->QueryIntAttribute("height", &height)){
		case TIXML_SUCCESS:
			if(height<=0)
				TiXmlError("atribut height u <walls ...> musí být větší než 0.");
			break;
		case TIXML_WRONG_TYPE:
				TiXmlError("atribut height u <walls ...> není typu int.");
			break;
		case TIXML_NO_ATTRIBUTE:
			height= -1;
			break;
	}

	switch(wallEl->QueryIntAttribute("width", &width)){
		case TIXML_SUCCESS:
			if(width<=0)
				TiXmlError("atribut width u <walls ...> musí být větší než 0.");
			break;
		case TIXML_WRONG_TYPE:
				TiXmlError("atribut width u <walls ...> není typu int.");
			break;
		case TIXML_NO_ATTRIBUTE:
			width= -1;
			break;
	}
}

/** @details
 * @see load_wall_()
 * @param x hodnota atributu x, pokud není, vyvolá chybu TiXmlError()
 * @param y hodnota atributu y, pokud není, vyvolá chybu TiXmlError()
 */
void Game::load_wall_(TiXmlElement *wallEl, std::string & name,
			int & height, int & width, int & x, int & y){
	load_wall_(wallEl, name, height, width);
	if(!wallEl){
		x = y = -1;
		return;
	}
	switch(wallEl->QueryIntAttribute("x", &x)){
		case TIXML_SUCCESS:
			if(x<0)
				TiXmlError("atribut x u <walls ...> musí být nezáporný.");
			break;
		case TIXML_WRONG_TYPE:
				TiXmlError("atribut x u <walls ...> není typu int.");
			break;
		case TIXML_NO_ATTRIBUTE:
				TiXmlError("atribut x u <walls ...> chybí.");
			break;
	}

	switch(wallEl->QueryIntAttribute("y", &y)){
		case TIXML_SUCCESS:
			if(y<0)
				TiXmlError("atribut y u <walls ...> musí být nezáporný.");
			break;
		case TIXML_WRONG_TYPE:
				TiXmlError("atribut y u <walls ...> není typu int.");
			break;
		case TIXML_NO_ATTRIBUTE:
			TiXmlError("atribut y u <walls ...> chybí.");
			break;
	}
}
/** @details
 * @return Vrací SDL_Surface objektu popsaného podelementem se zadaným jménem.
 * Vrácené surface má nastavenou průhlednou barvu, nikoli však průhlednost.
 * Zjistí také jaké měl podelement nastavené toplapping (výška nevyužitá v mapě).
 * Při chybě (nenalezení podelementu nebo některého povinného atributu)
 * vyvolá výjimku typu string s chybovým hlášením.
 * @param El rodič hledaného podelementu
 * @param name_subEl název hledaného podelementu
 * @param toplapping reference na int, do které se přiřadí nalezená (nebo defaultní) hodnota
 * @param sur_src zdrojový surface, ze kterého se vyjme výsledný surface
 * @return Výsledný surface patřící nalezenému podelementu.
 * @see subElement(), attrIntValue()
 */
SDL_Surface* Game::load_subEl_surface_(TiXmlElement *El, const char* name_subEl,
				int & toplapping, SDL_Surface* sur_src){
	attr_map_t attr_map;
	SDL_Rect rect;
	SDL_Surface* sur_dst;
	int x, y, w, h;
	subElement(El,name_subEl,attr_map);
	if(!attrIntValue("x",x,attr_map))
		throw string("chybějící atribut x");
	else	rect.x= static_cast<Sint16>(x);
	if(!attrIntValue("y",y,attr_map))
		throw string("chybějící atribut y");
	else	rect.y= static_cast<Sint16>(y);
	if(!attrIntValue("width",w,attr_map))
			rect.w= CELL_SIZE;
	else if(w<1)
		throw string("atribut width musí mít kladnou hodnotu");
	else	rect.w= CELL_SIZE*static_cast<Uint16>(w);
	if(!attrIntValue("height",h,attr_map))
			rect.h= CELL_SIZE;
	else if(h<1)
		throw string("atribut height musí mít kladnou hodnotu");
	else	rect.h= CELL_SIZE*static_cast<Uint16>(h);
	if(!attrIntValue("toplapping",toplapping,attr_map))
		toplapping=0;
	else if(toplapping<0)
		throw string("atribut toplapping musí mít nezápornou hodnotu");
	else	rect.h+= CELL_SIZE*toplapping;

	// preneseni obrazku do noveho surface
	sur_dst= create_transparent_surface(rect.w, rect.h, false);
	SDL_BlitSurface(sur_src, &rect, sur_dst, 0);

	return sur_dst;
}

/** @details
 * Vytvoří zed na zadaných souřadnicích a vloží ji do mapy.
 * Zed vkládá do mapy na všechna políčka, které zabírá.
 * @param sur surface vkládané zdi
 * @param toplapping počet políček odshora obrázku,
 * které nezabírají místo v mapě
 * @param x souřadnice levého rohu zdi v mapě
 * @param y souřadnice horního rohu zdi v mapě
 */
void Game::insert_wall_(const Surface & sur,
				Uint16 toplapping, Uint16 x, Uint16 y){
	// vytvorit a ulozit do seznamu statickych objektu
	staticMOs_.push_back(new Wall(sur, x, y-toplapping) );
	// ulozit do mapy na spravna policka
	for(Uint16 column=0; column<sur.width()/CELL_SIZE ; ++column){
		for(Uint16 field=0 ; field<(sur.height()-toplapping)/CELL_SIZE
							; ++field){
			if(x+column>=map_array_.size() || y+field>=map_array_[0].size())
				continue;
			map_array_[x+column][y+field].push_back(staticMOs_.back());
		}
	}
}

Game::~Game(){
	for(int i=0; i< staticMOs_.size() ; ++i){
		delete (staticMOs_[i]);
	}
	// TODO dynamicMOs_
}

void Game::draw(SDL_Surface* window){
	Uint16 column=0, field;
	// pozadi
	map_array_t::value_type::value_type::iterator it;
	for(field = 0 ; field<map_array_[column].size() ; ++field){
		for(column=0 ; column< map_array_.size() ; ++column){
			for(it= map_array_[column][field].begin() ;
					it!= map_array_[column][field].end() ;
					++it){
				(*it)->draw(window);

			}
		}
		column=0;
	}
}

/// Nastavení parametrů hráče.
void Game::set_player(Uint8 player_num, Uint8 lives,
	Uint8 bombs, Uint8 flames, Uint8 boots){
}
/// Spuštění hry.
void Game::play(){
	std::cout << "Play" << std::endl;
}
/// Info o ukončení hry.
bool Game::success() const{
	return true;
}
/// Info o hráči.
void Game::player(Uint8 player_num, Uint8 & lives,
	Uint8 & bombs, Uint8 & flames, Uint8 & boots) const{
}


/*
/// Seznam dynamických objektů mapy.
std::list<DynamicMO> dynamicMOs_;
/// Seznam statických objektů mapy.
std::vector<StaticMO> staticMOs_;
/// Dvourozměrné pole mapy se seznamem objektů na něm položených.
std::vector< std::vector< std::list< MapObject* > > > map_array_;
/*************** END OF class Game ******************************/
