
#include <vector>
#include <string>
#include <helpers/agar.h>
#include <helpers/sdl.h>
#include <helpers/tixml.h>
#include <helpers/stl.h>
#include <constants.h>
#include "base.h"
#include "mapobjects.h"
#include "background.h"
#include "wall.h"
#include "floorobject.h"
#include "box.h"
#include "bonus.h"
#include "creature.h"
#include "player.h"

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
void GameBaseLoader::load_subEl_surface( TiXmlElement *El, const char* name_subEl,
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
Uint16 GameBaseLoader::load_subEl_animation( TiXmlElement *El, const char* name_subEl,
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
SDL_Surface* GameBaseLoader::load_src_surface(TiXmlElement *El,
			const char* attr_name, bool force){
	string str;
	SDL_Surface *sur_SDL=0;

	if(readAttr(El, attr_name, str, force)){
		// najit zdrojovy soubor
		if(!locate_file("", str, str)) {
			throw string("file specified in value of ")+attr_name+
				" was not locate.";
		}
		sur_SDL=SDL_LoadBMP(str.c_str());
		if(!sur_SDL)
			throw string("file specified in value of ")+attr_name+
				" located but isn't valid BMP file.";
		// nastavim pruhlednost
		set_transparent_color(sur_SDL, Color::transparent);
	}

	return sur_SDL;
}

/**************** END OF class GameBaseLoader ************************/

/*************** class GameBase ******************************/

/** @details
 * Vytvori zaklad hry pro kooperativ mod.
 * @param players_count počet hráčů
 * @param mapname název mapy
 * @see load_map_()
 */
GameBase::GameBase(Uint16 players_count, const std::string & mapname){
	players_.insert(players_.end(), players_count, 0);
	load_map_(players_count, mapname, false, true);
}

/** @details
 * Vytvori zaklad hry pro deathmatch.
 * @param players_count počet hráčů
 * @param mapname název mapy
 * @param bonuses seznam bonusu k vygenerovani
 * @param creatures jestli se maji vkladat prisery
 * @see load_map_(), load_bonuses_()
 */
GameBase::GameBase(Uint16 players_count, const std::string & mapname,
			const bonuses_t & bonuses, bool creatures){
	players_.insert(players_.end(), players_count, 0);
	load_map_(players_count, mapname, true, creatures);
	load_bonuses_(bonuses);
}

/** @details
 * Načte mapu z XML souborů, umístí do ní pevně dané objekty,
 * vytvoří struktury s generovanými objekty.
 * @param players_count počet hráčů
 * @param mapname název mapy
 * @param deathmatch vytváří-li mapu pro deathmatch
 * @param creatures má-li připravit do mapy také příšery
 * @see TiXmlError()
 */
void GameBase::load_map_(Uint16 players_count, const std::string & mapname,
			bool deathmatch, bool creatures){
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
		if(width<=2)
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
	load_floorobjects_(map_el->FirstChildElement("floorobjects"));
	// nacteni hracu
	if(deathmatch)
		load_players_deathmatch_(
			map_el->FirstChildElement("players"), players_count);
	else
		load_players_(
			map_el->FirstChildElement("players"), players_count);
	// nacteni zdi
	load_walls_(map_el->FirstChildElement("walls"));
	// nacteni boxu
	load_boxes_(map_el->FirstChildElement("boxes"));
	// nacteni priser
	if(creatures)
		load_creatures_(map_el->FirstChildElement("creatures"));
	// nacteni bonusu
	if(!deathmatch)
		load_bonuses_(map_el->FirstChildElement("bonuses"));
	// vytvoreni mapy prazdnych policek pro generovane boxy
	load_noboxes_(map_el->FirstChildElement("dont_generate"));
	// vytvoreni mapy prazdnych policek pro generovane prisery
	load_nocreatures_(map_el->FirstChildElement("dont_generate"));
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
		sur_src = load_src_surface(bg_el);
		// pozadi
		load_subEl_animation(bg_el, "clean_bg", anim, sur_src);
		load_subEl_animation(bg_el, "burned_bg", anim_burned, sur_src);
		for(column=0 ; column< base_array_.size() ; ++column){
			for(field = 0 ; field<base_array_[column].size() ; ++field){
				insert_background_(anim, anim_burned, column, field);
			}
		}
		// rohy
		load_subEl_animation(bg_el, "topleft", anim, sur_src);
		insert_wall_(anim, 0, 0, 0, anim.width()/CELL_SIZE, anim.height()/CELL_SIZE);

		load_subEl_animation(bg_el, "topright", anim, sur_src);
		insert_wall_(anim, 0,
				base_array_.size()-anim.width()/CELL_SIZE, 0,
				anim.width()/CELL_SIZE, anim.height()/CELL_SIZE);

		toplapping =
		load_subEl_animation(bg_el, "bottomleft", anim, sur_src);
		insert_wall_(anim, toplapping, 0,
				base_array_[0].size()-anim.height()/CELL_SIZE+toplapping,
				anim.width()/CELL_SIZE, anim.height()/CELL_SIZE-toplapping);

		toplapping =
		load_subEl_animation(bg_el, "bottomright", anim, sur_src);
		insert_wall_(anim, toplapping,
				base_array_.size()-anim.width()/CELL_SIZE,
				base_array_[0].size()-anim.height()/CELL_SIZE+toplapping,
				anim.width()/CELL_SIZE, anim.height()/CELL_SIZE-toplapping);

		// strany
		load_subEl_animation(bg_el, "top", anim, sur_src);
		isTypeOf & isBG(isTypeOf::isBgType);
		for(column=0 ; column< base_array_.size() ; ++column){
			if(isBG(base_array_[column][0].back().o))
				insert_wall_(anim, 0, column, 0,
					anim.width()/CELL_SIZE, anim.height()/CELL_SIZE);
		}

		toplapping =
		load_subEl_animation(bg_el, "left", anim, sur_src);
		for(field = 0 ; field<base_array_[0].size() ; ++field){
			if(isBG(base_array_[0][field].back().o))
				insert_wall_(anim, toplapping, 0, field,
					anim.width()/CELL_SIZE, anim.height()/CELL_SIZE-toplapping);
		}

		toplapping = load_subEl_animation(bg_el, "bottom", anim, sur_src);
		field= base_array_[0].size()-anim.height()/CELL_SIZE+toplapping;
		for(column=0 ; column< base_array_.size() ; ++column){
			if(isBG(base_array_[column][field].back().o))
				insert_wall_(anim, toplapping, column, field,
					anim.width()/CELL_SIZE, anim.height()/CELL_SIZE-toplapping);
		}

		toplapping = load_subEl_animation(bg_el, "right", anim, sur_src);
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
		TiXmlError("missing element <players ...>");

	string filename;
	Uint16 column, field, width, height, speed;
	Surface sur_src, sur_src_s;

	try{
		readAttr(playersEl, "x", column);
		readAttr(playersEl, "y", field);
		if(column >= base_array_.size()
		|| field >= base_array_[column].size()
		|| isTypeOf::isWallBox(base_array_[column][field].back().o) )
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
			rootEl = TiXmlRootElement(doc, filename, "player", true);
			// zdrojovy obrazek
			sur_src = load_src_surface(rootEl);
			sur_src_s = load_src_surface(rootEl, "shadow_src", false);

			// vyska a sirska obrazku
			readAttr(rootEl, "height", height);
			readAttr(rootEl, "width", width);
			readAttr(rootEl, "speed", speed);

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
				anim_burned, column, field, speed, count);
		}
	}
	catch(const string & s){
		TiXmlError(filename,s);
	}
}

