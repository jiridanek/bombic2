
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <utility>
#include "SDL_lib.h"
#include "tixml_helper.h"
#include "stl_helper.h"
#include "constants.h"
#include "game_base.h"
#include "game_mapobjects.h"
#include "game_background.h"
#include "game_wall.h"
#include "game_floorobject.h"
#include "game_box.h"
#include "game_bonus.h"
#include "game_creature.h"
#include "game_player.h"
#include "game_bomb.h"
#include "game_flame.h"
#include "game_presumption.h"

using namespace std;

/********************** class GameBaseLoader ************************/

/** @details
 * Vytvoří Surface objektu popsaného podelementem se zadaným jménem.
 * Vrácené surface má nastavenou průhlednou barvu, i průhlednost.
 * @throw string Při chybě (nenalezení podelementu nebo některého povinného atributu)
 * vyvolá výjimku s chybovým hlášením.
 * @param El rodič hledaného podelementu
 * @param name_subEl název hledaného podelementu
 * @param sur_dst cílový obrázek
 * @param width výška obrázku
 * @param height šířka obrázku
 * @param sur_src zdrojový surface
 * @see subElement(), readAttr()
 * @see create_transparent_surface()
 */
void GameBaseLoader::load_subEl_surface_( TiXmlElement *El, const char* name_subEl,
			Surface & sur_dst, Uint16 width, Uint16 height, const Surface & sur_src){

	El = subElement(El, name_subEl);

	// atributy pro surface
	Uint16 x, y;
	readAttr(El, "x", x);
	readAttr(El, "y", y);

	// vytvorit surface
	sur_dst= create_transparent_surface(width, height, false);
	SDL_Rect rect={ x, y, width, height };
	SDL_BlitSurface(sur_src.getSurface(), &rect, sur_dst.getSurface(), 0);
}

/** @details
 * Vytvoří Animation objektu popsaného podelementem se zadaným jménem.
 * Vrácené surface má nastavenou průhlednou barvu, nikoli však průhlednost.
 * Zjistí také jaké měl podelement nastavené toplapping (výška nevyužitá v mapě).
 * @throw string Při chybě (nenalezení podelementu nebo některého povinného atributu)
 * vyvolá výjimku s chybovým hlášením.
 * @param El rodič hledaného podelementu
 * @param name_subEl název hledaného podelementu
 * @param anim_dst cílová animace
 * @param sur_src zdrojový surface
 * @return Vrací toplapping - hodnotu atributu nebo nulu pokud nebyl nalezen.
 * @see subElement(), readAttr()
 */
