
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
#include "game_base.h"
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
Game* Game::myself_ptr_ = 0;

Game* Game::get_instance(){
	if(!myself_ptr_)
		throw string("in Game::get_instance(): no Game instance created.");
	return myself_ptr_;
}


/**
 * @param players_count počet hráčů
 * @param mapname název mapy
 */
Game::Game(const GameBase & base){
// 	bool deathmatch, bool creatures, bool bombsatend){
	if(myself_ptr_)
		throw string("in Game contructor: another Game instance created.");
	myself_ptr_ = this;
	try{
		load_placed_MOs_(base.base_array_);
		load_generated_MOs_(base.generated_MOs_);
	}
	catch(...){
		myself_ptr_ = 0;
		throw;
	}
}

/** @details
 * Okopíruje zadanou mapu, jednak vytvoří základní rozměry mapy,
 * jednak okopíruje umístěné objekty.
 * @param base_array pole s pevně umístěnými objekty
 */
void Game::load_placed_MOs_(const GameBase::base_array_t base_array){
	// vytvoreni zakladniho rozmeru mapy
	map_array_t::value_type column;
	map_array_t::value_type::value_type empty_list;
	column.insert(column.end(), base_array[0].size(), empty_list);
	map_array_.insert(map_array_.end(), base_array.size(), column);
	// okopirovani objektu
	GameBase::base_array_t::value_type::value_type::iterator it;
	Uint16 field, column, x, y;
	MapObject *new_obj;

	for(field = 0 ; field<base_array[0].size() ; ++field){
		for(column=0 ; column< base_array.size() ; ++column){
			for(it= base_array[column][field].begin() ;
					it!= base_array[column][field].end() ;
					++it){
				insert_MO_(it->o, it->w, it->h, column, field);
			}
		}
	}
}

/** @details
 * Vygeneruje v pořadí bedny, bonusy a příšery.
 * @param generatedMOs hezky seřazený seznam objektů k vygenerování
 * @throw string Chybové hlášení, pokud není seznam hezky seřazený,
 * nebo obsahuje něco, co nemá.
 */
void Game::load_generated_MOs_(const GameBase::generatedMOs_t & generatedMOs){
	GameBase::generatedMOs_t::iterator
		it_first= generatedMOs.begin(),
		it_second= it_first,
		end_it= generatedMOs.end();
	// bedny
	if(it_first==end_it) return;
	isTypeOf isCurType(BOX);
	while(isCurType(*it_second) && ++it_second!=end_it);
	generate_boxes_(it_first, it_second);
	it_first= it_second;
	// bonusy
	if(it_first==end_it) return;
	isCurType.clear().addType(BONUS);
	while(isCurType(*it_second) && ++it_second!=end_it);
	generate_bonuses_(it_first, it_second);
	it_first= it_second;
	// nestvury
	if(it_first==end_it) return;
	isCurType.clear().addType(CREATURE);
	while(isCurType(*it_second) && ++it_second!=end_it);
	generate_creatures_(it_first, it_second);

	if(it_second!=end_it)
		throw string("in Game::load_generated_MOs_(): unexpected object type to generate");
}

/** @details
 * Zjistí volná políčka pro vygenerování beden a pokusí se náhodně
 * rozmístit všechny bedny v zadaném intervalu.
 * @param begin iterator s nahodnym pristupem, začátek beden
 * @param end iterator s nahodnym pristupem za koncem beden
 */
void Game::generate_boxes_(const GameBase & base,
			GameBase::generatedMOs_t::iterator begin,
			GameBase::generatedMOs_t::iterator end){

	Uint16 x, y,
		count_free = base.boxes_array_count_+1,
		count = end-begin;

	for(x=0 ; x<map_array_.size() ; ++x){
		for(y=0 ; y<map_array_[x].size() ; ++y){
			if(!base.boxes_array_[x][y]) continue;
			if(--count_free==0) return;

			if(count> rand()%count_free){
				insert_MO_(*(begin+count-1), 1, 1, x, y);
				if(--count==0) return;
			}
		}
	}
}

/** @details
 * Zjistí políčka pro vygenerování bonusů a pokusí se náhodně
 * rozmístit všechny bonusy v zadaném intervalu.
 * @param begin začátek bonusu
 * @param end konec bonusu
 */
void Game::generate_bonuses_(GameBase::generatedMOs_t::iterator begin,
			GameBase::generatedMOs_t::iterator end){
	// TODO
}