/** @details
 * Načte z XML a vloží do mapy hráče pro deathmatch.
 * @param playersEl element v XML souboru specifikující výchozí pozici hráče
 * @param count počet hráčů
 */
void GameBase::load_players_deathmatch_(TiXmlElement *playersEl, Uint16 count){
	if(!playersEl)
		TiXmlError("missing element <players ...>");

	string filename;
	Uint16 column, field, width, height, speed;
	Surface sur_src, sur_src_s;

	TiXmlDocument doc;
	TiXmlElement *rootEl;
	try{
		while( count-- ){
			filename = "player"+x2string(count);

			// souradnice hrace
			try{
				rootEl = subElement(playersEl, filename.c_str());
				readAttr(rootEl, "x", column);
				readAttr(rootEl, "y", field);
				if(column >= base_array_.size()
				|| field >= base_array_[column].size()
				|| isTypeOf::isWallBox(base_array_[column][field].back().o) )
					throw string("value of attribute is out of range.");
			}
			catch(const string & s){
				TiXmlError("in element <players ...>: "+s);
			}

			// nacteni hrace
			rootEl = TiXmlRootElement(doc, filename, "player", true);
			// zdrojovy obrazek
			sur_src = load_src_surface(rootEl);
			sur_src_s = load_src_surface(rootEl, "shadow_src", false);

			// vyska a sirska obrazku
			readAttr(rootEl, "height", height);
			readAttr(rootEl, "width", width);
			readAttr(rootEl, "speed", speed);

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
				anim_burned, column, field, speed, count);
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

	// nacteni zdi
	string name;
	try{
		readAttr(wallsEl, "name", name);
	}
	catch(const string & s){
		TiXmlError("in element <walls ...>: "+s);
	}

	string filename= name;
	TiXmlDocument doc;
	TiXmlElement *rootEl =
		TiXmlRootElement(doc, filename, "wall", true);
	Surface sur_src;
	Animation anim;
	Uint16 toplapping;
	try{
		sur_src = load_src_surface(rootEl);
		toplapping = load_subEl_animation(rootEl, "img",
				anim, sur_src);
	}
	catch(const string & s){
		TiXmlError(filename,s);
		return;
	}
	// rozmery zdi z velikosti obrazku
	Uint16 height = anim.height()/CELL_SIZE - toplapping;
	Uint16 width = anim.width()/CELL_SIZE;

	// vytvoreni zdi
	TiXmlElement * el= wallsEl->FirstChildElement("wall");
	try{
		while(el){
			Uint16 x, y;
			try{
				readAttr(el, "x", x);
				readAttr(el, "y", y);
			}
			catch(const string & s){
				TiXmlError("in element <wall ...>: "+s);
			}

			insert_wall_(anim, toplapping, x, y, width, height);

			el= el->NextSiblingElement("wall");
		}
	}
	catch(const string & s){
		TiXmlError(filename,s);
	}
	// try to load next walls
	load_walls_(wallsEl->NextSiblingElement("walls"));
}

/** @details
 * Načte z XML a vloží do mapy objekty na zemi.
 * @param floorsEl element v XML souboru specifikující objekt na zemi
 */
void GameBase::load_floorobjects_(TiXmlElement *floorsEl){
	// nenalezen nepovinny tag floorobjects
	if(!floorsEl) return;

	// nacteni zdi
	string name;
	try{
		readAttr(floorsEl, "name", name);
	}
	catch(const string & s){
		TiXmlError("in element <floorobjects ...>: "+s);
	}

	string filename= name;
	TiXmlDocument doc;
	TiXmlElement *rootEl =
		TiXmlRootElement(doc, filename, "floorobject", true);
	Surface sur_src;
	Animation anim;
	try{
		sur_src = load_src_surface(rootEl);
		load_subEl_animation(rootEl, "img",
				anim, sur_src);
	}
	catch(const string & s){
		TiXmlError(filename,s);
		return;
	}
	// rozmery zdi z velikosti obrazku
	Uint16 height = anim.height()/CELL_SIZE;
	Uint16 width = anim.width()/CELL_SIZE;

	// vytvoreni zdi
	TiXmlElement * el= floorsEl->FirstChildElement("floorobject");
	try{
		while(el){
			Uint16 x, y;
			try{
				readAttr(el, "x", x);
				readAttr(el, "y", y);
			}
			catch(const string & s){
				TiXmlError("in element <floorobject ...>: "+s);
			}

			insert_floorobject_(anim, x, y, width, height);

			el= el->NextSiblingElement("floorobject");
		}
	}
	catch(const string & s){
		TiXmlError(filename,s);
	}
	// try to load next walls
	load_floorobjects_(floorsEl->NextSiblingElement("floorobjects"));
}

/** @details
 * Načte z XML a vloží do mapy pevně dané bedny, ostatní připraví pro vygenerování.
 * @param boxesEl element v XML souboru specifikující bedny v mapě
 */
void GameBase::load_boxes_(TiXmlElement *boxesEl){
	// nenalezen nepovinny tag boxes
	if(!boxesEl) return;

	// nacteni boxu
	string name;
	Uint16 random_generated;
	try{
		readAttr(boxesEl, "name", name);
		if(!readAttr(boxesEl, "random_generated", random_generated, false))
			random_generated = 0;
	}
	catch(const string & s){
		TiXmlError("in element <boxes ...>: "+s);
	}

	string filename= name;
	TiXmlDocument doc;
	TiXmlElement *rootEl =
		TiXmlRootElement(doc, filename, "box", true);
	Surface sur_src;
	Animation anim, anim_burning;
	Uint16 toplapping;
	try{
		sur_src = load_src_surface(rootEl);
		toplapping = load_subEl_animation(rootEl, "img",
				anim, sur_src);
		load_subEl_animation(rootEl, "burning", anim_burning, sur_src);
	}
	catch(const string & s){
		TiXmlError(filename,s);
		return;
	}
	// rozmery boxu z velikosti obrazku
	Uint16 height = anim.height()/CELL_SIZE - toplapping;
	Uint16 width = anim.width()/CELL_SIZE;

	// vytvoreni pevnych boxu
	TiXmlElement * el= boxesEl->FirstChildElement("box");
	try{
		while(el){
			Uint16 x, y;
			try{
				readAttr(el, "x", x);
				readAttr(el, "y", y);
			}
			catch(const string & s){
				TiXmlError("in element <boxes ...>: "+s);
			}
			// vlozeni do mapy
			insert_box_(anim, anim_burning, toplapping,
				x, y, width, height);
			el = el->NextSiblingElement("box");
		}
	}
	catch(const string & s){
		TiXmlError(filename,s);
	}
	// rozsahlejsi bedny nebudou nahodne generovany
	if(height!=1 || width!=1) return;
	// vlozit pouze do seznamu pro pozdejsi nahodne vygenerovani
	while(random_generated--){
		insert_box_(anim, anim_burning, toplapping);
	}
	// try to load next boxes
	load_boxes_(boxesEl->NextSiblingElement("boxes"));
}

/** @details
 * Načte z XML informace o políčkách, na kterých nesmí být bedna.
 * Na základě tohoto, a již umístěných objektů v mapě vytvoří strukturu
 * políček vhodných k umístění beden, zároven uloží počet těchto políček.
 * @param boxesEl element v XML souboru specifikující bedny v mapě
 */
void GameBase::load_noboxes_(TiXmlElement *boxesEl){

	if(boxesEl) {
		TiXmlElement *El;
		// zakazana policka z XML
		El= boxesEl->FirstChildElement("nobox");
		try{
			while(El){
				Uint16 x, y;
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
	}

	// spocitam neobsazena policka v mape - ikdyz nemam zadny nobox
	allowed_boxes_count_= 0;
	for(Uint16 x=0; x<base_array_.size() ; ++x){
		for(Uint16 y=0 ; y<base_array_[x].size() ; ++y){
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

	TiXmlDocument doc;
	TiXmlElement *rootEl;
	try{
		while(bonusEl){
			try{
				readAttr(bonusEl, "name", bonus_name);
				filename = bonus_name;
				readAttr(bonusEl, "random_generated", count);
			}
			catch(const string & s){
				TiXmlError("in element <bonus ...>: "+s);
			}
			// nacteni bonusu
			rootEl = TiXmlRootElement(doc, filename, "bonus", true);
			// obrazek do mapy
			sur_src = load_src_surface(rootEl);
			load_subEl_animation(rootEl, "img", anim, sur_src);
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

void GameBase::load_bonuses_(const bonuses_t & bonuses){
	string filename;
	int count;

	Surface sur_src;
	Animation anim;

	TiXmlDocument doc;
	TiXmlElement *rootEl;
	try{
		for(Uint16 i=0 ; i<bonuses.size() ; ++i){
			filename = bonuses[i].n;
			count = bonuses[i].c;
			// nacteni bonusu
			rootEl = TiXmlRootElement(doc, filename, "bonus", true);
			// obrazek do mapy
			sur_src = load_src_surface(rootEl);
			load_subEl_animation(rootEl, "img", anim, sur_src);
			// do seznamu nezarazenych bonusu pridam bonus count krat
			while(count--){
				insert_bonus_(bonuses[i].n, anim);
			}
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
	Uint16 x,y, random_generated, width, height,
		speed, lives, intelligence;
	Surface sur_src, sur_src_s;
	isTypeOf isBadType(BOX, WALL, BONUS);

	TiXmlDocument doc;
	TiXmlElement *rootEl, *El;
	try{
		while(creaturesEl){
			try{
				readAttr(creaturesEl, "name", filename);
				if(!readAttr(creaturesEl, "random_generated", random_generated, false))
					random_generated = 0;
			}
			catch(const string & s){
				TiXmlError("in element <creatures ...>: "+s);
			}
			// nacteni prisery
			rootEl = TiXmlRootElement(doc, filename, "creature", true);
			// zdrojovy obrazek
			sur_src = load_src_surface(rootEl);
			sur_src_s = load_src_surface(rootEl, "shadow_src", false);

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
			}

			// nahodne rozlozeni priser
			while(random_generated--){
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
 * @param w šířka zdi v políčkách
 * @param h výška zdi v políčkách
 */
void GameBase::insert_wall_(const Animation & anim,
				Uint16 toplapping, Uint16 x, Uint16 y, Uint16 w, Uint16 h){
	// ulozit do mapy na spravna policka, musi se tam vejit cely
	if(static_cast<Uint16>(x+w-1) >= base_array_.size()
	|| static_cast<Uint16>(y+h-1) >= base_array_[0].size())
		return;

	proportionedMO_t new_obj= {
		new Wall(anim, x*CELL_SIZE, (y-toplapping)*CELL_SIZE),
		w, h };
	// ulozim na posledni vykreslovane policko
	base_array_[x+w-1][y+h-1].push_back( new_obj);
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
 * @param w šířka objektu v políčkách
 * @param h výška objektu v políčkách
 */
void GameBase::insert_floorobject_(const Animation & anim,
			Uint16 x, Uint16 y, Uint16 w, Uint16 h){
	// ulozit do mapy na spravna policka, musi se tam vejit cely
	if(static_cast<Uint16>(x+w-1) >= base_array_.size()
	|| static_cast<Uint16>(y+h-1) >= base_array_[0].size())
		return;

	// vytvorit
	proportionedMO_t new_obj= {
		new Floorobject(anim, x*CELL_SIZE, y*CELL_SIZE),
		w, h };
	// ulozim na posledni vykreslovane policko
	base_array_[x+w-1][y+h-1].push_back( new_obj);
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
 * @param w šířka bedny v políčkách
 * @param h výška bedny v políčkách
 */
void GameBase::insert_box_(const Animation & anim, const Animation & anim_burning,
				Uint16 toplapping, Uint16 x, Uint16 y, Uint16 w, Uint16 h){
	// ulozit do mapy na spravna policka, musi se tam vejit cely
	if(static_cast<Uint16>(x+w-1) >= base_array_.size()
	|| static_cast<Uint16>(y+h-1) >= base_array_[0].size())
		return;

	// vytvorit
	proportionedMO_t new_obj= {
		new Box(anim, anim_burning, toplapping, x*CELL_SIZE, y*CELL_SIZE),
		w, h };
	// ulozim na posledni vykreslovane policko
	base_array_[x+w-1][y+h-1].push_back( new_obj);
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
 * @param num číslo (pořadí) hráče
 */
void GameBase::insert_player_(
			const Animation & anim_up, const Animation & anim_right,
			const Animation & anim_down, const Animation & anim_left,
			const Animation & anim_burned, Uint16 x, Uint16 y,
			Uint16 speed, Uint16 num){
	// ulozit do mapy na spravne policko
	if(x>=base_array_.size() || y>=base_array_[0].size())
		return;

	// vytvorit
	players_[num] = new Player(
			anim_up, anim_right, anim_down, anim_left, anim_burned,
			x*CELL_SIZE+CELL_SIZE/2, y*CELL_SIZE+CELL_SIZE/2, speed, num);
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
 * Zruší všechny umístěné i generované objekty.
 */
GameBase::~GameBase(){
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
 * @param prop vlastnosti hráče
 */
void GameBase::set_player(Uint16 player_num, const PlayerProperties & prop){
	players_[player_num]->set_properties(prop);
}

/**
 * @param player_num číslo (pořadí) hráče
 * @param prop vlastnosti hráče
 */
void GameBase::get_player(Uint16 player_num, PlayerProperties & prop){
	players_[player_num]->get_properties(prop);
}

/*************** END OF class GameBase ******************************/
