
#include <iostream>
#include <list>
#include <vector>
#include <string>
#include <utility>
#include <algorithm>
#include "SDL_lib.h"
#include "tixml_helper.h"
#include "stl_helper.h"
#include "constants.h"
#include "game.h"
#include "game_mapobjects.h"
#include "game_background.h"
#include "game_wall.h"
#include "game_floorobject.h"
#include "game_box.h"
#include "game_bonus.h"
#include "game_creature.h"
#include "game_player.h"

using namespace std;

/*************** class Game ******************************/
Game::map_array_t Game::map_array_;

Game::Game(Uint16 players_count, const std::string & mapname){
// 	bool deathmatch, bool creatures, bool bombsatend){
	load_map_(players_count, mapname);
}

void Game::load_map_(Uint16 players_count, const std::string & mapname){
	// nacteni hodnot z xml
	TiXmlDocument doc;
	TiXmlElement *map_el;
	string filename(mapname);
	map_el = TiXmlRootElement(doc, filename, "map", true);

	// atributy
	int height, width;
	// TODO readAttr
	switch(map_el->QueryIntAttribute("height", &height)){
		case TIXML_SUCCESS:
			if(height<=2)
				TiXmlError(filename,"the value of attribute height must be greater than 2.");
			break;
		case TIXML_WRONG_TYPE:
			TiXmlError(filename,"wrong type of attribute height, it must be int.");
			break;
		case TIXML_NO_ATTRIBUTE:
			TiXmlError(filename,"missing attribute height in <map ...>.");
			break;
	}
	switch(map_el->QueryIntAttribute("width", &width)){
		case TIXML_SUCCESS:
			if(width<=2)
				TiXmlError(filename,"the value of attribute width must be greater than 2.");
			break;
		case TIXML_WRONG_TYPE:
			TiXmlError(filename,"wrong type of attribute width, it must be int.");
			break;
		case TIXML_NO_ATTRIBUTE:
			TiXmlError(filename,"missing attribute width in <map ...>.");
			break;
	}
	// vytvoreni zakladniho rozmeru mapy
	map_array_.clear();
	map_array_t::value_type column;
	map_array_t::value_type::value_type empty_list;
	column.insert(column.end(), height, empty_list);
	map_array_.insert(map_array_.end(), width, column);

	// nacteni pozadi
	string str; // TODO readAttr
	if(!readStringAttr(map_el, "background", str))
		TiXmlError(filename, "missing attribute background in <map ...>");
	else
		// nacteni pozadi mapy
		load_background_(str);

	// nacteni objektu na zemi
	load_floorobjects_(map_el->FirstChildElement("floorobject"));
	// nacteni priser
	load_players_(map_el->FirstChildElement("players"), players_count);
	// nacteni zdi
	load_walls_(map_el->FirstChildElement("walls"));
	// nacteni boxu
	load_boxes_(map_el->FirstChildElement("boxes"));
	// nacteni bonusu
	load_bonuses_(map_el->FirstChildElement("bonuses"));
	// nacteni priser
	load_creatures_(map_el->FirstChildElement("creatures"));
}

void Game::load_background_(const std::string & bgname){
	// nacteni hodnot z xml
	TiXmlDocument doc;
	TiXmlElement *bg_el;
	string filename(bgname);
	bg_el = TiXmlRootElement(doc, filename, "background", true);

	// atributy vnorenych elementu
	int toplapping;
	Surface sur_src, sur1, sur2;
	Uint16 column, field;
	try{
		// nacteni zdrojoveho obrazku
		sur_src = load_src_surface_(bg_el);
		// pozadi
		sur1 = load_subEl_surface_(bg_el, "clean_bg", toplapping, sur_src.GetSurface());
		sur2 = load_subEl_surface_(bg_el, "burned_bg", toplapping, sur_src.GetSurface());
		for(column=0 ; column< map_array_.size() ; ++column){
			for(field = 0 ; field<map_array_[column].size() ; ++field){
				// vytvorit a ulozit do seznamu statickych objektu
				// TODO debug
				staticMOs_.push_back(new Background(sur1,sur2, column*CELL_SIZE, field*CELL_SIZE) );
				// ulozit do mapy na spravne policko
				map_array_[column][field].push_back(staticMOs_.back());
			}
		}
		// rohy
		sur1 = load_subEl_surface_(bg_el, "topleft", toplapping, sur_src.GetSurface());
		insert_wall_(sur1, 0, 0, 0);

		sur1 = load_subEl_surface_(bg_el, "topright", toplapping, sur_src.GetSurface());
		insert_wall_(sur1, 0,
				map_array_.size()-sur1.width()/CELL_SIZE, 0);

		sur1 = load_subEl_surface_(bg_el, "bottomleft", toplapping, sur_src.GetSurface());
		insert_wall_(sur1, toplapping, 0,
				map_array_[0].size()-(sur1.height()-toplapping)/CELL_SIZE);

		sur1 = load_subEl_surface_(bg_el, "bottomright", toplapping, sur_src.GetSurface());
		insert_wall_(sur1, toplapping,
				map_array_.size()-sur1.width()/CELL_SIZE,
				map_array_[0].size()-(sur1.height()-toplapping)/CELL_SIZE);

		// strany
		sur1 = load_subEl_surface_(bg_el, "top", toplapping, sur_src.GetSurface());
		isTypeOf isBG(BACKGROUND);
		for(column=0 ; column< map_array_.size() ; ++column){
			if(isBG(map_array_[column][0].back()))
				insert_wall_(sur1, 0, column, 0);
		}

		sur1 = load_subEl_surface_(bg_el, "left", toplapping, sur_src.GetSurface());
		for(field = 0 ; field<map_array_[0].size() ; ++field){
			if(isBG(map_array_[0][field].back()))
				insert_wall_(sur1, toplapping, 0, field);
		}

		sur1 = load_subEl_surface_(bg_el, "bottom", toplapping, sur_src.GetSurface());
		field= map_array_[0].size()-(sur1.height()-toplapping)/CELL_SIZE;
		for(column=0 ; column< map_array_.size() ; ++column){
			if(isBG(map_array_[column][field].back()))
				insert_wall_(sur1, toplapping, column, field);
		}

		sur1 = load_subEl_surface_(bg_el, "right", toplapping, sur_src.GetSurface());
		column = map_array_.size()-sur1.width()/CELL_SIZE;
		for(field = 0 ; field<map_array_[column].size() ; ++field){
			if(isBG(map_array_[column][field].back()))
				insert_wall_(sur1, toplapping, column, field);
		}

	}
	catch(string s){
		TiXmlError(filename,s);
	}
}