/** @details
 * Zjistí volná políčka pro vygenerování příšer a pokusí se náhodně
 * rozmístit všechny příšery v zadaném intervalu.
 * @param begin začátek příšer
 * @param end konec příšer
 */
void Game::generate_creatures_(GameBase::generatedMOs_t::iterator begin,
			GameBase::generatedMOs_t::iterator end){
	// TODO
}


/** @details
 * Vytvoří objekt,
 * vloží jej do seznamu statických nebo dynamických objektů,
 * vloží ho do mapy na všechna políčka, která zabírá.
 * @param mapObject objekt k okopírování
 * @param width počet políček zabíraných v mapě na šířku
 * @param height počet políček zabíraných v mapě na výšku
 * @param column sloupec mapy, ve kterém je levý roh objektu
 * @param field řádek mapy, ve kterém je horní roh objektu
 * @throw string chybové hlášení při vkládání objektu, jehož typ není ve výčtu.
 * Typicky se stane, že programátor přidá typ objektu a zapomene ho přidat i sem.
 */
void Game::insert_MO_(const MapObject* mapObject,
		Uint16 width, Uint16 height, Uint16 column, Uint16 field){

	// vytvoreni objektu
	switch(mapObject->type()){
		case BACKGROUND:
			new_obj = new Background(mapObject, column, field);
			staticMOs_.push_back(new_obj);
			break;
		case FLOOROBJECT:
			new_obj = new Floorobject(mapObject, column, field);
			staticMOs_.push_back(new_obj);
			break;
		case WALL:
			new_obj = new Wall(mapObject, column, field);
			staticMOs_.push_back(new_obj);
			break;
		case BOX:
			new_obj = new Box(mapObject, column, field);
			dynamicMOs_.push_back(new_obj);
			break;
		case BONUS:
			new_obj = new Bonus(mapObject, column, field);
			dynamicMOs_.push_back(new_obj);
			break;
		case CREATURE:
			new_obj = new Creature(mapObject, column, field);
			dynamicMOs_.push_back(new_obj);
			break;
		case PLAYER:
			new_obj = new Player(mapObject, column, field);
			dynamicMOs_.push_back(new_obj);
			break;
		default:
			throw string("in Game::insert_MO_(): unhandled object type");
	}
	Uint16 x, y;
	// vlozeni do mapy na vsechna policka ktera objekt zabira
	for(x=column ; x< column+width ; ++x){
		for(y=field ; y< field+height ; ++y){
			// pokud se vejde do mapy
			if(x<map_array_.size() && y<map_array[x].size())
				map_array_[x][y].push_back(new_obj);
		}
	}
}


/**
 * @param boxesEl element v XML souboru specifikující bedny v mapě
 */
void Game::load_boxes_(TiXmlElement *boxesEl){
	// nenalezen nepovinny tag boxes
	if(!boxesEl) return;

	TiXmlDocument doc;
	TiXmlElement *rootEl, *El;

	// nacteni atributu
	string name_def, filename;
	Surface sur_src;
	Animation anim_def, anim_burning_def, anim, anim_burning;
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

		toplapping_def=
		load_subEl_animation_(rootEl, "img", anim_def, sur_src);
		load_subEl_animation_(rootEl, "burning", anim_burning_def, sur_src);
	}
	catch(string s){
		TiXmlError(filename,s);
	}
	// kontrola jestli koresponduji rozmery obrazku
	if(height_def!=(anim_def.height()-toplapping_def)/CELL_SIZE)
		TiXmlError(filename,"the value of attribute height doesn't correspond with value in <walls ...>.");

	if(width_def!= anim_def.width()/CELL_SIZE)
		TiXmlError(filename,"the value of attribute width doesn't correspond with value in <walls ...>.");

	// vytvoreni pevnych boxu
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

				toplapping =
				load_subEl_animation_(rootEl, "img", anim_def, sur_src);
				load_subEl_animation_(rootEl, "burning", anim_burning_def, sur_src);

				// kontrola jestli koresponduji rozmery obrazku
				if(height!=(anim.height()-toplapping)/CELL_SIZE)
					TiXmlError(filename,"the value of attribute height doesn't correspond with value in <boxes ...>.");

				if(width!= anim.width()/CELL_SIZE)
					TiXmlError(filename,"the value of attribute width doesn't correspond with value in <boxes ...>.");
				// vlozeni do mapy
				insert_box_(anim, anim_burning, toplapping, x, y);
			}
			else{
				// vlozeni do mapy
				insert_box_(anim_def, anim_burning_def, toplapping_def, x, y);
			}
			--count;
			El= El->NextSiblingElement("box");
		}
	}
	catch(string s){
		TiXmlError(filename,s);
	}
	// rozsahlejsi bedny nebudou nahodne generovany
	if(height_def!=1 || width_def!=1) return;

	for(x=0; x<static_cast<int>(map_array_.size()) ; ++x){
		for(y=0 ; y<static_cast<int>(map_array_[x].size()) ; ++y){
			if(noboxes[x][y]) continue;
			if(!isBG(map_array_[x][y].back())) continue;
			// jsem na prazdnem policku
			if(count/count_free >= SDL_Rand()){
				insert_box_(anim_def, anim_burning_def, toplapping_def, x, y);
				if(--count==0) return;
			}
			if(--count_free<=0) return;
		}
	}
}

