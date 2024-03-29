
#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <algorithm>
#include "agar_helper.h"
#include "SDL_lib.h"
#include "constants.h"
#include "config.h"
#include "game.h"
#include "game_base.h"
#include "game_tools.h"
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
Game::Game(const GameBase & base, GameTools * gameTools):
			tools(gameTools), remaining_creatures_(0), remaining_periods_(500) {
// 	bool deathmatch, bool creatures, bool bombsatend){
	if(myself_ptr_)
		throw string("in Game constructor: another Game instance created.");
	myself_ptr_ = this;
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

	list< const MapObject * > bonuses(begin, end);
	list< const MapObject * >::iterator bonus_it;

	Uint16 x, y, random;

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
		count_bonuses = bonuses.size();

	for(field_it=fields.begin() ; count_fields && count_bonuses ; ++field_it){
		random = rand();
		if(count_bonuses> random%count_fields){
			bonus_it = bonuses.begin();
			random%=count_bonuses;
			while(random--) ++bonus_it;
			insert_MO_(*(bonus_it), 1, 1, field_it->first, field_it->second);
			bonuses.erase(bonus_it);
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
	// zjisteni policek pro vygenerovani prisery
	for(x=0 ; x<map_array_.size() ; ++x){
		for(y=0 ; y<map_array_[x].size() ; ++y){
			// pokud je zakazano z GameBase
			if(!base.allowed_array_[x][y].creature) continue;

			for(map_it=map_array_[x][y].begin();
				map_it!=map_array_[x][y].end(); ++map_it){
					if(isTypeOf::isWallBoxPlayer(*map_it)) break;
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
			players_[static_cast<Player *>(new_obj)->player_num()].player =
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
	bool at_end=false;
	players_it player_it;
	set_players_view_(window);
	// iterace dokud neni vyvolano zavreni okna
	SDLKey key;
	while(true){
		switch(get_event(key)){
			case SDL_VIDEORESIZE:
				set_players_view_(window);
				break;
			case SDL_QUIT:
				key = SDLK_ESCAPE;
			case SDL_KEYDOWN:
				if(key==SDLK_ESCAPE){
					AG_Quit();
					return;
				}
			default:
				break;
		}

		SDL_PumpEvents();// obnoveni stavu klavesnice

		// vykresleni scen pro jednotlive hrace
		for(player_it = players_.begin() ;
				player_it!=players_.end() ; ++player_it){

			SDL_SetClipRect(window, &player_it->second.win_view);
			draw_(window, player_it->first);
			player_it->second.player->draw_panel(
				window, player_it->second.win_view);
		}
		SDL_SetClipRect(window, 0);


		// cekani - chceme presny pocet obrazku za sekundu
// 		fps_last= SDL_fps(fps_last, fps);
		// zobrazeni na obrazovku
		SDL_Flip(window);

		this_time = SDL_GetTicks();
		time_to_use += (this_time - last_time)%(100*Config::get_instance()->move_period());
		while(time_to_use > MOVE_PERIOD){
			time_to_use -= Config::get_instance()->move_period();
			// hýbne světem
			if(move_()){
				// TODO deatchmatch
				if(!players_.size())
					return;

				set_players_view_(window);
			}
			// posune animace
			update_();
			// pokud jsme na konci, vycka zbyvajici dobu
			if(at_end && --remaining_periods_==0)
				return;
		}
		last_time = this_time;

		// zbyvaji nejake prisery?
		// TODO deatchmatch
		if(!remaining_creatures_)
			at_end=true;
	}
}

/**
 */
void Game::set_players_view_(SDL_Surface* window){
	clear_surface(Color::black, window);

	Uint16 half_w = window->w/2, half_h = window->h/2,
		win_w = window->w, win_h = window->h,
		map_w = map_width()*CELL_SIZE, map_h = map_height()*CELL_SIZE;
	players_it it, it2;
	switch(players_.size()){
		case 0: break;
		case 1:
			it = players_.begin();
			it->second.win_view.x =0;
			it->second.win_view.y =0;
			it->second.win_view.w = min(win_w, map_w);
			it->second.win_view.h =	min(win_h, map_h);
			break;
		case 3:
		case 4:
			for(it = players_.begin() ; it!=players_.end() ; ++it){
				it->second.win_view.w =
						min( static_cast<Uint16>(half_w-1), map_w);
				it->second.win_view.h =
						min( static_cast<Uint16>(half_h-1), map_h);
				switch(it->first){
					case 0:
						it->second.win_view.x =0;
						it->second.win_view.y =0;
						break;
					case 1:
						it->second.win_view.x =half_w+1;
						it->second.win_view.y =half_h+1;
						break;
					case 2:
						it->second.win_view.x =0;
						it->second.win_view.y =half_h+1;
						break;
					case 3:
						it->second.win_view.x =half_w+1;
						it->second.win_view.y =0;
						break;
				}
			}
			break;
		case 2:
			it = it2 = players_.begin();
			// rozdelit svisle
			if(map_w < window->w){

				// kdo bude vlevo
				if(it->first==1)
					++it;
				else
					++it2;
				it->second.win_view.x = 0;
				it2->second.win_view.x = half_w+1;
				it->second.win_view.y = it2->second.win_view.y = 0;
				it->second.win_view.w = it2->second.win_view.w =
					min( static_cast<Uint16>(half_w-1), map_w);
				it->second.win_view.h = it2->second.win_view.h =
					min( win_h, map_h);
			}
			// rozdelit vodorovne
			else{
				// kdo bude nahore
				if(it->first==1 || it->first==2)
					++it;
				else
					++it2;
				it->second.win_view.x = it2->second.win_view.x = 0;
				it->second.win_view.y = 0;
				it2->second.win_view.y = half_h+1;
				it->second.win_view.w = it2->second.win_view.w =
					min( win_w, map_w);
				it->second.win_view.h = it2->second.win_view.h =
					min( static_cast<Uint16>(half_h-1), map_h);
			}
			break;
	}
}

Uint16 Game::count_rect_shift_(Uint16 player_coor,
			Uint16 rect_half_size, Uint16 map_size) const{
	Uint16 rect_shift=0;
	if(player_coor > rect_half_size){
		rect_shift+= player_coor - rect_half_size;
		if(map_size-player_coor < rect_half_size){
			Uint16 rect_unshift = rect_half_size - (map_size-player_coor);
			if(rect_shift < rect_unshift)
				return 0;
			else
				rect_shift-= rect_unshift;
		}
	}
	return rect_shift;
}


void Game::draw_(SDL_Surface* window, Uint16 player_num){
	// obnovim map_view
	Uint16 i,
		shift_x = count_rect_shift_(players_[player_num].player->getX(),
			players_[player_num].win_view.w/2, map_width()*CELL_SIZE),
		shift_y = count_rect_shift_(players_[player_num].player->getY(),
			players_[player_num].win_view.h/2, map_height()*CELL_SIZE);

	players_[player_num].map_view.x=
		players_[player_num].win_view.x - shift_x;
	players_[player_num].map_view.y=
		players_[player_num].win_view.y - shift_y;
	// vykreslim pozadi a pak prisery
	for(i=0 ; i<2 ; ++i){
		draw_map_(i==0, window, players_[player_num].map_view,
			shift_x/CELL_SIZE, shift_y/CELL_SIZE,
			(shift_x+players_[player_num].win_view.w)/CELL_SIZE,
			(shift_y+players_[player_num].win_view.h)/CELL_SIZE );
	}
}
/** @details
 * Vykreslí nejdříve objekty pozadí (background, floorobject)
 * následně ostatní, až poté panely.
 * @param window surface okna pro vykreslení
 */
void Game::draw_map_(bool bg, SDL_Surface* window, const SDL_Rect & map_view,
			Uint16 from_x, Uint16 from_y, Uint16 to_x, Uint16 to_y){

	Uint16 column, field;
	map_array_t::value_type::value_type::iterator it, end_it;

	// poprve projdu mapu a vykreslim pozadi a objekty na pozadi
	// objekty na policku seradim
	for(field = from_y ; field<map_array_[0].size() ; ++field){
		for(column= from_x ; column< map_array_.size() ; ++column){
			if(bg){
				it = map_array_[column][field].begin();
				end_it = map_array_[column][field].end();
				// vykreslim pozadi a objekt na zemi
				while( (it=find_if(it, end_it, isTypeOf::isBgType)) !=end_it){
					(*it)->draw(window, map_view);
					++it;
				}
			}
			else {
				// objekty na policku seradim
				map_array_[column][field].sort(isUnder);
				// vykreslim ty co nejsou pozadi
				for(it= map_array_[column][field].begin() ;
						it!= map_array_[column][field].end() ;
						++it){

					if(isTypeOf::isBgType(*it)) continue;

					// v praxi potrebuju prohodit dva hrace pokud jsou v zakrytu
					// spodniho vykreslim vzdycky, horniho bud preskocim nebo vykreslim
					if( (*it)->type() == PLAYER && rand()%2 ){
						Player * player1 = static_cast<Player *>(*it);
						// !! jeste nevim jestli player2 je opravdu PLAYER !!
						--it;
						Player * player2 = static_cast<Player *>(*it);
						++it;
						if(player2->type() == PLAYER && *player1 == *player2)
							continue;
					}
					(*it)->draw(window, map_view);
				}
			}
			if(column > to_x) break;
		}
		if(field > to_y) break;
	}
}


/** @details
 * Projde všechny dynamické objekty hry a zavolá na nich fci move().
 * V průběhu shromažduje objekty, které nakonec vyhodí.
 * @return Vrací TRUE, pokud byl zahozen nějaký hráč.
 * @see DynamicMO::move()
 * @see remove_object()
 */
bool Game::move_(){
	bool removed_player = false;
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

		if((*it)->type()==PLAYER) removed_player = true;
		remove_object(*it);
	}
	return removed_player;
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
	// TODO deatchmatch
	return remaining_creatures_==0 && players_.size()>0;
}

/** @details
 * Zjistí jestli hráč žije, nastaví jeho vlastnosti.
 * @param player_num číslo hráče
 * @param prop vlastnosti hráče
 * @return Vrací TRUE, pokud hráč žije, jinak false.
 */
bool Game::get_player(Uint16 player_num, PlayerProperties & prop) const{
	players_t::const_iterator it = players_.find(player_num);
	if(it==players_.end())
		return false;
	it->second.player->get_properties(prop);
	return true;
}

/**
 * @param x x-ová souřadnice políčka
 * @param y y-ová souřadnice políčka
 * @return Vrací TRUE pokud lze zadané políčko přejít (není na něm zed ani bedna).
 */
bool Game::field_canGoOver(Uint16 x, Uint16 y, bool check_bomb){
	return !field_withObject(x, y,
		check_bomb ? isTypeOf::isWallBoxBomb : isTypeOf::isWallBox);
}

/**
 * @param x x-ová souřadnice políčka
 * @param y y-ová souřadnice políčka
 * @param objectType typ objektu, na který se ptáme
 * @return Vrací TRUE pokud je na zadaném políčku objekt zadaného typu.
 */
bool Game::field_withObject(Uint16 x, Uint16 y, const isTypeOf & isType){
	map_array_t::value_type::value_type::const_iterator begin, end;

	begin = map_array_[x][y].begin();
	end = map_array_[x][y].end();

	return find_if(begin, end, isType)!=end;
}

/**
 * @param x x-ová souřadnice políčka
 * @param y y-ová souřadnice políčka
 * @param objectType typ objektu, na který se ptáme
 * @return Vrací pointer na první objekt splnující náš požadavek,
 * pokud na zadaném políčku takový objekt není, vrací 0.
 */
MapObject * Game::field_getObject(Uint16 x, Uint16 y, const isTypeOf & isType){
	map_array_t::value_type::value_type::const_iterator begin, end;

	begin = map_array_[x][y].begin();
	end = map_array_[x][y].end();

	return (begin = find_if(begin, end, isType))!=end
		? *begin : 0;
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
	// jeste provedu akce zavisle na typu objektu
	switch(obj->type()){
		case CREATURE:
			// snizit pocet zbyvajicich priser
			--remaining_creatures_;
			break;
		case PLAYER:
			// zahodit hrace ze seznamu hrajicich
			players_.erase( static_cast<Player *>(obj)->player_num() );
			break;
		case BOMB_STAYING:
		case BOMB_MOVING:
			// vyhodit bombu ze seznamu hrace
			for(players_it it = players_.begin() ;
							it!=players_.end() ; ++it){
				it->second.bombs.remove( static_cast<Bomb *>(obj) );
			}
			break;
		default: break;
	}
	// zahodim obj
	delete obj;
}

/** @details
 * Vyhodí objekt ze starého políčka a vloží jej na nové.
 * @param old_x stará souřadnice v políčkách
 * @param old_y stará souřadnice v políčkách
 * @param new_x nová souřadnice v políčkách
 * @param new_y nová souřadnice v políčkách
 * @param obj pointer na objekt, který chceme přesunout
 */
void Game::change_position(Uint16 old_x, Uint16 old_y,
			Uint16 new_x, Uint16 new_y, MapObject * obj){
	map_array_[old_x][old_y].remove(obj);
	map_array_[new_x][new_y].push_back(obj);
}

/** @details
 * Umístí bombu do mapy,
 * přidá bombu do seznamu hráčových bomb.
 * @param player_num číslo hráče
 * @param x souřadnice bomby v políčkách
 * @param y souřadnice bomby v políčkách
 * @param bomb pointer na bombu, kterou chceme položit
 */
void Game::plant_bomb(Uint16 player_num, Uint16 x, Uint16 y, Bomb* bomb){
	insert_object(x, y, bomb);
	players_[player_num].bombs.push_back(bomb);
}

/** @details
 * Spočítá položené nevybouchlé bomby hráče.
 * @param player_num číslo hráče
 * @return Počet bomb.
 */
Uint16 Game::count_bombs(Uint16 player_num){
	Uint16 count = 0;
	bombs_t::const_iterator it;
	for(it = players_[player_num].bombs.begin() ;
				it!=players_[player_num].bombs.end() ; ++it){
		if(!(*it)->is_mega())
			++count;
	}
	return count;
}

/** @details
 * Odstraní položeným bombám hráče ruční odpalování.
 * @param player_num číslo hráče
 */
void Game::remove_bombs_timer(Uint16 player_num){
	bombs_t::iterator it;
	for(it = players_[player_num].bombs.begin() ;
			it != players_[player_num].bombs.end() ; ++it){
		(*it)->remove_timer();
	}
}

/** @details
 * Nechá vybouchnout první bombu hráče, pokud nějaká je.
 * @param player_num číslo hráče
 * @return Vrací TRUE pokud má hráč položenou alespon jednu bombu.
 */
void Game::explode_bomb(Uint16 player_num){
	if(!players_[player_num].bombs.empty())
		players_[player_num].bombs.front()->explode();
}

Uint16 Game::map_height() const {
	return map_array_[0].size();
}

Uint16 Game::map_width() const {
	return map_array_.size();
}

/*************** END OF class Game ******************************/