Uint16 GameBaseLoader::load_subEl_animation_( TiXmlElement *El, const char* name_subEl,
			Animation & anim_dst, const Surface & sur_src){

	Uint16 w, h, toplapping;
	El = subElement(El,name_subEl);
	// sirska
	if(!readAttr(El, "width", w, false))
			w= CELL_SIZE;
	else if(w<1)
		throw string("the value of width must be higher than 0");
	else	w*= CELL_SIZE;
	// vyska
	if(!readAttr(El, "height", h, false))
			h= CELL_SIZE;
	else if(h<1)
		throw string("the value of height must be higher than 0");
	else	h*= CELL_SIZE;
	// horni presah
	if(!readAttr(El, "toplapping", toplapping, false))
		toplapping=0;
	else	h+= CELL_SIZE*toplapping;

	Animation anim_new(El, w, h, sur_src);
	anim_dst = anim_new;

	return toplapping;
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
SDL_Surface* GameBaseLoader::load_src_surface_(TiXmlElement *El,
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

/**************** END OF class GameBaseLoader ************************/

/*************** class GameBase ******************************/

/** @details
 * Předpřipraví mapu, atd TODO
 * @param players_count počet hráčů
 * @param mapname název mapy
 */
GameBase::GameBase(Uint16 players_count, const std::string & mapname){
// 	bool deathmatch, bool creatures, bool bombsatend){
	players_.insert(players_.end(), players_count, 0);
	load_map_(players_count, mapname);
}

/** @details
 * Načte mapu z XML souborů, umístí do ní pevně dané objekty,
 * vytvoří struktury s generovanými objekty.
 * @param players_count počet hráčů
 * @param mapname název mapy
 */
void GameBase::load_map_(Uint16 players_count, const std::string & mapname){
	// nacteni hodnot z xml
	TiXmlDocument doc;
	TiXmlElement *map_el;
	string filename(mapname);
	map_el = TiXmlRootElement(doc, filename, "map", true);

	// atributy
	Uint16 height, width;
	try {
		readAttr(map_el, "height", height);
		if(height<=2)
			throw string("the value of attribute height must be greater than 2.");
		readAttr(map_el, "width", width);
		if(height<=2)
			throw string("the value of attribute width must be greater than 2.");
	}
	catch(const string & s){
		TiXmlError(filename, s);
	}

	// vytvoreni zakladniho rozmeru mapy
	base_array_t::value_type column;
	base_array_t::value_type::value_type empty_list;
	column.insert(column.end(), height, empty_list);
	base_array_.insert(base_array_.end(), width, column);

	// vytvoreni rozmeru mapy povolenych policek pro generovani
	allowed_field_t allowed_field = {true, true};
	allowed_array_t::value_type allowed_column;

	allowed_column.insert(allowed_column.end(),
		base_array_[0].size(), allowed_field);

	allowed_array_.clear();
	allowed_array_.insert(allowed_array_.end(),
		base_array_.size(), allowed_column);

	// nacteni pozadi
	string str;
	try {
		readAttr(map_el, "background", str);
	}
	catch(const string & s){
		TiXmlError(filename, s);
	}

	// nacteni pozadi mapy
	load_background_(str);

	// nacteni objektu na zemi
	load_floorobjects_(map_el->FirstChildElement("floorobject"));
	// nacteni hracu
	load_players_(map_el->FirstChildElement("players"), players_count);
	// nacteni zdi
	load_walls_(map_el->FirstChildElement("walls"));
	// nacteni boxu
	load_boxes_(map_el->FirstChildElement("boxes"));
	// nacteni bonusu
	load_bonuses_(map_el->FirstChildElement("bonuses"));
	// nacteni priser
	load_creatures_(map_el->FirstChildElement("creatures"));
	// vytvoreni mapy prazdnych policek pro generovane boxy
	load_noboxes_(map_el->FirstChildElement("boxes"));
	// vytvoreni mapy prazdnych policek pro generovane prisery
	load_nocreatures_(map_el->FirstChildElement("nocreatures"));
	// vyhozeni nulovych pointeru
	clear_null_objects_();
}

/** @details
 * Vytvoří z XML a vloží do mapy její pozadí.
 * @param bgname název pozadí mapy
 */
void GameBase::load_background_(const std::string & bgname){
	// nacteni hodnot z xml
	TiXmlDocument doc;
	TiXmlElement *bg_el;
	string filename(bgname);
	bg_el = TiXmlRootElement(doc, filename, "background", true);

	Surface sur_src;
	Animation anim, anim_burned;
	Uint16 column, field, toplapping;
	try{
		// nacteni zdrojoveho obrazku
		sur_src = load_src_surface_(bg_el);
		// pozadi
		load_subEl_animation_(bg_el, "clean_bg", anim, sur_src);
		load_subEl_animation_(bg_el, "burned_bg", anim_burned, sur_src);
		for(column=0 ; column< base_array_.size() ; ++column){
			for(field = 0 ; field<base_array_[column].size() ; ++field){
				insert_background_(anim, anim_burned, column, field);
			}
		}
		// rohy
		load_subEl_animation_(bg_el, "topleft", anim, sur_src);
		insert_wall_(anim, 0, 0, 0, anim.width()/CELL_SIZE, anim.height()/CELL_SIZE);

		load_subEl_animation_(bg_el, "topright", anim, sur_src);
		insert_wall_(anim, 0,
				base_array_.size()-anim.width()/CELL_SIZE, 0,
				anim.width()/CELL_SIZE, anim.height()/CELL_SIZE);

		toplapping =
		load_subEl_animation_(bg_el, "bottomleft", anim, sur_src);
		insert_wall_(anim, toplapping, 0,
				base_array_[0].size()-anim.height()/CELL_SIZE+toplapping,
				anim.width()/CELL_SIZE, anim.height()/CELL_SIZE-toplapping);

		toplapping =
		load_subEl_animation_(bg_el, "bottomright", anim, sur_src);
		insert_wall_(anim, toplapping,
				base_array_.size()-anim.width()/CELL_SIZE,
				base_array_[0].size()-anim.height()/CELL_SIZE+toplapping,
				anim.width()/CELL_SIZE, anim.height()/CELL_SIZE-toplapping);

		// strany
		load_subEl_animation_(bg_el, "top", anim, sur_src);
		isTypeOf isBG(BACKGROUND);
		for(column=0 ; column< base_array_.size() ; ++column){
			if(isBG(base_array_[column][0].back().o))
				insert_wall_(anim, 0, column, 0,
					anim.width()/CELL_SIZE, anim.height()/CELL_SIZE);
		}

		toplapping =
		load_subEl_animation_(bg_el, "left", anim, sur_src);
		for(field = 0 ; field<base_array_[0].size() ; ++field){
			if(isBG(base_array_[0][field].back().o))
				insert_wall_(anim, toplapping, 0, field,
					anim.width()/CELL_SIZE, anim.height()/CELL_SIZE-toplapping);
		}

		toplapping = load_subEl_animation_(bg_el, "bottom", anim, sur_src);
		field= base_array_[0].size()-anim.height()/CELL_SIZE+toplapping;
		for(column=0 ; column< base_array_.size() ; ++column){
			if(isBG(base_array_[column][field].back().o))
				insert_wall_(anim, toplapping, column, field,
					anim.width()/CELL_SIZE, anim.height()/CELL_SIZE-toplapping);
		}

		toplapping = load_subEl_animation_(bg_el, "right", anim, sur_src);
		column = base_array_.size()-anim.width()/CELL_SIZE;
		for(field = 0 ; field<base_array_[column].size() ; ++field){
			if(isBG(base_array_[column][field].back().o))
				insert_wall_(anim, toplapping, column, field,
					anim.width()/CELL_SIZE, anim.height()/CELL_SIZE-toplapping);
		}

	}
	catch(const string & s){
		TiXmlError(filename,s);
	}
}

/** @details
 * Načte z XML a vloží do mapy hráče.
 * @param playersEl element v XML souboru specifikující výchozí pozici hráče
 * @param count počet hráčů
 */
void GameBase::load_players_(TiXmlElement *playersEl, Uint16 count){
	if(!playersEl)
		throw string("missing element players");

	string filename;
	Uint16 column, field, width, height;
	// TODO
	Uint16 speed, lives, intelligence;
	Surface sur_src, sur_src_s;

	isTypeOf isWall(WALL);

	try{
		readAttr(playersEl, "x", column);
		readAttr(playersEl, "y", field);
		if(column >= base_array_.size()
		|| field >= base_array_[column].size()
		|| isWall(base_array_[column][field].back().o) )
			throw string("value of attribute is out of range.");
	}
	catch(const string & s){
		TiXmlError("in element <players ...>: "+s);
	}
	TiXmlDocument doc;
	TiXmlElement *rootEl;
	try{
		while( count-- ){
			filename = "player"+x2string(count);
			// nacteni hrace
			rootEl = TiXmlRootElement(doc, filename, "creature", true);
			// zdrojovy obrazek
			sur_src = load_src_surface_(rootEl);
			sur_src_s = load_src_surface_(rootEl, "shadow_src", false);

			// vyska a sirska obrazku
			readAttr(rootEl, "height", height);
			readAttr(rootEl, "width", width);

			// TODO vyhodit
			// rychlost, pocet zivotu a inteligence prisery
			attr_SpeedLivesIntelligence(rootEl, speed, lives, intelligence);
			if(speed>CELL_SIZE/2){
				cerr << "Maximal allowed creature's speed is "
					<< CELL_SIZE/2 << endl;
				TiXmlError(filename,"too high value of speed");
			}

			Animation anim_up(subElement(rootEl,"up"),
						width, height, sur_src, sur_src_s),
					anim_right(subElement(rootEl,"right"),
						width, height, sur_src, sur_src_s),
					anim_down(subElement(rootEl,"down"),
						width, height, sur_src, sur_src_s),
					anim_left(subElement(rootEl,"left"),
						width, height, sur_src, sur_src_s),
					anim_burned(subElement(rootEl,"burned"),
						width, height, sur_src);
			insert_player_(anim_up, anim_right, anim_down, anim_left,
				anim_burned, column, field, speed, lives, count);
		}
	}
	catch(const string & s){
		TiXmlError(filename,s);
	}
}

/** @details
 * Načte z XML a vloží do mapy zdi.
 * @param wallsEl element v XML souboru specifikující zdi mapy
 */
void GameBase::load_walls_(TiXmlElement *wallsEl){
	// nenalezen nepovinny tag walls
	if(!wallsEl) return;

	TiXmlDocument doc;
	TiXmlElement *rootEl;

	// nacteni atributu
	string name_def, filename, str;
	Surface sur_src;
	Animation anim_def, anim;
	Uint16 toplapping_def, height_def, width_def,
		toplapping, height, width, x, y;
	// TODO overcrossing

	try{
		readAttr(wallsEl, "name", name_def);
		if(!readAttr(wallsEl, "height", height_def, false))
			height_def = 1;
		if(!readAttr(wallsEl, "width", width_def, false))
			width_def = 1;
	}
	catch(const string & s){
		TiXmlError("in element <walls ...>: "+s);
	}

	// nacteni defaultni zdi
	filename= name_def;
	rootEl = TiXmlRootElement(doc, filename, "wall", true);
	try{
		sur_src = load_src_surface_(rootEl);
		toplapping_def = load_subEl_animation_(rootEl, "img",
				anim_def, sur_src);
	}
	catch(const string & s){
		TiXmlError(filename,s);
	}

	// kontrola jestli koresponduji rozmery obrazku
	if(height_def!=anim_def.height()/CELL_SIZE-toplapping_def)
		TiXmlError(filename,"the value of attribute height doesn't correspond with value in <walls ...>");

	if(width_def!= anim_def.width()/CELL_SIZE)
		TiXmlError(filename,"the value of attribute width doesn't correspond with value in <walls ...>");
	// vytvoreni zdi
	wallsEl= wallsEl->FirstChildElement("wall");
	try{
		while(wallsEl){
			try{
				if(!readAttr(wallsEl, "name", filename, false))
					filename.clear();
				if(!readAttr(wallsEl, "height", height, false))
					height = height_def;
				if(!readAttr(wallsEl, "width", width, false))
					width = width_def;
				readAttr(wallsEl, "x", x);
				readAttr(wallsEl, "y", y);
			}
			catch(const string & s){
				TiXmlError("in element <walls ...>: "+s);
			}

			// nacteni nove zdi
			if(!filename.empty() && filename!=name_def){
				rootEl = TiXmlRootElement(doc, filename, "wall", true);
				sur_src = load_src_surface_(rootEl);
				toplapping= load_subEl_animation_(rootEl, "img",
						anim, sur_src);

				// kontrola jestli koresponduji rozmery obrazku
				if(height!=anim.height()/CELL_SIZE-toplapping)
					TiXmlError(filename,"the value of attribute height doesn't correspond with value in <walls ...>");

				if(width!= anim.width()/CELL_SIZE)
					TiXmlError(filename,"the value of attribute width doesn't correspond with value in <walls ...>");
				insert_wall_(anim, toplapping, x, y, width, height);
			}
			else{
				insert_wall_(anim_def, toplapping_def, x, y, width_def, height_def);
			}

			wallsEl= wallsEl->NextSiblingElement("wall");
		}
	}
	catch(const string & s){
		TiXmlError(filename,s);
	}
}

/** @details
 * Načte z XML a vloží do mapy objekty na zemi.
 * @param floorEl element v XML souboru specifikující objekt na zemi
 */
void GameBase::load_floorobjects_(TiXmlElement *floorEl){
	string filename;
	Uint16 height, width, x,y;

	Surface sur_src;
	Animation anim;

	TiXmlDocument doc;
	TiXmlElement *rootEl;
	try{
		while(floorEl){
			try{
				readAttr(floorEl, "name", filename);
				if(!readAttr(floorEl, "height", height, false))
					height = 1;
				if(!readAttr(floorEl, "width", width, false))
					width = 1;
				readAttr(floorEl, "x", x);
				readAttr(floorEl, "y", y);
			}
			catch(const string & s){
				TiXmlError("in element <floorobject ...>: "+s);
			}
			// nacteni objektu
			rootEl = TiXmlRootElement(doc, filename, "floorobject", true);
			sur_src = load_src_surface_(rootEl);
			load_subEl_animation_(rootEl, "img", anim, sur_src);

			// kontrola jestli koresponduji rozmery obrazku
			if(height!=anim.height()/CELL_SIZE)
				TiXmlError(filename,"the value of attribute height doesn't correspond with value in <floorobject ...> in map.");

			if(width!= anim.width()/CELL_SIZE)
				TiXmlError(filename,"the value of attribute width doesn't correspond with value in <floorobject ...> in map.");

			insert_floorobject_(anim, x, y, width, height);
			floorEl= floorEl->NextSiblingElement("floorobject");
		}
	}
	catch(const string & s){
		TiXmlError(filename,s);
	}
}

/** @details
 * Načte z XML a vloží do mapy pevně dané bedny, ostatní připraví pro vygenerování.
 * @param boxesEl element v XML souboru specifikující bedny v mapě
 */
void GameBase::load_boxes_(TiXmlElement *boxesEl){
	// nenalezen nepovinny tag boxes
	if(!boxesEl) return;

	TiXmlDocument doc;
	TiXmlElement *rootEl, *El;

	// nacteni atributu
	string name_def, filename;
	Surface sur_src;
	Animation anim_def, anim_burning_def, anim, anim_burning;
	Uint16 toplapping_def, height_def, width_def, count,
		toplapping, height, width, x, y;

	// TODO overcrossing

	try{
		readAttr(boxesEl, "name", name_def);
		readAttr(boxesEl, "count", count);
		if(!readAttr(boxesEl, "height", height_def, false))
			height_def = 1;
		if(!readAttr(boxesEl, "width", width_def, false))
			width_def = 1;
	}
	catch(const string & s){
		TiXmlError("in element <boxes ...>: "+s);
	}

	// nacteni defaultniho boxu
	filename= name_def;
	rootEl = TiXmlRootElement(doc, filename, "box", true);
	try{
		sur_src = load_src_surface_(rootEl);

		toplapping_def=
		load_subEl_animation_(rootEl, "img", anim_def, sur_src);
		load_subEl_animation_(rootEl, "burning", anim_burning_def, sur_src);
	}
	catch(const string & s){
		TiXmlError(filename,s);
	}
	// kontrola jestli koresponduji rozmery obrazku
	if(height_def!=anim_def.height()/CELL_SIZE-toplapping_def)
		TiXmlError(filename,"the value of attribute height doesn't correspond with value in <walls ...>.");

	if(width_def!= anim_def.width()/CELL_SIZE)
		TiXmlError(filename,"the value of attribute width doesn't correspond with value in <walls ...>.");

	// vytvoreni pevnych boxu
	El= boxesEl->FirstChildElement("box");
	try{
		while(El){
			try{
				if(!readAttr(El, "name", filename, false))
					filename.clear();
				if(!readAttr(El, "height", height, false))
					height = height_def;
				if(!readAttr(El, "width", width, false))
					width = width_def;
				readAttr(El, "x", x);
				readAttr(El, "y", y);
			}
			catch(const string & s){
				TiXmlError("in element <boxes ...>: "+s);
			}
			// nacteni noveho boxu
			if(!filename.empty() && filename!=name_def){
				rootEl = TiXmlRootElement(doc, filename, "box", true);
				sur_src = load_src_surface_(rootEl);

				toplapping =
				load_subEl_animation_(rootEl, "img", anim_def, sur_src);
				load_subEl_animation_(rootEl, "burning", anim_burning_def, sur_src);

				// kontrola jestli koresponduji rozmery obrazku
				if(height!=anim.height()/CELL_SIZE-toplapping)
					TiXmlError(filename,"the value of attribute height doesn't correspond with value in <boxes ...>.");

				if(width!= anim.width()/CELL_SIZE)
					TiXmlError(filename,"the value of attribute width doesn't correspond with value in <boxes ...>.");
				// vlozeni do mapy
				insert_box_(anim, anim_burning, toplapping,
					x, y, width, height);
			}
			else{
				// vlozeni do mapy
				insert_box_(anim_def, anim_burning_def, toplapping_def,
					x, y, width_def, height_def);
			}
			if(--count==0) break;
			El= El->NextSiblingElement("box");
		}
	}
	catch(const string & s){
		TiXmlError(filename,s);
	}
	// rozsahlejsi bedny nebudou nahodne generovany
	if(height_def!=1 || width_def!=1) return;
	// vlozit pouze do seznamu pro pozdejsi nahodne vygenerovani
	while(count--){
		insert_box_(anim_def, anim_burning_def, toplapping_def);
	}

}

/** @details
 * Načte z XML informace o políčkách, na kterých nesmí být bedna.
 * Na základě tohoto, a již umístěných objektů v mapě vytvoří strukturu
 * políček vhodných k umístění beden, zároven uloží počet těchto políček.
 * @param boxesEl element v XML souboru specifikující bedny v mapě
 */
void GameBase::load_noboxes_(TiXmlElement *boxesEl){

	if(!boxesEl) return;

	TiXmlElement *El;
	Uint16 x, y;
	// zakazana policka z XML
	El= boxesEl->FirstChildElement("nobox");
	try{
		while(El){
			readAttr(El, "x", x);
			readAttr(El, "y", y);

			// vlozeni do mapy zakazanych policek
			if(x<allowed_array_.size() && y<allowed_array_[x].size())
				allowed_array_[x][y].box = false;
			El= El->NextSiblingElement("nobox");
		}
	}
	catch(const string & s){
		TiXmlError("in element <nobox ...>: "+s);
	}
	// spocitam neobsazena policka v mape
	allowed_boxes_count_= 0;
	for(x=0; x<base_array_.size() ; ++x){
		for(y=0 ; y<base_array_[x].size() ; ++y){
			if(allowed_array_[x][y].box)
				++allowed_boxes_count_;
		}
	}
}

/** @details
 * Načte z XML informace o políčkách, na kterých nesmí být příšera.
 * Na základě tohoto, a již umístěných objektů v mapě doplní do struktury
 * políček vhodných k umístění.
 * @param nocreaturesEl element v XML souboru specifikující políčka v mapě,
 * na kterých nechceme příšery.
 * @see GameBase::load_noboxes_()
 */
void GameBase::load_nocreatures_(TiXmlElement *nocreaturesEl){

	if(!nocreaturesEl) return;

	TiXmlElement *El;
	Uint16 x, y;
	// zakazana policka z XML
	El= nocreaturesEl->FirstChildElement("nocreature");
	try{
		while(El){
			readAttr(El, "x", x);
			readAttr(El, "y", y);

			// vlozeni do mapy zakazanych policek
			if(x<allowed_array_.size() && y<allowed_array_[x].size())
				allowed_array_[x][y].creature = false;
			El= El->NextSiblingElement("nocreature");
		}
	}
	catch(const string & s){
		TiXmlError("in element <nocreature ...>: "+s);
	}
}

/** @details
 * Načte z XML a připraví pro pozdější vygenerování bonusy.
 * @param bonusEl element v XML souboru specifikující bonus
 */
void GameBase::load_bonuses_(TiXmlElement *bonusEl){
	string filename, bonus_name;
	Uint16 count;

	Surface sur_src;
	Animation anim;

// 	vector< Animation > bonuses;
// 	vector< Animation >::iterator it;

	TiXmlDocument doc;
	TiXmlElement *rootEl;
	try{
		while(bonusEl){
			try{
				readAttr(bonusEl, "name", bonus_name);
				filename = bonus_name;
				readAttr(bonusEl, "count", count);
			}
			catch(const string & s){
				TiXmlError("in element <bonus ...>: "+s);
			}
			// nacteni bonusu
			rootEl = TiXmlRootElement(doc, filename, "bonus", true);
			// obrazek do mapy
			sur_src = load_src_surface_(rootEl);
			load_subEl_animation_(rootEl, "img", anim, sur_src);
			// do seznamu nezarazenych bonusu pridam bonus count krat
			while(count--){
				insert_bonus_(bonus_name, anim);
			}
			bonusEl= bonusEl->NextSiblingElement("bonuses");
		}
	}
	catch(const string & s){
		TiXmlError(filename,s);
	}
}

/** @details
 * Načte z XML a vloží do mapy pevně umístěné nestvůry,
 * ostatní připraví pro pozdější vygenerování.
 * @param creaturesEl element v XML souboru specifikující nestvůry
 */
void GameBase::load_creatures_(TiXmlElement *creaturesEl){
	string filename;
	Uint16 x,y, count, width, height,
		speed, lives, intelligence;
	Surface sur_src, sur_src_s;
	isTypeOf isBadType(BOX); isBadType.addType(WALL).addType(BONUS);

	TiXmlDocument doc;
	TiXmlElement *rootEl, *El;
	try{
		while(creaturesEl){
			try{
				readAttr(creaturesEl, "name", filename);
				readAttr(creaturesEl, "count", count);
			}
			catch(const string & s){
				TiXmlError("in element <creatures ...>: "+s);
			}
			// nacteni prisery
			rootEl = TiXmlRootElement(doc, filename, "creature", true);
			// zdrojovy obrazek
			sur_src = load_src_surface_(rootEl);
			sur_src_s = load_src_surface_(rootEl, "shadow_src", false);

			// vyska a sirska obrazku
			readAttr(rootEl, "height", height);
			readAttr(rootEl, "width", width);
			// rychlost, pocet zivotu a inteligence prisery
			attr_SpeedLivesIntelligence(rootEl, speed, lives, intelligence);
			if(speed>CELL_SIZE/2){
				cerr << "Maximal allowed creature's speed is "
					<< CELL_SIZE/2 << endl;
				TiXmlError(filename,"too high value of speed");
			}

			Animation anim_up(subElement(rootEl,"up"),
						width, height, sur_src, sur_src_s),
					anim_right(subElement(rootEl,"right"),
						width, height, sur_src, sur_src_s),
					anim_down(subElement(rootEl,"down"),
						width, height, sur_src, sur_src_s),
					anim_left(subElement(rootEl,"left"),
						width, height, sur_src, sur_src_s),
					anim_burned(subElement(rootEl,"burned"),
						width, height, sur_src);

			// pevne zarazene prisery
			for(El = creaturesEl->FirstChildElement("creature");
					El ; El= El->NextSiblingElement("creature")){
				try {
					readAttr(El, "x", x);
					readAttr(El, "y", y);
				}
				catch(const string & s){
					TiXmlError("in element <creature ...>: "+s);
				}
				if(x>=base_array_.size()
				|| y>=base_array_[x].size()
				|| isBadType(base_array_[x][y].back().o) ) continue;
				insert_creature_(anim_up, anim_right, anim_down, anim_left,
					anim_burned, x, y, speed, lives, intelligence);
				if(--count==0) break;
			}

			// nahodne rozlozeni priser
			while(count--){
				insert_creature_(anim_up, anim_right, anim_down,
					anim_left, anim_burned, speed, lives, intelligence);
			}
			creaturesEl= creaturesEl->NextSiblingElement("creatures");
		}
	}
	catch(const string & s){
		TiXmlError(filename,s);
	}
}

/** @details
 * Vytvoří pozadí na zadaných souřadnicích a vloží ho do mapy.
 * @param anim animace pozadí
 * @param anim_burned animace ohořelého
 * @param x souřadnice políčka v mapě
 * @param y souřadnice políčka v mapě
 */
void GameBase::insert_background_(const Animation & anim,
			const Animation & anim_burned, Uint16 x, Uint16 y){
	proportionedMO_t new_obj= {
		new Background(anim, anim_burned, x*CELL_SIZE, y*CELL_SIZE),
		1, 1};
	// ulozit do mapy na spravne policko
	base_array_[x][y].push_back(new_obj);
}

/** @details
 * Vytvoří zed na zadaných souřadnicích a vloží ji do mapy.
 * Zed vkládá do mapy na všechna políčka, které zabírá.
 * Zakáže políčka pro vygenerování příšery i bedny.
 * @param anim animace vkládané zdi
 * @param toplapping počet políček odshora obrázku,
 * které nezabírají místo v mapě
 * @param x souřadnice políčka pro levý roh zdi v mapě
 * @param y souřadnice políčka pro horní roh zdi v mapě
 */
void GameBase::insert_wall_(const Animation & anim,
				Uint16 toplapping, Uint16 x, Uint16 y, Uint16 w, Uint16 h){
	// ulozit do mapy na spravna policka
	if(x>=base_array_.size() && y>=base_array_[0].size())
		return;

	proportionedMO_t new_obj= {
		new Wall(anim, x*CELL_SIZE, (y-toplapping)*CELL_SIZE),
		w, h };
	base_array_[x][y].push_back( new_obj);
	new_obj.o= 0; new_obj.w= new_obj.h= 0;
	allowed_field_t not_allowed_field = {false, false};
	// ulozit prazdny pointer, kvuli naslednemu zjistovani typu posledniho objektu na policku
	for(Uint16 column=0; column<w ; ++column){
		for(Uint16 field=0 ; field<h ; ++field){
			if(x+column>=base_array_.size() || y+field>=base_array_[0].size())
				continue;
			base_array_[x+column][y+field].push_back(new_obj);
			allowed_array_[x+column][y+field] = not_allowed_field;
		}
	}

}

/** @details
 * Vytvoří objekt na zemi na zadaných souřadnicích a vloží ho do mapy.
 * Objekt vkládá do mapy na všechna políčka, které zabírá.
 * @param anim animace vkládaného objektu
 * @param x souřadnice políčka pro levý roh objektu v mapě
 * @param y souřadnice políčka pro horní roh objektu v mapě
 */
void GameBase::insert_floorobject_(const Animation & anim,
			Uint16 x, Uint16 y, Uint16 w, Uint16 h){
	// ulozit do mapy
	if(x>=base_array_.size() || y>=base_array_[0].size())
		return;

	// vytvorit
	proportionedMO_t new_obj= {
		new Floorobject(anim, x*CELL_SIZE, y*CELL_SIZE),
		w, h };
	base_array_[x][y].push_back( new_obj);
	new_obj.o= 0; new_obj.w= new_obj.h= 0;
	// ulozit prazdny pointer, kvuli naslednemu zjistovani typu posledniho objektu na policku
	for(Uint16 column=0; column<w ; ++column){
		for(Uint16 field=0 ; field<h ; ++field){
			if(x+column>=base_array_.size() || y+field>=base_array_[0].size())
				continue;
			base_array_[x+column][y+field].push_back(new_obj);
		}
	}
}

/** @details
 * Vytvoří bednu a vloží ji do seznamu pro pozdější náhodné vygenerování.
 * @param anim animace vkládané bedny
 * @param anim_burning animace hořící bedny
 * @param toplapping počet políček odshora obrázku,
 * které nezabírají místo v mapě
 */
void GameBase::insert_box_(const Animation & anim, const Animation & anim_burning,
				Uint16 toplapping){
	// vytvorit
	MapObject* new_obj =
		new Box(anim, anim_burning, toplapping, 0, 0);
	// ulozit do seznamu generovanych objektu
	generatedMOs_.push_back( new_obj );
}
/** @details
 * Vytvoří bednu na zadaných souřadnicích a vloží ji do mapy.
 * Bednu vkládá do mapy na všechna políčka, která zabírá.
 * Zakáže vygenerování další bedny a příšery.
 * @param anim animace vkládané bedny
 * @param anim_burning animace hořící bedny
 * @param toplapping počet políček odshora obrázku,
 * které nezabírají místo v mapě
 * @param x souřadnice políčka pro levý roh bedny v mapě
 * @param y souřadnice políčka pro horní roh bedny v mapě
 */
void GameBase::insert_box_(const Animation & anim, const Animation & anim_burning,
				Uint16 toplapping, Uint16 x, Uint16 y, Uint16 w, Uint16 h){
	// ulozit do mapy na spravna policka
	if(x>=base_array_.size() || y>=base_array_[0].size())
		return;

	// vytvorit
	proportionedMO_t new_obj= {
		new Box(anim, anim_burning, toplapping, x*CELL_SIZE, y*CELL_SIZE),
		w, h };
	base_array_[x][y].push_back( new_obj);
	new_obj.o= 0; new_obj.w= new_obj.h= 0;
	allowed_field_t not_allowed_field = {false, false};
	// ulozit prazdny pointer, kvuli naslednemu zjistovani typu posledniho objektu na policku
	for(Uint16 column=0; column<w ; ++column){
		for(Uint16 field=0 ; field<h ; ++field){
			if(x+column>=base_array_.size() || y+field>=base_array_[0].size())
				continue;
			base_array_[x+column][y+field].push_back(new_obj);
			allowed_array_[x+column][y+field] = not_allowed_field;
		}
	}
}

/** @details
 * Vytvoří bonus a vloží ho do seznamu pro pozdější náhodné vygenerování.
 * @param bonus_name jméno bonusu v XML
 * @param anim animace vkládaného bonusu (do mapy)
 */
void GameBase::insert_bonus_(const std::string & bonus_name,
					const Animation & anim){
	// vytvorit
	MapObject* new_obj = new Bonus(bonus_name, anim, 0, 0);
	// ulozit do seznamu generovanych objektu
	generatedMOs_.push_back( new_obj);
}

/** @details
 * Vytvoří nestvůru na zadaných souřadnicích
 * a vloží ji do seznamu pro pozdější vygenerování.
 * @param anim_left animace pro otočení doleva
 * @param anim_up animace pro otočení nahoru
 * @param anim_right animace pro otočení doprava
 * @param anim_down animace pro otočení dolů
 * @param anim_burned animace pro umírání
 * @param speed rychlost v pixelech za jednu časovou jednotku
 * @param lives počet životů
 * @param ai kvalita umělé inteligence
 */
void GameBase::insert_creature_(
			const Animation & anim_up, const Animation & anim_right,
			const Animation & anim_down, const Animation & anim_left,
			const Animation & anim_burned, Uint16 speed, Uint16 lives, Uint16 ai){
	// vytvorit
	MapObject* new_obj =
		new Creature(anim_up, anim_right, anim_down, anim_left, anim_burned,
			0, 0, speed, lives, ai);
	// ulozit do seznamu generovanych objektu
	generatedMOs_.push_back( new_obj);
}
/** @details
 * Vytvoří nestvůru na zadaných souřadnicích a vloží ji do mapy.
 * Zakáže vygenerovat na tomto místě bendu.
 * @param anim_left animace pro otočení doleva
 * @param anim_up animace pro otočení nahoru
 * @param anim_right animace pro otočení doprava
 * @param anim_down animace pro otočení dolů
 * @param anim_burned animace pro umírání
 * @param x souřadnice nestvůry v mapě
 * @param y souřadnice nestvůry v mapě
 * @param speed rychlost v pixelech za jednu časovou jednotku
 * @param lives počet životů
 * @param ai kvalita umělé inteligence
 */
void GameBase::insert_creature_(
			const Animation & anim_up, const Animation & anim_right,
			const Animation & anim_down, const Animation & anim_left,
			const Animation & anim_burned, Uint16 x, Uint16 y,
			Uint16 speed, Uint16 lives, Uint16 ai){
	// ulozit do mapy na spravne policko
	if(x>=base_array_.size() || y>=base_array_[0].size())
		return;

	// vytvorit
	proportionedMO_t new_obj= {
		new Creature(anim_up, anim_right, anim_down, anim_left, anim_burned,
			x*CELL_SIZE+CELL_SIZE/2, y*CELL_SIZE+CELL_SIZE/2,
			speed, lives, ai),
		1, 1 };
	base_array_[x][y].push_back( new_obj);
	allowed_array_[x][y].box = false;
}


/** @details
 * Vytvoří hráče na zadaných souřadnicích a vloží jej do mapy.
 * Zakáže na tomto místě a v nejbližším okolí vygenerovat příšeru a bednu.
 * @param anim_left animace pro otočení doleva
 * @param anim_up animace pro otočení nahoru
 * @param anim_right animace pro otočení doprava
 * @param anim_down animace pro otočení dolů
 * @param anim_burned animace pro umírání
 * @param x souřadnice hráče v mapě
 * @param y souřadnice hráče v mapě
 * @param speed rychlost v pixelech za jednu časovou jednotku
 * @param lives počet životů
 * @param num číslo (pořadí) hráče
 */
void GameBase::insert_player_(
			const Animation & anim_up, const Animation & anim_right,
			const Animation & anim_down, const Animation & anim_left,
			const Animation & anim_burned, Uint16 x, Uint16 y,
			Uint16 speed, Uint16 lives, Uint16 num){
	// ulozit do mapy na spravne policko
	if(x>=base_array_.size() || y>=base_array_[0].size())
		return;

	// vytvorit
	players_[num] = new Player(
			anim_up, anim_right, anim_down, anim_left, anim_burned,
			x*CELL_SIZE+CELL_SIZE/2, y*CELL_SIZE+CELL_SIZE/2,
			speed, lives, num);
	proportionedMO_t new_obj= { players_[num], 1, 1 };

	base_array_[x][y].push_back( new_obj);
	// zakazu generovani na policka okolo
	allowed_field_t not_allowed_field = {false, false};
	for(Sint8 i=-1; i<=1 ; ++i){
		allowed_array_[x+i][y] = not_allowed_field;
		allowed_array_[x][y+i] = not_allowed_field;
	}

}


/** @details
 * Do pole mapy jsme u rozměrnějších objektů vkládali
 * pravý objekt pouze na levé horní políčko.
 * Na ostatní políčka, která zabírá, jsme vkládali nulové pointery,
 * abychom zamezili umístění dalšího objektu na zabrané políčko.
 * Nyní chceme tyto falešné nulové objekty vyhodit, jednak jsou nám už k ničemu
 * a jednak by nám vadili při dalším použití base_array_.
 */
void GameBase::clear_null_objects_(){
	Uint16 field, column;
	base_array_t::value_type::value_type::iterator it;
	for(field = 0 ; field<base_array_[0].size() ; ++field){
		for(column=0 ; column< base_array_.size() ; ++column){
			it= base_array_[column][field].begin();
			while(it!= base_array_[column][field].end()){
				if(!it->o)
					it= base_array_[column][field].erase(it);
				else
					++it;
			}
		}
	}
}


/**
 * @see destroy_()
 */
GameBase::~GameBase(){
	destroy_();
}

/** @details
 * Zruší všechny umístěné i generované objekty.
 */
void GameBase::destroy_(){
	base_array_t::value_type::value_type::iterator it;
	Uint16 field, column;
	// umistene
	for(column=0 ; column< base_array_.size() ; ++column){
		for(field = 0 ; field<base_array_[0].size() ; ++field){
			for(it= base_array_[column][field].begin() ;
					it!= base_array_[column][field].end() ;
					++it){
				if(it->o)
					delete it->o;
			}
		}
	}
	base_array_.clear();
	// generovane
	for(Uint16 i=0 ; i<generatedMOs_.size() ; ++i){
		delete generatedMOs_[i];
	}
	generatedMOs_.clear();
}


/**
 * @param player_num číslo (pořadí) hráče
 * @param lives počet životů
 * @param bombs počet bomb, jimiž disponuje
 * @param flames velikost plamene
 * @param boots počet botiček
 */
void GameBase::set_player(Uint16 player_num, Uint16 lives,
	Uint16 bombs, Uint16 flames, Uint16 boots){
	// TODO

}

/*************** END OF class GameBase ******************************/

/******************* class GameTools ******************************/

GameTools::GameTools(Uint16 players_count){
	positioned_surface_t empty_positioned_sur = {0, 0, 0};
	// pripravim seznam pro pozicovane obrazky
	panels_.insert(panels_.end(),
		players_count, empty_positioned_sur);
	bonuses_.insert(bonuses_.end(),
		GAMETOOLS_BONUSES_COUNT, empty_positioned_sur);

	TiXmlDocument doc;
	TiXmlElement *root_el;
	string filename("gametools");
	// nacteni XML souboru
	root_el = TiXmlRootElement(doc, filename, "gametools", false);
	try{
		// nacteni zdrojoveho surface
		Surface sur_src= load_src_surface_(root_el);
		// nacteni plamenu
		load_flame_(subElement(root_el, "flame"), sur_src);
		// nacteni bomb
		load_bombs_(subElement(root_el, "bombs"), sur_src);
		// nacteni panelu
		load_panels_(subElement(root_el, "panels"), players_count, sur_src);
		// nacteni zmensenin bonusu
		load_bonuses_(subElement(root_el, "bonuses"), sur_src);
	}
	catch(const string & err){
		TiXmlError(filename, err);
	}
}

void GameTools::load_flame_(TiXmlElement *flameEl, const Surface & sur_src){
	readAttr(flameEl, "timeperiod", flame_period_);

	load_subEl_animation_(flameEl, "top", flame_top_, sur_src);
	load_subEl_animation_(flameEl, "bottom", flame_bottom_, sur_src);
	load_subEl_animation_(flameEl, "topbottom", flame_topbottom_, sur_src);
	load_subEl_animation_(flameEl, "left", flame_left_, sur_src);
	load_subEl_animation_(flameEl, "right", flame_right_, sur_src);
	load_subEl_animation_(flameEl, "leftright", flame_leftright_, sur_src);
	load_subEl_animation_(flameEl, "cross", flame_cross_, sur_src);
}

void GameTools::load_bombs_(TiXmlElement *bombsEl, const Surface & sur_src){

	load_subEl_animation_(bombsEl, "normal", bomb_normal_, sur_src);
	load_subEl_animation_(bombsEl, "mega", bomb_mega_, sur_src);
	load_subEl_animation_(bombsEl, "presumption", presumption_, sur_src);
}


void GameTools::load_panels_(TiXmlElement *panelsEl,
				Uint16 count, const Surface & sur_src){
	// sirska vyska
	Uint16 width, height;
	readAttr(panelsEl, "width", width);
	readAttr(panelsEl, "height", height);

	string el_name;
	while( count-- ){
		el_name = "player"+x2string(count);

		load_subEl_surface_(panelsEl, el_name.c_str(), panels_[count].sur,
				width, height, sur_src);
		set_transparency(panels_[count].sur.getSurface(), 128);
		switch(count){
			case 0:
				panels_[0].x = 0;
				panels_[0].y = 0;
				break;
			case 1:
				panels_[1].x = width;
				panels_[1].y = height;
				break;
			case 2:
				panels_[2].x = 0;
				panels_[2].y = height;
				break;
			case 3:
				panels_[3].x = width;
				panels_[3].y = 0;
		}
	}
}

void GameTools::load_bonuses_(TiXmlElement *bonusesEl, const Surface & sur_src){
	// sirska vyska
	Uint16 width, height;
	readAttr(bonusesEl, "width", width);
	readAttr(bonusesEl, "height", height);

	TiXmlElement * el;
	char * bonuses_names[] = GAMETOOLS_BONUSES_NAMES;
	for(Uint16 i =0 ; i< GAMETOOLS_BONUSES_COUNT ; ++i){
		load_subEl_surface_(bonusesEl, bonuses_names[i],
				bonuses_[i].sur, width, height, sur_src);
		el = subElement(bonusesEl, bonuses_names[i]);
		readAttr(el, "draw_x", bonuses_[i].x);
		readAttr(el, "draw_y", bonuses_[i].y);
	}
}

Flame* GameTools::flame_top(Uint16 x, Uint16 y) const {
	return new Flame(flame_top_, x*CELL_SIZE, y*CELL_SIZE, flame_period_);
}

Flame* GameTools::flame_bottom(Uint16 x, Uint16 y) const {
	return new Flame(flame_bottom_, x*CELL_SIZE, y*CELL_SIZE, flame_period_);
}

Flame* GameTools::flame_topbottom(Uint16 x, Uint16 y) const {
	return new Flame(flame_topbottom_, x*CELL_SIZE, y*CELL_SIZE, flame_period_);
}

Flame* GameTools::flame_left(Uint16 x, Uint16 y) const {
	return new Flame(flame_left_, x*CELL_SIZE, y*CELL_SIZE, flame_period_);
}

Flame* GameTools::flame_right(Uint16 x, Uint16 y) const {
	return new Flame(flame_right_, x*CELL_SIZE, y*CELL_SIZE, flame_period_);
}

Flame* GameTools::flame_leftright(Uint16 x, Uint16 y) const {
	return new Flame(flame_leftright_, x*CELL_SIZE, y*CELL_SIZE, flame_period_);
}

Flame* GameTools::flame_cross(Uint16 x, Uint16 y) const {
	return new Flame(flame_cross_, x*CELL_SIZE, y*CELL_SIZE, flame_period_);
}

Bomb* GameTools::bomb_normal(Uint16 x, Uint16 y,
				Uint16 flamesize, bool timer) const {
	return new Bomb(bomb_normal_,
		x*CELL_SIZE+CELL_SIZE/2, y*CELL_SIZE+CELL_SIZE/2,
		flamesize, timer);
}

Bomb* GameTools::bomb_mega(Uint16 x, Uint16 y,
				Uint16 flamesize, bool timer) const {
	return new MegaBomb(bomb_mega_,
		x*CELL_SIZE+CELL_SIZE/2, y*CELL_SIZE+CELL_SIZE/2,
		flamesize, timer);
}

Presumption* GameTools::presumption(Uint16 x, Uint16 y) const {
	return new Presumption(presumption_,
		x*CELL_SIZE, y*CELL_SIZE);
}

Uint16 GameTools::get_panel_x_(Uint16 player_num, const SDL_Rect & rect){
	return panels_[player_num].x
		? rect.x+rect.w - panels_[player_num].x
		: rect.x+panels_[player_num].x;
}
Uint16 GameTools::get_panel_y_(Uint16 player_num, const SDL_Rect & rect){
	return panels_[player_num].y
		? rect.y+rect.h - panels_[player_num].y
		: rect.y+panels_[player_num].y;
}

void GameTools::draw_panel_player(SDL_Surface * window, const SDL_Rect & rect,
			Uint16 player_num, Uint16 lives, Uint16 flames, Uint16 bombs,
			Uint16 megabombs, bool slider, bool kicker){
	// panel
	draw_surface(
		get_panel_x_(player_num, rect), get_panel_y_(player_num, rect),
		panels_[player_num].sur.getSurface(), window);
	// pocet zivotu
	Surface text = text_(x2string(lives)+"x");
	draw_surface(
		get_panel_x_(player_num, rect) + 8,
		get_panel_y_(player_num, rect) + 4,
		text.getSurface(), window);

	// plameny
	text = text_(x2string(flames)+"x");
	draw_surface(
		get_panel_x_(player_num, rect) + bonuses_[MEGABOMB].x +
			bonuses_[MEGABOMB].sur.width()*3/2 - text.width()/2,
		get_panel_y_(player_num, rect) + bonuses_[SHIELD].y +
			bonuses_[SHIELD].sur.height()/2 - text.height()/2,
		text.getSurface(), window);
	// normalni bomby
	if(!megabombs){
		text = text_(x2string(bombs)+"x");
		draw_surface(
			get_panel_x_(player_num, rect) + bonuses_[MEGABOMB].x +
				bonuses_[MEGABOMB].sur.width()*3/2 - text.width()/2,
			get_panel_y_(player_num, rect) + bonuses_[MEGABOMB].y +
				bonuses_[MEGABOMB].sur.height()/2 - text.height()/2,
			text.getSurface(), window);
	}
	// megabomby
	if(megabombs)
		draw_panel_bonus(window, rect, player_num,
			MEGABOMB, x2string(megabombs)+"x");
	// posilani
	if(slider)
		draw_panel_bonus(window, rect, player_num,
			SLIDER);
	// kopani
	if(kicker)
		draw_panel_bonus(window, rect, player_num,
			KICKER);
}

void GameTools::draw_panel_bonus(SDL_Surface * window, const SDL_Rect & rect,
			Uint16 player_num, BONUSES bonus, const std::string & val){
	// obrazek
	draw_surface(
		 get_panel_x_(player_num, rect) + bonuses_[bonus].x,
		 get_panel_y_(player_num, rect) + bonuses_[bonus].y,
		bonuses_[bonus].sur.getSurface(), window);
	// hodnota
	if(!val.empty()){
		Surface text = text_(val);
		draw_surface(
			get_panel_x_(player_num, rect) + bonuses_[bonus].x +
				bonuses_[bonus].sur.width()*3/2 - text.width()/2,
			get_panel_y_(player_num, rect) + bonuses_[bonus].y +
				bonuses_[bonus].sur.height()/2 - text.height()/2,
			text.getSurface(), window);
	}
}

const Surface & GameTools::text_(const std::string & text){
	texts_t::iterator it;
	it= texts_.find(text);
	// zjistim jestli uz byl text vytvoren
	if(it==texts_.end()){
		extern Fonts g_font;
		// vytvorim surface
		Surface sur = get_text( g_font[GAMETOOLS_BONUSES_FONT_SIZE],
					text.c_str(), Color::white);
		// a ulozim do mapy
		it = texts_.insert(it, make_pair(text, sur));
	}
	return it->second; // vracim jiz vytvoreny text
}

/************** END OF class GameTools ******************************/