/**
 * @param bonusEl element v XML souboru specifikující bonus
 */
void Game::load_bonuses_(TiXmlElement *bonusEl){
	string filename;
	int x,y, count;

	Surface sur_src;
	Animation anim;
	// dvojice velky obrazek do mapy a maly obrazek do panelu
	vector< Animation > bonuses;
	vector< Animation >::iterator it;

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
			load_subEl_animation_(rootEl, "img", anim, sur_src);
			// do seznamu nezarazenych bonusu pridam bonus count krat
			while(count--){
				// TODO ruzne bonusy
				bonuses.push_back(anim);
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

/**
 * @param creaturesEl element v XML souboru specifikující nestvůry
 */
void Game::load_creatures_(TiXmlElement *creaturesEl){
	string filename;
	int x,y, count, width, height;
	Uint16 speed, lives, intelligence;
	Surface sur_src, sur_src_s;
	// seznam prazdnych policek
	vector< pair<Uint16, Uint16> > empty_fields;
	vector< pair<Uint16, Uint16> >::iterator it;
	// naplneni seznamu prazdnych policek
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

			// vyska a sirska obrazku
			attr_HeightWidth(rootEl, height, width);
			if(height<1) TiXmlError(filename,"missing attribute height");
			if(width<1) TiXmlError(filename,"missing attribute width");
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
					attr_XY(El, x, y);
				}
				catch(string s){
					TiXmlError("in element <creature ...>: "+s);
				}
				if(x>=static_cast<Sint16>(map_array_.size())
				|| y>=static_cast<Sint16>(map_array_[x].size()) ) continue;
				if(isBadType(map_array_[x][y].back())) continue;
				insert_creature_(anim_up, anim_right, anim_down, anim_left,
					anim_burned, x, y, speed, lives, intelligence);
				--count;
			}

			// nahodne rozlozeni priser
			while(count--){
				// nahodne policko
				it = empty_fields.begin()+ rand() % empty_fields.size();
				insert_creature_(anim_up, anim_right, anim_down,
					anim_left, anim_burned, it->first, it->second,
					speed, lives, intelligence);
			}
			creaturesEl= creaturesEl->NextSiblingElement("creatures");
		}
	}
	catch(string s){
		TiXmlError(filename,s);
	}
}

/* * @details
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
 /*
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
}*/

/** @details
 * Vytvoří Animation objektu popsaného podelementem se zadaným jménem.
 * Vrácené surface má nastavenou průhlednou barvu, nikoli však průhlednost.
 * Zjistí také jaké měl podelement nastavené toplapping (výška nevyužitá v mapě).
 * @throw string Při chybě (nenalezení podelementu nebo některého povinného atributu)
 * vyvolá výjimku s chybovým hlášením.
 * @param El rodič hledaného podelementu
 * @param name_subEl název hledaného podelementu
 * @param sur_src zdrojový surface
 * @param anim_dst
 * @return Vrací toplapping - hodnotu atributu nebo nulu pokud nebyl nalezen.
 * @see subElement(), readAttr()
 */
