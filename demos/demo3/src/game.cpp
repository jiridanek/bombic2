
#include <iostream>
// #include <list>
#include <vector>
#include <string>
#include <utility>
#include <algorithm>
#include "SDL_lib.h"
// #include "tixml_helper.h"
// #include "stl_helper.h"
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
#include "game_bomb.h"
#include "game_flame.h"

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
Game::Game(const GameBase & base, GameTools * gameTools): tools(gameTools) {
// 	bool deathmatch, bool creatures, bool bombsatend){
	if(myself_ptr_)
		throw string("in Game constructor: another Game instance created.");
	myself_ptr_ = this;
	// pripravit prostor pro ctyry hrace
	for(Uint16 i=1; i<=4 ; ++i){
		players_[i]=0;
	}
	// zkusit nahrat umistene a vygenerovat ostatni objekty
	try{
		load_placed_MOs_(base.base_array_);
		load_generated_MOs_(base);
	}
	catch(...){
		myself_ptr_ = 0;
		throw;
	}
}

/** @details
 * Zruší nejdříve všechny statické,
 * následně všechny dynamické objekty v mapě.
 * Vynuluje myself_pointer_.
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

	myself_ptr_ = 0;
}

/** @details
 * Okopíruje zadanou mapu, jednak vytvoří základní rozměry mapy,
 * jednak okopíruje umístěné objekty.
 * @param base_array pole s pevně umístěnými objekty
 */
