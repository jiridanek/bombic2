
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
	string filename("xml/"+mapname+".xml");
	TiXmlDocument doc(filename.c_str());

	if(!doc.LoadFile())
		TiXmlError("nelze otevřít XML soubor "+filename+", nebo není syntakticky správný.");

	TiXmlElement *map_el;

	map_el= doc.FirstChildElement("map");
	if(!map_el)
		TiXmlError("root element musí být <map ...>.");

	// atributy mapy
	string str;

	switch(QueryStringAttribute(map_el,"name", &str)){
		case TIXML_SUCCESS:
			if(str!=mapname)
				TiXmlError("chybné jméno mapy v souboru "+filename);
			break;
		case TIXML_NO_ATTRIBUTE:
			TiXmlError("chybějící atribut name v <map ...>");
			break;
	}

	int height, width;
	switch(map_el->QueryIntAttribute("height", &height)){
		case TIXML_SUCCESS:
			if(height<=2)
				TiXmlError("atribut height musí být větší než 2.");
			break;
		case TIXML_WRONG_TYPE:
			TiXmlError("atribut height není typu int.");
			break;
		case TIXML_NO_ATTRIBUTE:
			TiXmlError("chybějící atribut height v <map ...>.");
			break;
	}
	switch(map_el->QueryIntAttribute("width", &width)){
		case TIXML_SUCCESS:
			if(width<=2)
				TiXmlError("atribut width musí být větší než 2.");
			break;
		case TIXML_WRONG_TYPE:
			TiXmlError("atribut width není typu int.");
			break;
		case TIXML_NO_ATTRIBUTE:
			TiXmlError("chybějící atribut width v <map ...>.");
			break;
	}
	// vytvoreni zakladniho rozmeru mapy
	map_array_t::value_type column;
	map_array_t::value_type::value_type empty_list;
	column.insert(column.end(), height, empty_list);
	map_array_.insert(map_array_.end(), width, column);

	// nacteni pozadi
	switch(QueryStringAttribute(map_el,"background", &str)){
		case TIXML_SUCCESS:
			// nacteni pozadi mapy
			load_background_(str);
			break;
		case TIXML_NO_ATTRIBUTE:
			TiXmlError("chybějící atribut name v <map ...>");
			break;
	}

}

void Game::load_background_(const std::string & bgname){
	// nacteni hodnot z xml
	string filename("xml/"+bgname+".xml");
	TiXmlDocument doc(filename.c_str());

	if(!doc.LoadFile())
		TiXmlError("nelze otevřít XML soubor "+filename+", nebo není syntakticky správný.");

	TiXmlElement *bg_el;

	bg_el= doc.FirstChildElement("background");
	if(!bg_el)
		TiXmlError("root element musí být <background ...>.");

	// atributy pozadi
	string str;
	SDL_Surface *sur_src;

	switch(QueryStringAttribute(bg_el,"name", &str)){
		case TIXML_SUCCESS:
			if(str!=bgname)
				TiXmlError(filename,"chybný název pozadí v atributu name");
			break;
		case TIXML_NO_ATTRIBUTE:
			TiXmlError(filename, "chybějící atribut name v <background ...>.");
			break;
	}
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
	Uint16 column, field;
	try{
		// pozadi
		sur1 = load_bg_subEl_(bg_el, "clean_bg", toplapping, sur_src);
		sur2 = load_bg_subEl_(bg_el, "burned_bg", toplapping, sur_src);
		for(column=0 ; column< map_array_.size() ; ++column){
			for(field = 0 ; field<map_array_[column].size() ; ++field){
				// vytvorit a ulozit do seznamu statickych objektu
				staticMOs_.push_back(new Background(sur1,sur2, column, field) );
				// ulozit do mapy na spravne policko
				map_array_[column][field].push_back(staticMOs_.back());
			}
		}
		// rohy
		sur1 = load_bg_subEl_(bg_el, "topleft", toplapping, sur_src);
			// vytvorit a ulozit do seznamu statickych objektu
			staticMOs_.push_back(new Wall(sur1, 0, 0) );
			// ulozit do mapy na spravne policko
			for(column=0; column<sur1.GetSurface()->w/CELL_SIZE ; ++column){
				for(field=0; field<sur1.GetSurface()->h/CELL_SIZE ; ++field){
					map_array_[column][field].push_back(staticMOs_.back());
				}
			}
		sur1 = load_bg_subEl_(bg_el, "topright", toplapping, sur_src);
			// vytvorit a ulozit do seznamu statickych objektu
			staticMOs_.push_back(new Wall(sur1, map_array_.size()-1, 0) );
			// ulozit do mapy na spravne policko
			for(column=0; column<sur1.GetSurface()->w/CELL_SIZE ; ++column){
				for(field=0; field<sur1.GetSurface()->h/CELL_SIZE ; ++field){
					map_array_[map_array_.size()-1-column][field]
						.push_back(staticMOs_.back());
				}
			}
		sur1 = load_bg_subEl_(bg_el, "bottomleft", toplapping, sur_src);
			// vytvorit a ulozit do seznamu statickych objektu
			staticMOs_.push_back(new Wall(sur1, 0, map_array_[0].size()-1-toplapping) );
			// ulozit do mapy na spravne policko
			for(column=0; column<sur1.GetSurface()->w/CELL_SIZE ; ++column){
				for(field=0
						; field<(sur1.GetSurface()->h-toplapping)/CELL_SIZE
						; ++field){
					map_array_[column][map_array_[0].size()-1-field]
						.push_back(staticMOs_.back());
				}
			}
		sur1 = load_bg_subEl_(bg_el, "bottomright", toplapping, sur_src);
			// vytvorit a ulozit do seznamu statickych objektu
			staticMOs_.push_back(new Wall(sur1,
					map_array_.size()-1, map_array_[0].size()-1-toplapping) );
			// ulozit do mapy na spravne policko
			for(column=0; column<sur1.GetSurface()->w/CELL_SIZE ; ++column){
				for(field=0
						; field<(sur1.GetSurface()->h-toplapping)/CELL_SIZE
						; ++field){
					map_array_[map_array_.size()-1-column][map_array_[0].size()-1-field]
						.push_back(staticMOs_.back());
				}
			}
/*
	map_array_t::value_type::value_type::iterator it;
	for(column=0 ; column< map_array_.size() ; ++column){
		for(field = 0 ; field<map_array_[column].size() ; ++field){
			for(it= map_array_[column][field].begin() ;
					it!= map_array_[column][field].end() ;
					++it){
				cout << column << "," << field << ":" << (*it)->type() << endl;
			}
		}
	}
*/
	}
	catch(string s){
		TiXmlError(filename,s);
	}
}

SDL_Surface* Game::load_bg_subEl_(TiXmlElement *bg_el, const char* name_subEl,
				int & toplapping, SDL_Surface* sur_src){
	attr_map_t attr_map;
	SDL_Rect rect;
	SDL_Surface* sur_dst;
	int x, y, w, h;
	subElement(bg_el,name_subEl,attr_map);
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

Game::~Game(){
	for(int i=0; i< staticMOs_.size() ; ++i){
		delete (staticMOs_[i]);
	}
	// TODO dynamicMOs_
}

void Game::draw(SDL_Surface* window){
	Uint16 column, field;
	// pozadi
	map_array_t::value_type::value_type::iterator it;
	for(column=0 ; column< map_array_.size() ; ++column){
		for(field = 0 ; field<map_array_[column].size() ; ++field){
			for(it= map_array_[column][field].begin() ;
					it!= map_array_[column][field].end() ;
					++it){
				(*it)->draw(window);

			}
		}
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