Uint16 Game::load_subEl_animation_( TiXmlElement *El, const char* name_subEl,
			Animation & anim_dst, const Surface & sur_src){

	Uint16 w, h, toplapping;
	El = subElement(El,name_subEl);
	if(!readAttr(El, "width", w, false))
			w= CELL_SIZE;
	else if(w<1)
		throw string("the value of width must be higher than 0");
	else	w*= CELL_SIZE;
	if(!readAttr(El, "height", h, false))
			h= CELL_SIZE;
	else if(h<1)
		throw string("the value of height must be higher than 0");
	else	h*= CELL_SIZE;
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
 * @param anim animace vkládané zdi
 * @param toplapping počet políček odshora obrázku,
 * které nezabírají místo v mapě
 * @param x souřadnice levého rohu zdi v mapě
 * @param y souřadnice horního rohu zdi v mapě
 */
void Game::insert_wall_(const Animation & anim,
				Uint16 toplapping, Uint16 x, Uint16 y){
	// vytvorit a ulozit do seznamu statickych objektu
	staticMOs_.push_back(new Wall(anim, x*CELL_SIZE, (y-toplapping)*CELL_SIZE) );
	// ulozit do mapy na spravna policka
	for(Uint16 column=0; column<anim.width()/CELL_SIZE ; ++column){
		for(Uint16 field=0 ; field<(anim.height()-toplapping)/CELL_SIZE
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
 * @param anim animace vkládaného objektu
 * @param x souřadnice levého rohu objektu v mapě
 * @param y souřadnice horního rohu objektu v mapě
 */
void Game::insert_floorobject_(const Animation & anim, Uint16 x, Uint16 y){
	// vytvorit a ulozit do seznamu statickych objektu
	staticMOs_.push_back(new Floorobject(anim, x*CELL_SIZE, y*CELL_SIZE));
	// ulozit do mapy na spravna policka
	for(Uint16 column=0; column<anim.width()/CELL_SIZE ; ++column){
		for(Uint16 field=0 ; field<anim.height()/CELL_SIZE ; ++field){
			if(x+column>=map_array_.size() || y+field>=map_array_[0].size())
				continue;
			map_array_[x+column][y+field].push_back(staticMOs_.back());
		}
	}
}

/** @details
 * Vytvoří bednu na zadaných souřadnicích a vloží ji do mapy.
 * Bednu vkládá do mapy na všechna políčka, která zabírá.
 * @param anim animace vkládané bedny
 * @param anim_burning animace hořící bedny
 * @param toplapping počet políček odshora obrázku,
 * které nezabírají místo v mapě
 * @param x souřadnice levého rohu bedny v mapě
 * @param y souřadnice horního rohu bedny v mapě
 */
void Game::insert_box_(const Animation & anim, const Animation & anim_burning,
				Uint16 toplapping, Uint16 x, Uint16 y){
	// vytvorit a ulozit do seznamu dynamickych objektu
	dynamicMOs_.push_back(new Box(anim, anim_burning, toplapping, x*CELL_SIZE, y*CELL_SIZE) );
	// ulozit do mapy na spravna policka
	for(Uint16 column=0; column<anim.width()/CELL_SIZE ; ++column){
		for(Uint16 field=0 ; field<(anim.height()-toplapping)/CELL_SIZE
							; ++field){
			if(x+column>=map_array_.size() || y+field>=map_array_[0].size())
				continue;
			map_array_[x+column][y+field].push_back(dynamicMOs_.back());
		}
	}
}

/** @details
 * Vytvoří bonus na zadaných souřadnicích a vloží ho do mapy.
 * @param anim animace vkládaného bonusu (do mapy)
 * @param x souřadnice bonusu v mapě
 * @param y souřadnice bonusu v mapě
 */
void Game::insert_bonus_(const Animation & anim, Uint16 x, Uint16 y){
	// vytvorit a ulozit do seznamu dynamickych objektu
	dynamicMOs_.push_back(new Bonus(anim, x*CELL_SIZE, y*CELL_SIZE) );
	// ulozit do mapy na spravne policko
	if(x>=map_array_.size() || y>=map_array_[0].size())
		return;
	map_array_[x][y].push_back(dynamicMOs_.back());
}

/** @details
 * Vytvoří nestvůru na zadaných souřadnicích a vloží ji do mapy.
 * @param anim_left animace pro otočení doleva
 * @param anim_up animace pro otočení nahoru
 * @param anim_right animace pro otočení doprava
 * @param anim_down animace pro otočení dolů
 * @param anim_burned animace pro umírání
 * @param x souřadnice bonusu v mapě
 * @param y souřadnice bonusu v mapě
 * @param speed rychlost v pixelech za jednu časovou jednotku
 * @param lives počet životů
 * @param ai kvalita umělé inteligence
 */
void Game::insert_creature_(const Animation & anim_up, const Animation & anim_right,
			const Animation & anim_down, const Animation & anim_left,
			const Animation & anim_burned, Uint16 x, Uint16 y,
			Uint16 speed, Uint16 lives, Uint16 ai){

	// vytvorit a ulozit do seznamu dynamickych objektu
	dynamicMOs_.push_back(new Creature(anim_up, anim_right, anim_down, anim_left, anim_burned,
			x*CELL_SIZE+CELL_SIZE/2, y*CELL_SIZE+CELL_SIZE/2,
			speed, lives, ai) );
	// ulozit do mapy na spravne policko
	if(x>=map_array_.size() || y>=map_array_[0].size())
		return;
	map_array_[x][y].push_back(dynamicMOs_.back());
}

/** @details
 * Vytvoří hráče na zadaných souřadnicích a vloží jej do mapy.
 * @param anim_left animace pro otočení doleva
 * @param anim_up animace pro otočení nahoru
 * @param anim_right animace pro otočení doprava
 * @param anim_down animace pro otočení dolů
 * @param anim_burned animace pro umírání
 * @param x souřadnice bonusu v mapě
 * @param y souřadnice bonusu v mapě
 * @param speed rychlost v pixelech za jednu časovou jednotku
 * @param lives počet životů
 */
void Game::insert_player_(const Animation & anim_up, const Animation & anim_right,
			const Animation & anim_down, const Animation & anim_left,
			const Animation & anim_burned, Uint16 x, Uint16 y,
			Uint16 speed, Uint16 lives){
	// vytvorit a ulozit do seznamu dynamickych objektu
	dynamicMOs_.push_back(new Player(
			anim_up, anim_right, anim_down, anim_left, anim_burned,
			x*CELL_SIZE+CELL_SIZE/2, y*CELL_SIZE+CELL_SIZE/2,
			speed, lives) );
	// ulozit do mapy na spravne policko
	if(x>=map_array_.size() || y>=map_array_[0].size())
		return;
	map_array_[x][y].push_back(dynamicMOs_.back());
}

/** @details
 * Zruší nejdříve všechny statické,
 * následně všechny dynamické objekty v mapě.
 */
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

/** @details
 * Vykreslí nejdříve objekty pozadí (background, floorobject)
 * následně ostatní.
 * @param window surface okna pro vykreslení
 */
void Game::draw(SDL_Surface* window){
	Uint16 column, field;
	map_array_t::value_type::value_type::iterator it, end_it;
	isTypeOf isBgType;
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
			}
		}
	}
}

/**
 * @param player_num číslo (pořadí) hráče
 * @param lives počet životů
 * @param bombs počet bomb, jimiž disponuje
 * @param flames velikost plamene
 * @param boots počet botiček
 */
void Game::set_player(Uint16 player_num, Uint16 lives,
	Uint16 bombs, Uint16 flames, Uint16 boots){
}

/** @details
 * Projde všechny dynamické objekty hry a zavolá na nich fci move().
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

/**
 * @param x x-ová souřadnice políčka
 * @param y y-ová souřadnice políčka
 * @return Vrací TRUE pokud je na zadaném políčku příšera.
 */
bool Game::field_withCreature(Uint16 x, Uint16 y){
	map_array_t::value_type::value_type::const_iterator begin, end;
	isTypeOf isCreature(CREATURE);

	begin = map_array_[x][y].begin();
	end = map_array_[x][y].end();

	return find_if(begin, end, isCreature)!=end;
}

/** @details
 * Specifikovaný objekt vyhodí ze seznamu dynamických objektů,
 * z mapy a následně jej dealokuje!
 * @param obj dynamický objekt, který chceme vyhodit
 */
void Game::remove_object(DynamicMO * obj){
	Uint16 x, y;
	// vyhodim obj z mapy
	for(x = 0 ; x<map_array_.size() ; ++x){
		for(y=0 ; y<map_array_[x].size() ; ++y){
			map_array_[x][y].remove(obj);
		}
	}
	// vyhodim obj ze seznamu dynamickych objektu
	dynamicMOs_.remove(obj);
	// zahodim obj
	delete obj;
}

/*************** END OF class Game ******************************/