void Game::load_placed_MOs_(const GameBase::base_array_t & base_array){
	// vytvoreni zakladniho rozmeru mapy
	map_array_t::value_type empty_column;
	map_array_t::value_type::value_type empty_list;
	empty_column.insert(empty_column.end(), base_array[0].size(), empty_list);
	map_array_.insert(map_array_.end(), base_array.size(), empty_column);
	// okopirovani objektu
	GameBase::base_array_t::value_type::value_type::const_iterator it;

	Uint16 field, column;

	for(field = 0 ; field<base_array[0].size() ; ++field){
		for(column=0 ; column< base_array.size() ; ++column){
			for(it= base_array[column][field].begin();
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
void Game::load_generated_MOs_(const GameBase & base){
	GameBase::generatedMOs_t::const_iterator
		it_first= base.generatedMOs_.begin(),
		it_second= it_first,
		end_it= base.generatedMOs_.end();

	// bedny
	if(it_first==end_it) return;
	isTypeOf isCurType(BOX);

	while(isCurType(*it_second) && ++it_second!=end_it);
	generate_boxes_(base, it_first, it_second);

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
	generate_creatures_(base, it_first, it_second);

	if(it_second!=end_it)
		throw string("in Game::load_generated_MOs_(): unexpected object type to generate or objects in wrong order.");
}

/** @details
 * Zjistí volná políčka pro vygenerování beden a pokusí se náhodně
 * rozmístit všechny bedny v zadaném intervalu.
 * @param base reference na GameBase kvuli allowed_array_ a allowed_boxes_count_
 * @param begin iterator s nahodnym pristupem, začátek beden
 * @param end iterator s nahodnym pristupem za koncem beden
 */
void Game::generate_boxes_(const GameBase & base,
			GameBase::generatedMOs_t::const_iterator begin,
			GameBase::generatedMOs_t::const_iterator end){

	Uint16 x, y,
		// pocet zbyvajicich volnych policek
		count_free = base.allowed_boxes_count_,
		// pocet zbyvajicich boxu k rozdeleni
		count = end-begin;

	for(x=0 ; x<map_array_.size() ; ++x){
		for(y=0 ; y<map_array_[x].size() ; ++y){
			// neni jiz zadne volne policko, nebo co rozdelovat
			if(!count_free || !count) return;
			if(!base.allowed_array_[x][y].box) continue;

			if(count> rand()%count_free){
				insert_MO_(*(begin+count-1), 1, 1, x, y);
				--count;
			}
			--count_free;
		}
	}
}

/** @details
 * Zjistí políčka pro vygenerování bonusů a pokusí se náhodně
 * rozmístit všechny bonusy v zadaném intervalu.
 * @param begin začátek bonusů
 * @param end konec bonusů
 */
void Game::generate_bonuses_(
			GameBase::generatedMOs_t::const_iterator begin,
			GameBase::generatedMOs_t::const_iterator end){

	vector< pair<Uint16, Uint16> > fields;
	vector< pair<Uint16, Uint16> >::iterator field_it;

	Uint16 x, y;

	// zjisteni policek s bednami
	for(x=0 ; x<map_array_.size() ; ++x){
		for(y=0 ; y<map_array_[x].size() ; ++y){
			if(field_withObject(x, y, BOX))
				fields.push_back(make_pair(x,y));
		}
	}

		// pocet zbyvajicich volnych policek
	Uint16 count_fields = fields.size(),
		// pocet zbyvajicich bonusu k rozdeleni
		count_bonuses = end-begin;

	for(field_it=fields.begin() ; count_fields && count_bonuses ; ++field_it){
		if(count_bonuses> rand()%count_fields){
			insert_MO_(*(begin+count_bonuses-1), 1, 1, field_it->first, field_it->second);
			--count_bonuses;
		}
		--count_fields;
	}
}

/** @details
 * Zjistí volná políčka pro vygenerování příšer a pokusí se náhodně
 * rozmístit všechny příšery v zadaném intervalu.
 * @param base reference na GameBase kvuli allowed_array_
 * @param begin začátek příšer
 * @param end konec příšer
 */
void Game::generate_creatures_(const GameBase & base,
			GameBase::generatedMOs_t::const_iterator begin,
			GameBase::generatedMOs_t::const_iterator end){

	vector< pair<Uint16, Uint16> > fields;
	vector< pair<Uint16, Uint16> >::iterator field_it;


	map_array_t::value_type::value_type::iterator map_it;

	Uint16 x, y;
	isTypeOf isBad(WALL); isBad.addType(BOX).addType(PLAYER);
	// zjisteni policek pro vygenerovani prisery
	for(x=0 ; x<map_array_.size() ; ++x){
		for(y=0 ; y<map_array_[x].size() ; ++y){
			// pokud je zakazano z GameBase
			if(!base.allowed_array_[x][y].creature) continue;

			for(map_it=map_array_[x][y].begin();
				map_it!=map_array_[x][y].end(); ++map_it){
					if(isBad(*map_it)) break;
			}
			// pouze pokud nevypadl na isBad
			if(map_it==map_array_[x][y].end())
				fields.push_back(make_pair(x,y));
		}
	}
	// neni volne policko
	if(fields.empty()) return;
	// kazdou priseru umistim
	for( ; begin!=end ; ++begin){
		field_it = fields.begin()+rand()%fields.size();
		insert_MO_(*begin, 1, 1, field_it->first, field_it->second);
	}
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

	Uint16 x=column*CELL_SIZE, y=field*CELL_SIZE;
	MapObject* new_obj;
	// vytvoreni objektu
	switch(mapObject->type()){
		case BACKGROUND:
			new_obj = new Background(
				*static_cast<const Background*>(mapObject), x, y);
			staticMOs_.push_back(static_cast<StaticMO*>(new_obj));
			break;
		case FLOOROBJECT:
			new_obj = new Floorobject(
				*static_cast<const Floorobject*>(mapObject), x, y);
			staticMOs_.push_back(static_cast<StaticMO*>(new_obj));
			break;
		case WALL:
			new_obj = new Wall(
				*static_cast<const Wall*>(mapObject), x, y, height);
			staticMOs_.push_back(static_cast<StaticMO*>(new_obj));
			break;
		case BOX:
			new_obj = new Box(
				*static_cast<const Box*>(mapObject), x, y);
			dynamicMOs_.push_back(static_cast<DynamicMO*>(new_obj));
			break;
		case BONUS:
			new_obj = new Bonus(
				*static_cast<const Bonus*>(mapObject), x, y);
			dynamicMOs_.push_back(static_cast<DynamicMO*>(new_obj));
			break;
		case CREATURE:
			new_obj = new Creature(
				*static_cast<const Creature*>(mapObject),
					x+CELL_SIZE/2, y+CELL_SIZE/2);
			dynamicMOs_.push_back(static_cast<DynamicMO*>(new_obj));
			// zvysim pocet priser
			++remaining_creatures_;
			break;
		case PLAYER:
			new_obj = new Player(
				*static_cast<const Player *>(mapObject),
					x+CELL_SIZE/2, y+CELL_SIZE/2);
			dynamicMOs_.push_back(static_cast<DynamicMO*>(new_obj));
			// pridam hrace mezi hrace
			players_[static_cast<Player *>(new_obj)->player_num()] =
				static_cast<Player *>(new_obj);
			break;
		default:
			throw string("in Game::insert_MO_(): unhandled object type");
	}
	// vlozeni do mapy na vsechna policka ktera objekt zabira
	for(x=column ; x< column+width ; ++x){
		for(y=field ; y< field+height ; ++y){
			// pokud se vejde do mapy
			if(x<map_array_.size() && y<map_array_[x].size()){
				if(new_obj->type()==BACKGROUND)
					map_array_[x][y].push_front(new_obj);
				else
					map_array_[x][y].push_back(new_obj);
			}
		}
	}
}

/** @details
 * V realtime modelu spustí hru jako takovou, vykresluje objekty,
 * hýbe objekty, hlídá jestli hra skončila.
 * @param window surface okna pro vykreslení
 */
void Game::play(SDL_Surface* window){
	// konstrukce zarizujici spravny pocet obrazku za sekundu
// 	Uint8 fps=10;
// 	Uint32 fps_last= 0;

	int last_time = SDL_GetTicks(),
		time_to_use = 0, this_time=last_time;
	// iterace dokud neni vyvolano zavreni okna
	while(!get_event_isquit(SDLK_ESCAPE)) {
		SDL_PumpEvents();// obnoveni stavu klavesnice

		draw_(window);

		// cekani - chceme presny pocet obrazku za sekundu
// 		fps_last= SDL_fps(fps_last, fps);
		// zobrazeni na obrazovku
		SDL_Flip(window);

		this_time = SDL_GetTicks();
		time_to_use += this_time - last_time;
		for( ; time_to_use > MOVE_PERIOD; time_to_use -= MOVE_PERIOD){
			// hýbne světem
			move_();
			// posune animace
			update_();
		}
		last_time = this_time;

		for(Uint16 i=4 ; true ; --i){
			if(i==0) return;
			if(players_[i]) break;
		}
	}
}

/** @details
 * Vykreslí nejdříve objekty pozadí (background, floorobject)
 * následně ostatní.
 * @param window surface okna pro vykreslení
 */
void Game::draw_(SDL_Surface* window){
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


/** @details
 * Projde všechny dynamické objekty hry a zavolá na nich fci move().
 * V průběhu shromažduje objekty, které nakonec vyhodí.
 * @see DynamicMO::move()
 * @see remove_object()
 */
void Game::move_(){
	dynamicMOs_t::iterator it;
	dynamicMOs_t removedDynamicMOs;
	// tahnout
	for(it= dynamicMOs_.begin() ; it!= dynamicMOs_.end() ; ++it){
		if( (*it)->move() )
			removedDynamicMOs.push_back(*it);
	}
	// zahodit
	for(it= removedDynamicMOs.begin() ;
			it!= removedDynamicMOs.end() ; ++it){
		remove_object(*it);
	}
}

/** @details
 * Projde všechny dynamické i statické objekty hry a zavolá na nich fci update().
 * @see MapObject::update()
 */
void Game::update_(){
	dynamicMOs_t::iterator d_it;
	staticMOs_t::iterator s_it;
	// dynamicke
	for(d_it= dynamicMOs_.begin() ; d_it!= dynamicMOs_.end() ; ++d_it){
		(*d_it)->update();
	}
	// staticke
	for(s_it= staticMOs_.begin() ; s_it!= staticMOs_.end() ; ++s_it){
		(*s_it)->update();
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
 * @param objectType typ objektu, na který se ptáme
 * @return Vrací TRUE pokud je na zadaném políčku objekt zadaného typu.
 */
bool Game::field_withObject(Uint16 x, Uint16 y, OBJECT_TYPES objectType){
	map_array_t::value_type::value_type::const_iterator begin, end;
	isTypeOf isType(objectType);

	begin = map_array_[x][y].begin();
	end = map_array_[x][y].end();

	return find_if(begin, end, isType)!=end;
}

/** @details
 * Vloží dynamický objekt do mapy a seznamu dynamických objektů.
 * Instance Game se později postará o jeho dealokování.
 * @param x souřadnice políčka pro vložení
 * @param y souřadnice políčka pro vložení
 * @param obj objekt, který chceme vložit
 */
void Game::insert_object(Uint16 x, Uint16 y, DynamicMO * obj){
	dynamicMOs_.push_back(obj);
	map_array_[x][y].push_back(obj);
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
	switch(obj->type()){
		case CREATURE:
			--remaining_creatures_;
			break;
		case PLAYER:
			players_[static_cast<Player *>(obj)->player_num()]=0;
			break;
		default: ;
	}
	// zahodim obj
	delete obj;
}

/** @details
 * @TODO
 */
void Game::change_position(Uint16 old_x, Uint16 old_y,
			Uint16 new_x, Uint16 new_y, MapObject * obj){
	map_array_[old_x][old_y].remove(obj);
	map_array_[new_x][new_y].push_back(obj);
}

/*************** END OF class Game ******************************/