void Game::load_players_(TiXmlElement *playersEl, Uint16 count){
	string filename;
	int column, field, x,y, width, height;
	Uint16 speed, lives, intelligence;
	bool is_shadow;
	Surface sur_src, sur_src_s, sur_left, sur_left_s, sur_up, sur_up_s, sur_right, sur_right_s,
		sur_down, sur_down_s, sur_burned;
	SDL_Rect rect;

	isTypeOf isWall(WALL);

	try{
		attr_XY(playersEl, column, field);
		if(column>=static_cast<Sint16>(map_array_.size())
		|| field>=static_cast<Sint16>(map_array_[column].size())
		|| isWall(map_array_[column][field].back()) )
			throw string("value of attribute is out of range.");
	}
	catch( string s){
		TiXmlError("in element <players ...>: "+s);
	}
	TiXmlDocument doc;
	TiXmlElement *rootEl, *El;
	try{
		for( ; count>0 ; --count ){
			filename = "player"+x2string(count);
			// nacteni hrace
			rootEl = TiXmlRootElement(doc, filename, "creature", true);
			// zdrojovy obrazek
			sur_src = load_src_surface_(rootEl);
			sur_src_s = load_src_surface_(rootEl, "shadow_src", false);
			is_shadow = sur_src_s.GetSurface()!=0;

			// vyska a sirska obrazku
			attr_HeightWidth(rootEl, height, width);
			if(height<1) TiXmlError(filename,"missing attribute height");
			if(width<1) TiXmlError(filename,"missing attribute width");
			rect.w = static_cast<Uint16>(width);
			rect.h = static_cast<Uint16>(height);
			// rychlost, pocet zivotu a inteligence prisery
			attr_SpeedLivesIntelligence(rootEl, speed, lives, intelligence);
			if(speed>CELL_SIZE/2){
				cerr << "Maximal allowed creature's speed is "
					<< CELL_SIZE/2 << endl;
// 				TiXmlError(filename,"too high value of speed");
			}

			// left
			El = subElement(rootEl,"left");
			attr_XY(El, x, y);
			rect.x= static_cast<Sint16>(x);
			rect.y= static_cast<Sint16>(y);
			// preneseni obrazku do noveho surface
			sur_left= create_transparent_surface(width, height, false);
			SDL_BlitSurface(sur_src.GetSurface(), &rect, sur_left.GetSurface(), 0);
			if(is_shadow){
				attr_ShadowXY(El, x, y);
				rect.x= static_cast<Sint16>(x);
				rect.y= static_cast<Sint16>(y);
				// preneseni obrazku do noveho surface
				sur_left_s= create_transparent_surface(width, height, true);
				SDL_BlitSurface(sur_src_s.GetSurface(), &rect, sur_left_s.GetSurface(), 0);
			}
			// up
			El = subElement(rootEl,"up");
			attr_XY(El, x, y);
			rect.x= static_cast<Sint16>(x);
			rect.y= static_cast<Sint16>(y);
			// preneseni obrazku do noveho surface
			sur_up= create_transparent_surface(width, height, false);
			SDL_BlitSurface(sur_src.GetSurface(), &rect, sur_up.GetSurface(), 0);
			if(is_shadow){
				attr_ShadowXY(El, x, y);
				rect.x= static_cast<Sint16>(x);
				rect.y= static_cast<Sint16>(y);
				// preneseni obrazku do noveho surface
				sur_up_s= create_transparent_surface(width, height, true);
				SDL_BlitSurface(sur_src_s.GetSurface(), &rect, sur_up_s.GetSurface(), 0);
			}
			// right
			El = subElement(rootEl,"right");
			attr_XY(El, x, y);
			rect.x= static_cast<Sint16>(x);
			rect.y= static_cast<Sint16>(y);
			// preneseni obrazku do noveho surface
			sur_right= create_transparent_surface(width, height, false);
			SDL_BlitSurface(sur_src.GetSurface(), &rect, sur_right.GetSurface(), 0);
			if(is_shadow){
				attr_ShadowXY(El, x, y);
				rect.x= static_cast<Sint16>(x);
				rect.y= static_cast<Sint16>(y);
				// preneseni obrazku do noveho surface
				sur_right_s= create_transparent_surface(width, height, true);
				SDL_BlitSurface(sur_src_s.GetSurface(), &rect, sur_right_s.GetSurface(), 0);
			}
			// down
			El = subElement(rootEl,"down");
			attr_XY(El, x, y);
			rect.x= static_cast<Sint16>(x);
			rect.y= static_cast<Sint16>(y);
			// preneseni obrazku do noveho surface
			sur_down= create_transparent_surface(width, height, false);
			SDL_BlitSurface(sur_src.GetSurface(), &rect, sur_down.GetSurface(), 0);
			if(is_shadow){
				attr_ShadowXY(El, x, y);
				rect.x= static_cast<Sint16>(x);
				rect.y= static_cast<Sint16>(y);
				// preneseni obrazku do noveho surface
				sur_down_s= create_transparent_surface(width, height, true);
				SDL_BlitSurface(sur_src_s.GetSurface(), &rect, sur_down_s.GetSurface(), 0);
			}
			// burned
			El = subElement(rootEl,"burned");
			attr_XY(El, x, y);
			rect.x= static_cast<Sint16>(x);
			rect.y= static_cast<Sint16>(y);
			// preneseni obrazku do noveho surface
			sur_burned= create_transparent_surface(width, height, false);
			SDL_BlitSurface(sur_src.GetSurface(), &rect, sur_burned.GetSurface(), 0);

			insert_player_(sur_left, sur_left_s,
				sur_up, sur_up_s, sur_right, sur_right_s,
				sur_down, sur_down_s, sur_burned, column, field,
				speed, lives);
		}
	}
	catch(string s){
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
	Surface sur_src, sur_def, sur;
	int toplapping_def, height_def, width_def,
		toplapping, height, width, x, y;
	// TODO overcrossing

	try{
		attr_Name(wallsEl, name_def);
		attr_HeightWidth(wallsEl, height_def, width_def);
	}
	catch( string s){
		TiXmlError("in element <walls ...>: "+s);
	}
	if(name_def.empty())
		TiXmlError("missing attribute name in <walls ...> .");

	if(height_def<1) height_def=1;
	if(width_def<1) width_def=1;

	// nacteni defaultni zdi
	filename= name_def;
	rootEl = TiXmlRootElement(doc, filename, "wall", true);
	try{
		sur_src = load_src_surface_(rootEl);
		sur_def= load_subEl_surface_(rootEl, "img",
				toplapping_def, sur_src.GetSurface());
	}
	catch(string s){
		TiXmlError(filename,s);
	}

	// kontrola jestli koresponduji rozmery obrazku
	if(height_def!=(sur_def.height()-toplapping_def)/CELL_SIZE)
		TiXmlError(filename,"the value of attribute height doesn't correspond with value in <walls ...>");

	if(width_def!= sur_def.width()/CELL_SIZE)
		TiXmlError(filename,"the value of attribute width doesn't correspond with value in <walls ...>");
	// vytvoreni zdi
	wallsEl= wallsEl->FirstChildElement("wall");
	try{
		while(wallsEl){
			try{
				attr_Name(wallsEl, filename);
				attr_HeightWidth(wallsEl, height, width);
				attr_XY(wallsEl, x, y);
			}
			catch( string s){
				TiXmlError("in element <walls ...>: "+s);
			}
			if(height<1) height=height_def;
			if(width<1) width=width_def;
			// nacteni nove zdi
			if(!filename.empty() && filename!=name_def){
				rootEl = TiXmlRootElement(doc, filename, "wall", true);
				sur_src = load_src_surface_(rootEl);
				sur= load_subEl_surface_(rootEl, "img",
						toplapping, sur_src.GetSurface());

				// kontrola jestli koresponduji rozmery obrazku
				if(height!=(sur.height()-toplapping)/CELL_SIZE)
					TiXmlError(filename,"the value of attribute height doesn't correspond with value in <walls ...>");

				if(width!= sur.width()/CELL_SIZE)
					TiXmlError(filename,"the value of attribute width doesn't correspond with value in <walls ...>");
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
		TiXmlError(filename,s);
	}

}

void Game::load_floorobjects_(TiXmlElement *floorEl){
	string filename;
	int height, width, x,y,toplapping;

	Surface sur_src, sur;

	TiXmlDocument doc;
	TiXmlElement *rootEl;
	try{
		while(floorEl){
			try{
				attr_Name(floorEl, filename);
				attr_HeightWidth(floorEl, height, width);
				attr_XY(floorEl, x, y);
			}
			catch( string s){
				TiXmlError("in element <floorobject ...>: "+s);
			}
			if(filename.empty())
				TiXmlError("empty attribute name in <floorobject ...>.");
			if(height<1) height=1;
			if(width<1) width=1;
			// nacteni objektu
			rootEl = TiXmlRootElement(doc, filename, "floorobject", true);
			sur_src = load_src_surface_(rootEl);
			sur= load_subEl_surface_(rootEl, "img",
					toplapping, sur_src.GetSurface());

			// kontrola jestli koresponduji rozmery obrazku
			if(height!=sur.height()/CELL_SIZE)
				TiXmlError(filename,"the value of attribute height doesn't correspond with value in <floorobject ...> in map.");

			if(width!= sur.width()/CELL_SIZE)
				TiXmlError(filename,"the value of attribute width doesn't correspond with value in <floorobject ...> in map.");

			insert_floorobject_(sur, x, y);
			floorEl= floorEl->NextSiblingElement("floorobject");
		}
	}
	catch(string s){
		TiXmlError(filename,s);
	}
}

void Game::load_boxes_(TiXmlElement *boxesEl){
	// nenalezen nepovinny tag boxes
	if(!boxesEl) return;

	TiXmlDocument doc;
	TiXmlElement *rootEl, *El;

	// nacteni atributu
	string name_def, filename;
	Surface sur_src, sur_img_def, sur_burning_def, sur_img, sur_burning;
	int toplapping_def, height_def, width_def, count,
		toplapping, height, width, x, y;

	// TODO overcrossing

	try{
		attr_Name(boxesEl, name_def);
		attr_HeightWidth(boxesEl, height_def, width_def);
		attr_Count(boxesEl, count);
	}
	catch(string s){
		TiXmlError("in element <boxes ...>: "+s);
	}
	if(name_def.empty())
		TiXmlError("missing attribute name in <boxes ...>.");
	if(count<1)
		TiXmlError("missing attribute count in <boxes ...>.");
	if(height_def<1) height_def=1;
	if(width_def<1) width_def=1;
	// nacteni defaultniho boxu
	filename= name_def;
	rootEl = TiXmlRootElement(doc, filename, "box", true);
	try{
		sur_src = load_src_surface_(rootEl);
		sur_burning_def= load_subEl_surface_(rootEl, "burning",
				toplapping_def, sur_src.GetSurface());
		sur_img_def= load_subEl_surface_(rootEl, "img",
				toplapping_def, sur_src.GetSurface());
	}
	catch(string s){
		TiXmlError(filename,s);
	}
	// kontrola jestli koresponduji rozmery obrazku
	if(height_def!=(sur_img_def.height()-toplapping_def)/CELL_SIZE)
		TiXmlError(filename,"the value of attribute height doesn't correspond with value in <walls ...>.");

	if(width_def!= sur_img_def.width()/CELL_SIZE)
		TiXmlError(filename,"the value of attribute width doesn't correspond with value in <walls ...>.");

	// vytvoreni boxu
	El= boxesEl->FirstChildElement("box");
	try{
		while(El){
			try{
				attr_Name(El, filename);
				attr_HeightWidth(El, height, width);
				attr_XY(El, x, y);
			}
			catch(string s){
				TiXmlError("in element <boxes ...>: "+s);
			}
			if(height<1) height=height_def;
			if(width<1) width=width_def;
			// nacteni noveho boxu
			if(!filename.empty() && filename!=name_def){
				rootEl = TiXmlRootElement(doc, filename, "box", true);
				sur_src = load_src_surface_(rootEl);
				sur_burning= load_subEl_surface_(rootEl, "burning",
						toplapping, sur_src.GetSurface());
				sur_img= load_subEl_surface_(rootEl, "img",
						toplapping, sur_src.GetSurface());
				// kontrola jestli koresponduji rozmery obrazku
				if(height!=(sur_img.height()-toplapping)/CELL_SIZE)
					TiXmlError(filename,"the value of attribute height doesn't correspond with value in <boxes ...>.");

				if(width!= sur_img.width()/CELL_SIZE)
					TiXmlError(filename,"the value of attribute width doesn't correspond with value in <boxes ...>.");
			}
			else{
				sur_img= sur_img_def;
				sur_burning= sur_burning_def;;
				toplapping= toplapping_def;
			}
			// vlozeni do mapy
			insert_box_(sur_img, sur_burning, toplapping, x, y);
			--count;
			El= El->NextSiblingElement("box");
		}
	}
	catch(string s){
		TiXmlError(filename,s);
	}
	// rozsahlejsi bedny nebudou nahodne generovany
	if(height_def!=1 || width_def!=1) return;
	// vytvoreni mapy policek zakazanych pro umisteni nahodne bedny
	vector< vector< bool> > noboxes;
	vector< bool> column_noboxes;
	column_noboxes.insert(column_noboxes.end(), map_array_[0].size(), false);
	noboxes.insert(noboxes.end(), map_array_.size(), column_noboxes);

	El= boxesEl->FirstChildElement("nobox");
	try{
		while(El){
			attr_XY(El, x, y);
			// vlozeni do mapy zakazanych policek
			if(x<static_cast<Sint16>(noboxes.size())
			&& y<static_cast<Sint16>(noboxes[0].size()))
				noboxes[x][y]=true;
			El= El->NextSiblingElement("nobox");
		}
	}
	catch(string s){
		TiXmlError("in element <nobox ...>: "+s);
	}
	// spocitani volnych policek pro nahodne boxy
	isTypeOf isBG(BACKGROUND);
	double count_free= 0.0;
	for(x=0; x<static_cast<Sint16>(map_array_.size()) ; ++x){
		for(y=0 ; y<static_cast<Sint16>(map_array_[x].size()) ; ++y){
			if(noboxes[x][y]) continue;
			if(!isBG(map_array_[x][y].back())) continue;
			++count_free;
		}
	}

	for(x=0; x<static_cast<Sint16>(map_array_.size()) ; ++x){
		for(y=0 ; y<static_cast<Sint16>(map_array_[x].size()) ; ++y){
			if(noboxes[x][y]) continue;
			if(!isBG(map_array_[x][y].back())) continue;
			// jsem na prazdnem policku
			if(count/count_free >= SDL_Rand()){
				insert_box_(sur_img_def, sur_burning_def, toplapping_def, x, y);
				if(--count==0) return;
			}
			if(--count_free<=0) return;
		}
	}

}

void Game::load_bonuses_(TiXmlElement *bonusEl){
	string filename;
	int help_var, x,y, count;

	Surface sur_src, sur;
	// dvojice velky obrazek do mapy a maly obrazek do panelu
	vector< Surface > bonuses;
	vector< Surface >::iterator it;

	TiXmlDocument doc;
	TiXmlElement *rootEl;
	try{
		while(bonusEl){
			try{
				attr_Name(bonusEl, filename);
				attr_Count(bonusEl, count);
			}
			catch( string s){
				TiXmlError("in element <bonus ...>: "+s);
			}
			if(filename.empty())
				TiXmlError("empty attribute name in <bonus ...>.");
			// nacteni bonusu
			rootEl = TiXmlRootElement(doc, filename, "bonus", true);
			// obrazek do mapy
			sur_src = load_src_surface_(rootEl);
			sur= load_subEl_surface_(rootEl, "img",
					help_var, sur_src.GetSurface());
			// do seznamu nezarazenych bonusu pridam bonus count krat
			while(count--){
				// TODO ruzne bonusy
				bonuses.push_back(sur);
			}
			bonusEl= bonusEl->NextSiblingElement("bonuses");
		}
	}
	catch(string s){
		TiXmlError(filename,s);
	}

	// spocitani policek pro nahodne bonusy
	double count_free= 0.0;
	isTypeOf isBox(BOX);
	for(x=0; x<static_cast<Sint16>(map_array_.size()) ; ++x){
		for(y=0 ; y<static_cast<Sint16>(map_array_[x].size()) ; ++y){
			// bonus muzu umistit pouze na policko s bednou
			if(isBox(map_array_[x][y].back()))
				++count_free;
		}
	}

	// projdu mapu a rozmistim nahodne bonusy
	for(x=0; x<static_cast<Sint16>(map_array_.size()) ; ++x){
		for(y=0 ; y<static_cast<Sint16>(map_array_[x].size()) ; ++y){
			if(!isBox(map_array_[x][y].back())) continue;
			// rozmistit ci nikoli
			count = bonuses.size();
			if(count==0) return;
			if(count/count_free >= SDL_Rand()){
				// nahodny bonus
				it = bonuses.begin()+ rand() % count;
				insert_bonus_(*it, x, y);
				// vyhozeni umisteneho bonusu
				bonuses.erase(it);
			}
			if(--count_free<=0) return;
		}
	}
}

void Game::load_creatures_(TiXmlElement *creaturesEl){
	string filename;
	int x,y, count, width, height;
	Uint16 speed, lives, intelligence;
	bool is_shadow;
	Surface sur_src, sur_src_s, sur_left, sur_left_s, sur_up, sur_up_s, sur_right, sur_right_s,
		sur_down, sur_down_s, sur_burned;
	SDL_Rect rect;
	// seznam prazdnych policek
	vector< pair<Uint16, Uint16> > empty_fields;
	vector< pair<Uint16, Uint16> >::iterator it;
	// naplneni seznamu prazdnych policek
// 	map_array_t::value_type::value_type::iterator l_it;
	isTypeOf isBadType(BOX); isBadType.addType(WALL).addType(BONUS);
	for(x=0; x<static_cast<int>(map_array_.size()) ; ++x){
		for(y=0 ; y<static_cast<int>(map_array_[x].size()) ; ++y){
			if(!isBadType(map_array_[x][y].back()))
				empty_fields.push_back(make_pair(x,y));
		}
	}
	// neni kam pridat jakoukoli priseru
	if(empty_fields.empty()) return;

	TiXmlDocument doc;
	TiXmlElement *rootEl, *El;
	try{
		while(creaturesEl){
			try{
				attr_Name(creaturesEl, filename);
				attr_Count(creaturesEl, count);
			}
			catch( string s){
				TiXmlError("in element <creatures ...>: "+s);
			}
			if(filename.empty())
				TiXmlError("empty attribute name in <creatures ...>.");
			// nacteni prisery
			rootEl = TiXmlRootElement(doc, filename, "creature", true);
			// zdrojovy obrazek
			sur_src = load_src_surface_(rootEl);
			sur_src_s = load_src_surface_(rootEl, "shadow_src", false);
			is_shadow = sur_src_s.GetSurface()!=0;

			// vyska a sirska obrazku
			attr_HeightWidth(rootEl, height, width);
			if(height<1) TiXmlError(filename,"missing attribute height");
			if(width<1) TiXmlError(filename,"missing attribute width");
			rect.w = static_cast<Uint16>(width);
			rect.h = static_cast<Uint16>(height);
			// rychlost, pocet zivotu a inteligence prisery
			attr_SpeedLivesIntelligence(rootEl, speed, lives, intelligence);
			if(speed>CELL_SIZE/2){
				cerr << "Maximal allowed creature's speed is "
					<< CELL_SIZE/2 << endl;
				TiXmlError(filename,"too high value of speed");
			}

			// left
			El = subElement(rootEl,"left");
			attr_XY(El, x, y);
			rect.x= static_cast<Sint16>(x);
			rect.y= static_cast<Sint16>(y);
			// preneseni obrazku do noveho surface
			sur_left= create_transparent_surface(width, height, false);
			SDL_BlitSurface(sur_src.GetSurface(), &rect, sur_left.GetSurface(), 0);
			if(is_shadow){
				attr_ShadowXY(El, x, y);
				rect.x= static_cast<Sint16>(x);
				rect.y= static_cast<Sint16>(y);
				// preneseni obrazku do noveho surface
				sur_left_s= create_transparent_surface(width, height, true);
				SDL_BlitSurface(sur_src_s.GetSurface(), &rect, sur_left_s.GetSurface(), 0);
			}
			// up
			El = subElement(rootEl,"up");
			attr_XY(El, x, y);
			rect.x= static_cast<Sint16>(x);
			rect.y= static_cast<Sint16>(y);
			// preneseni obrazku do noveho surface
			sur_up= create_transparent_surface(width, height, false);
			SDL_BlitSurface(sur_src.GetSurface(), &rect, sur_up.GetSurface(), 0);
			if(is_shadow){
				attr_ShadowXY(El, x, y);
				rect.x= static_cast<Sint16>(x);
				rect.y= static_cast<Sint16>(y);
				// preneseni obrazku do noveho surface
				sur_up_s= create_transparent_surface(width, height, true);
				SDL_BlitSurface(sur_src_s.GetSurface(), &rect, sur_up_s.GetSurface(), 0);
			}
			// right
			El = subElement(rootEl,"right");
			attr_XY(El, x, y);
			rect.x= static_cast<Sint16>(x);
			rect.y= static_cast<Sint16>(y);
			// preneseni obrazku do noveho surface
			sur_right= create_transparent_surface(width, height, false);
			SDL_BlitSurface(sur_src.GetSurface(), &rect, sur_right.GetSurface(), 0);
			if(is_shadow){
				attr_ShadowXY(El, x, y);
				rect.x= static_cast<Sint16>(x);
				rect.y= static_cast<Sint16>(y);
				// preneseni obrazku do noveho surface
				sur_right_s= create_transparent_surface(width, height, true);
				SDL_BlitSurface(sur_src_s.GetSurface(), &rect, sur_right_s.GetSurface(), 0);
			}
			// down
			El = subElement(rootEl,"down");
			attr_XY(El, x, y);
			rect.x= static_cast<Sint16>(x);
			rect.y= static_cast<Sint16>(y);
			// preneseni obrazku do noveho surface
			sur_down= create_transparent_surface(width, height, false);
			SDL_BlitSurface(sur_src.GetSurface(), &rect, sur_down.GetSurface(), 0);
			if(is_shadow){
				attr_ShadowXY(El, x, y);
				rect.x= static_cast<Sint16>(x);
				rect.y= static_cast<Sint16>(y);
				// preneseni obrazku do noveho surface
				sur_down_s= create_transparent_surface(width, height, true);
				SDL_BlitSurface(sur_src_s.GetSurface(), &rect, sur_down_s.GetSurface(), 0);
			}
			// burned
			El = subElement(rootEl,"burned");
			attr_XY(El, x, y);
			rect.x= static_cast<Sint16>(x);
			rect.y= static_cast<Sint16>(y);
			// preneseni obrazku do noveho surface
			sur_burned= create_transparent_surface(width, height, false);
			SDL_BlitSurface(sur_src.GetSurface(), &rect, sur_burned.GetSurface(), 0);

			// pevne zarazene prisery
			for(El = creaturesEl->FirstChildElement("creature");
					El ; El= El->NextSiblingElement("creature")){
				try {
					attr_XY(El, x, y);
				}
				catch(string s){
					TiXmlError("in element <creature ...>: "+s);
				}
				if(x>=static_cast<Sint16>(map_array_.size())
				|| y>=static_cast<Sint16>(map_array_[x].size()) ) continue;
				if(isBadType(map_array_[x][y].back())) continue;

				insert_creature_(sur_left, sur_left_s,
					sur_up, sur_up_s, sur_right, sur_right_s,
					sur_down, sur_down_s, sur_burned, x, y,
					speed, lives, intelligence);
				--count;
			}

			// nahodne rozlozeni priser
			while(count--){
				// nahodne policko
				it = empty_fields.begin()+ rand() % empty_fields.size();
				insert_creature_(sur_left, sur_left_s,
					sur_up, sur_up_s, sur_right, sur_right_s,
					sur_down, sur_down_s, sur_burned, it->first, it->second,
					speed, lives, intelligence);
			}
			creaturesEl= creaturesEl->NextSiblingElement("creatures");
		}
	}
	catch(string s){
		TiXmlError(filename,s);
	}
}

/** @details
 * Vytvoří SDL_Surface objektu popsaného podelementem se zadaným jménem.
 * Vrácené surface má nastavenou průhlednou barvu, nikoli však průhlednost.
 * Zjistí také jaké měl podelement nastavené toplapping (výška nevyužitá v mapě).
 * Při chybě (nenalezení podelementu nebo některého povinného atributu)
 * vyvolá výjimku typu string s chybovým hlášením.
 * @param El rodič hledaného podelementu
 * @param name_subEl název hledaného podelementu
 * @param toplapping reference na int, do které se přiřadí nalezená (nebo defaultní) hodnota
 * @param sur_src zdrojový surface, ze kterého se vyjme výsledný surface
 * @return Výsledný surface patřící nalezenému podelementu.
 * @see subElement(), readAttr()
 */
SDL_Surface* Game::load_subEl_surface_(TiXmlElement *El, const char* name_subEl,
				int & toplapping, SDL_Surface* sur_src){
	SDL_Rect rect;
	SDL_Surface* sur_dst;
	int x, y, w, h;
	El = subElement(El,name_subEl);
	readAttr(El, "x", x);
	rect.x= static_cast<Sint16>(x);
	readAttr(El, "y", y);
	rect.y= static_cast<Sint16>(y);
	if(!readAttr(El, "width", w, false))
			rect.w= CELL_SIZE;
	else if(w<1)
		throw string("the value of width must be higher than 0");
	else	rect.w= CELL_SIZE*static_cast<Uint16>(w);
	if(!readAttr(El, "height", h, false))
			rect.h= CELL_SIZE;
	else if(h<1)
		throw string("the value of height must be higher than 0");
	else	rect.h= CELL_SIZE*static_cast<Uint16>(h);
	if(!readAttr(El, "toplapping", toplapping, false))
		toplapping=0;
	else if(toplapping<0)
		throw string("the value of toplapping must be higher than 0 or equal");
	else	rect.h+= CELL_SIZE*toplapping;

	// preneseni obrazku do noveho surface
	sur_dst= create_transparent_surface(rect.w, rect.h, false);
	SDL_BlitSurface(sur_src, &rect, sur_dst, 0);

	return sur_dst;
}

/** @details
 * Vytvoří SDL_Surface bmp souboru s cestou definovanou v atributu attr_name(defaultně src).
 * Vrácené surface má nastavenou průhlednou barvu, nikoli však průhlednost.
 * Při chybě (nenalezení atributu nebo nevytvoření surface)
 * vyvolá výjimku typu string s chybovým hlášením.
 * @param El element v nemz se hleda atribut src
 * @param attr_name jmeno atributu, v nemz se ma hledat cesta k surface
 * defaultně "src"
 * @param force false pro volitelný atribut
 * defaultně true (povinný atribut)
 * @return Výsledný surface patřící nalezenému elementu.
 * @throw string pokud je nastaven atribut force a
 */
SDL_Surface* Game::load_src_surface_(TiXmlElement *El,
			const char* attr_name, bool force){
	string str;
	SDL_Surface *sur_SDL=0;

	if(readAttr(El, attr_name, str, force)){
		sur_SDL=SDL_LoadBMP(str.c_str());
		if(!sur_SDL)
			throw string("the value of ")+attr_name+
				" isn't valid path to file with BMP.";
		// nastavim pruhlednost
		set_transparent_color(sur_SDL, Color::transparent);
	}

	return sur_SDL;
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
	staticMOs_.push_back(new Wall(sur, x*CELL_SIZE, (y-toplapping)*CELL_SIZE) );
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

/** @details
 * Vytvoří objekt na zemi na zadaných souřadnicích a vloží ho do mapy.
 * Objekt vkládá do mapy na všechna políčka, které zabírá.
 * @param sur surface vkládaného objektu
 * @param x souřadnice levého rohu objektu v mapě
 * @param y souřadnice horního rohu objektu v mapě
 */
void Game::insert_floorobject_(const Surface & sur, Uint16 x, Uint16 y){
	// vytvorit a ulozit do seznamu statickych objektu
	staticMOs_.push_back(new Floorobject(sur, x*CELL_SIZE, y*CELL_SIZE));
	// ulozit do mapy na spravna policka
	for(Uint16 column=0; column<sur.width()/CELL_SIZE ; ++column){
		for(Uint16 field=0 ; field<sur.height()/CELL_SIZE ; ++field){
			if(x+column>=map_array_.size() || y+field>=map_array_[0].size())
				continue;
			map_array_[x+column][y+field].push_back(staticMOs_.back());
		}
	}
}

/** @details
 * Vytvoří bednu na zadaných souřadnicích a vloží ji do mapy.
 * Bednu vkládá do mapy na všechna políčka, která zabírá.
 * @param sur_img surface vkládané bedny
 * @param sur_burning surface hořící vkládané bedny
 * @param toplapping počet políček odshora obrázku,
 * které nezabírají místo v mapě
 * @param x souřadnice levého rohu bedny v mapě
 * @param y souřadnice horního rohu bedny v mapě
 */
void Game::insert_box_(const Surface & sur_img, const Surface & sur_burning,
				Uint16 toplapping, Uint16 x, Uint16 y){
	// vytvorit a ulozit do seznamu dynamickych objektu
	dynamicMOs_.push_back(new Box(sur_img, sur_burning, toplapping, x*CELL_SIZE, y*CELL_SIZE) );
	// ulozit do mapy na spravna policka
	for(Uint16 column=0; column<sur_img.width()/CELL_SIZE ; ++column){
		for(Uint16 field=0 ; field<(sur_img.height()-toplapping)/CELL_SIZE
							; ++field){
			if(x+column>=map_array_.size() || y+field>=map_array_[0].size())
				continue;
			map_array_[x+column][y+field].push_back(dynamicMOs_.back());
		}
	}
}

/** @details
 * Vytvoří bonus na zadaných souřadnicích a vloží ho do mapy.
 * @param sur surface vkládaného bonusu (do mapy)
 * @param x souřadnice bonusu v mapě
 * @param y souřadnice bonusu v mapě
 */
void Game::insert_bonus_(const Surface & sur, Uint16 x, Uint16 y){
	// vytvorit a ulozit do seznamu dynamickych objektu
	dynamicMOs_.push_back(new Bonus(sur, x*CELL_SIZE, y*CELL_SIZE) );
	// ulozit do mapy na spravne policko
	if(x>=map_array_.size() || y>=map_array_[0].size())
		return;
	map_array_[x][y].push_back(dynamicMOs_.back());
}

/** @details
 * Vytvoří nestvůru na zadaných souřadnicích a vloží ji do mapy.
 * @param sur_left surface pro otoceni doleva
 * @param sur_left_s surface stinu pro otoceni doleva
 * @param sur_up surface pro otoceni nahoru
 * @param sur_up_s surface stinu pro otoceni nahoru
 * @param sur_right surface pro otoceni doprava
 * @param sur_right_s surface stinu pro otoceni doprava
 * @param sur_down surface pro otoceni down
 * @param sur_down_s surface stinu pro otoceni down
 * @param sur_burned surface pro umirani
 * @param x souřadnice bonusu v mapě
 * @param y souřadnice bonusu v mapě
 * @param speed rychlost v pixelech za jednu casovou jednotku
 * @param lives pocet zivotu
 * @param ai kvalita umele inteligence
 */
void Game::insert_creature_(const Surface & sur_left, const Surface & sur_left_s,
			const Surface & sur_up, const Surface & sur_up_s,
			const Surface & sur_right, const Surface & sur_right_s,
			const Surface & sur_down, const Surface & sur_down_s,
			const Surface & sur_burned, Uint16 x, Uint16 y,
			Uint16 speed, Uint16 lives, Uint16 ai){

	// vytvorit a ulozit do seznamu dynamickych objektu
	dynamicMOs_.push_back(new Creature(sur_left, sur_left_s,
			sur_up, sur_up_s, sur_right, sur_right_s,
			sur_down, sur_down_s, sur_burned,
			x*CELL_SIZE+CELL_SIZE/2, y*CELL_SIZE+CELL_SIZE/2,
			speed, lives, ai) );
	// ulozit do mapy na spravne policko
	if(x>=map_array_.size() || y>=map_array_[0].size())
		return;
	map_array_[x][y].push_back(dynamicMOs_.back());
}

/** @details
 * Vytvoří hráče na zadaných souřadnicích a vloží jej do mapy.
 * @param sur_left surface pro otoceni doleva
 * @param sur_left_s surface stinu pro otoceni doleva
 * @param sur_up surface pro otoceni nahoru
 * @param sur_up_s surface stinu pro otoceni nahoru
 * @param sur_right surface pro otoceni doprava
 * @param sur_right_s surface stinu pro otoceni doprava
 * @param sur_down surface pro otoceni down
 * @param sur_down_s surface stinu pro otoceni down
 * @param sur_burned surface pro umirani
 * @param x souřadnice bonusu v mapě
 * @param y souřadnice bonusu v mapě
 * @param speed rychlost v pixelech za jednu casovou jednotku
 * @param lives pocet zivotu
 */
void Game::insert_player_(const Surface & sur_left, const Surface & sur_left_s,
			const Surface & sur_up, const Surface & sur_up_s,
			const Surface & sur_right, const Surface & sur_right_s,
			const Surface & sur_down, const Surface & sur_down_s,
			const Surface & sur_burned, Uint16 x, Uint16 y,
			Uint16 speed, Uint16 lives){

	// vytvorit a ulozit do seznamu dynamickych objektu
	dynamicMOs_.push_back(new Player(sur_left, sur_left_s,
			sur_up, sur_up_s, sur_right, sur_right_s,
			sur_down, sur_down_s, sur_burned,
			x*CELL_SIZE+CELL_SIZE/2, y*CELL_SIZE+CELL_SIZE/2,
			speed, lives) );
	// ulozit do mapy na spravne policko
	if(x>=map_array_.size() || y>=map_array_[0].size())
		return;
	map_array_[x][y].push_back(dynamicMOs_.back());
}

Game::~Game(){
	// zrusit staticke objekty
	for(Uint16 i=0; i< staticMOs_.size() ; ++i){
		delete (staticMOs_[i]);
	}
	// zrusit dynamicke objekty
	dynamicMOs_t::iterator it;
	for(it= dynamicMOs_.begin(); it!=dynamicMOs_.end(); ++it){
		delete (*it);
	}
}

void Game::draw(SDL_Surface* window){
	Uint16 column, field;
	map_array_t::value_type::value_type::iterator it, end_it;
	isTypeOf isBgType;
// TODO debug
Uint32 fps_last=0;
Uint8 *keystate = SDL_GetKeyState(0);
	// poprve projdu mapu a vykreslim pozadi a objekty na pozadi
	// objekty na policku seradim
	isBgType.addType(BACKGROUND).addType(FLOOROBJECT);
	for(field = 0 ; field<map_array_[0].size() ; ++field){
		for(column=0 ; column< map_array_.size() ; ++column){
			it = map_array_[column][field].begin();
			end_it = map_array_[column][field].end();
			// vykreslim pozadi a objekt na zemi
			while( (it=find_if(it, end_it, isBgType)) !=end_it){
				(*it)->draw(window);
				++it;
			}
			map_array_[column][field].sort(isUnder);
		}
	}

	// podruhe projdu mapu a vykreslim ostatni objekty
	for(field = 0 ; field<map_array_[0].size() ; ++field){
		for(column=0 ; column< map_array_.size() ; ++column){
			for(it= map_array_[column][field].begin() ;
					it!= map_array_[column][field].end() ;
					++it){
				if(!isBgType(*it))
					(*it)->draw(window);
// TODO debug
SDL_PumpEvents();
if(keystate[SDLK_m]){
	fps_last= SDL_fps(fps_last,10);
	SDL_Flip(window);
}
			}
		}
	}
}

/// Nastavení parametrů hráče.
void Game::set_player(Uint16 player_num, Uint16 lives,
	Uint16 bombs, Uint16 flames, Uint16 boots){
}

/** @details
 * Projde všechny dynamické objekty hry zavola na nich fci move().
 * @see DynamicMO::move()
 */
void Game::play(){
	dynamicMOs_t::iterator it;
	for(it= dynamicMOs_.begin() ; it!= dynamicMOs_.end() ; ++it){
		(*it)->move();
	}
}
/// Info o ukončení hry.
bool Game::success() const{
	return true;
}
/// Info o hráči.
void Game::player(Uint16 player_num, Uint16 & lives,
	Uint16 & bombs, Uint16 & flames, Uint16 & boots) const{
}

/**
 * @param x x-ová souřadnice políčka
 * @param y y-ová souřadnice políčka
 * @return Vrací TRUE pokud lze zadané políčko přejít (není na něm zed ani bedna).
 */
bool Game::field_canGoOver(Uint16 x, Uint16 y){
	map_array_t::value_type::value_type::const_iterator begin, end;
	isTypeOf isBlockedObject; isBlockedObject.addType(WALL).addType(BOX);

	begin = map_array_[x][y].begin();
	end = map_array_[x][y].end();

	return find_if(begin, end, isBlockedObject)==end;
}

/*************** END OF class Game ******************************/

