
// #include <iostream>
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
#include "game_bonus_application.h"
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
Game::Game(const GameBase & base, GameTools * gameTools,
				bool deathmatch, bool bombsatend):
			tools(gameTools), remaining_creatures_(0),
			remaining_periods_(500),
			deathmatch_(deathmatch), bombsatend_(bombsatend) {
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
 * Nemaže presumpce, ty si obstará bomba.
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
		// presumpce obstará bomba
		if((*it)->type()==PRESUMPTION)
			continue;
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
				// vlozi ho od leveho horniho rohu
				insert_MO_(it->o, it->w, it->h,
					column - it->w + 1, field - it->h + 1);
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
	// nestvury
	if(it_first==end_it) return;
	isCurType.clear().addType(CREATURE);

	while(isCurType(*it_second) && ++it_second!=end_it);
	generate_creatures_(base, it_first, it_second);

	it_first= it_second;
	// bonusy
	if(it_first==end_it) return;
	isCurType.clear().addType(BONUS);

	while(isCurType(*it_second) && ++it_second!=end_it);
	generate_bonuses_(it_first, it_second);

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
	// konstrukce zarizujici maximalni pocet obrazku za sekundu
// 	Uint8 fps=10;
// 	Uint32 fps_last= 0;

	int last_time = SDL_GetTicks(),
		time_to_use = 0, this_time=last_time;
	bool at_end=false;
	set_players_view_(window);
	// iterace dokud neni vyvolano zavreni okna
	SDLKey key;
	while(true){
		switch(get_event(key)){
			case SDL_VIDEORESIZE:
				set_players_view_(window);
				break;
			case SDL_QUIT:
				AG_Quit();
				return;
			case SDL_KEYUP:
				if(key==SDLK_ESCAPE)
					return;
			default:
				break;
		}

		SDL_PumpEvents();// obnoveni stavu klavesnice

		// vykresleni scen pro jednotlive hrace
		draw_(window);
		// cekani - chceme maximalni pocet obrazku za sekundu
// 		fps_last= SDL_fps(fps_last, fps);

		this_time = SDL_GetTicks();
		time_to_use += (this_time - last_time)%(100*CONFIG->move_period());
		while(time_to_use > MOVE_PERIOD){
			time_to_use -= CONFIG->move_period();
			// hýbne světem
			if(move_()){
				if(!players_.size()){
					return;
				} else {
					set_players_view_(window);
				}
			}
			// posune animace
			update_();
			// pokud jsme na konci, vycka zbyvajici dobu
			if(at_end && --remaining_periods_==0)
				return;
		}
		last_time = this_time;

		// zbyva nejaky protivnik?
		if((!remaining_creatures_ && !deathmatch_)
		|| (players_.size()<=1 && deathmatch_) )
			at_end=true;

	}
}

/** @details
 * Podle nastavení v configu a počtu hráčů
 * rozdělí hrací plochu do několika pohledů.
 * @param window surface okna, které rozděluji
 */
void Game::set_players_view_(SDL_Surface* window){
	clear_surface(Color::black, window);
	switch(CONFIG->split_screen() ? players_.size() : 1){
		case 0: break;
		case 1:
			set_players_view_1_(window);
			break;
		case 2:
			set_players_view_2_(window);
			break;
		case 3:
		case 4:
			set_players_view_4_(window);
			break;
	}
}

/** @details
 * Jeden pohled společný pro všechny hráče.
 * Typicky přes celé okno, velikost je však omezena.
 * @param window surface okna, které rozděluji
 */
void Game::set_players_view_1_(SDL_Surface * window){
	Uint16 win_w = window->w, win_h = window->h,
		map_w = CELL_SIZE*
			min( GAME_PLAYER_VIEW_MAX_WIDTH, map_width() ),
		map_h = CELL_SIZE*
			min( GAME_PLAYER_VIEW_MAX_HEIGHT, map_height() );
	SDL_Rect & win_view = players_.begin()->second.win_view;
	win_view.w = min(win_w, map_w);
	win_view.h = min(win_h, map_h);
	win_view.x = map_w < win_w ? (win_w-map_w)/2 : 0;
	win_view.y = map_h < win_h ? (win_h-map_h)/2 : 0;
}

/** @details
 * Dva pohledy pro dva hráče.
 * Řeší zda rozdělovat vodorovně či svisle,
 * kdo kde bude.
 * Každý musí mít stejně velký prostor.
 * @param window surface okna, které rozděluji
 */
void Game::set_players_view_2_(SDL_Surface * window){
	Uint16 half_w = window->w/2, half_h = window->h/2,
		win_w = window->w, win_h = window->h,
		map_w = CELL_SIZE*
			min( GAME_PLAYER_VIEW_MAX_WIDTH, map_width() ),
		map_h = CELL_SIZE*
			min( GAME_PLAYER_VIEW_MAX_HEIGHT, map_height() );
	players_it it, it2;
	it = it2 = players_.begin();
	// rozdelit svisle
	if(win_h < win_w){
		// kdo bude vlevo
		if(it->first==1)
			++it;
		else
			++it2;
		it->second.win_view.x = it2->second.win_view.x =
			map_w < half_w ? (half_w-map_w)/2 : 0;
		it2->second.win_view.x += half_w+1;
		it->second.win_view.y = it2->second.win_view.y =
			map_h < win_h ? (win_h-map_h)/2 : 0;

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
		it->second.win_view.x = it2->second.win_view.x =
			map_w < win_w ? (win_w-map_w)/2 : 0;
		it->second.win_view.y = it2->second.win_view.y =
			map_h < half_h ? (half_h-map_h)/2 : 0;
		it2->second.win_view.y += half_h+1;

		it->second.win_view.w = it2->second.win_view.w =
			min( win_w, map_w);
		it->second.win_view.h = it2->second.win_view.h =
			min( static_cast<Uint16>(half_h-1), map_h);
	}
}

/** @details
 * Čtyři pohledy pro tři nebo čtyři hráče.
 * Vždy rozdělí na čtvrtiny(dva sloupce, dva řádky),
 * je předem jasně dáno, kdo kde bude.
 * Každý má stejně velký prostor.
 * @param window surface okna, které rozděluji
 */
void Game::set_players_view_4_(SDL_Surface * window){
	Uint16 half_w = window->w/2, half_h = window->h/2,
		map_w = CELL_SIZE*
			min( GAME_PLAYER_VIEW_MAX_WIDTH, map_width() ),
		map_h = CELL_SIZE*
			min( GAME_PLAYER_VIEW_MAX_HEIGHT, map_height() );
	for(players_it it = players_.begin() ; it!=players_.end() ; ++it){
		it->second.win_view.w =
				min( static_cast<Uint16>(half_w-1), map_w);
		it->second.win_view.h =
				min( static_cast<Uint16>(half_h-1), map_h);
		it->second.win_view.x = map_w < half_w ? (half_w-map_w)/2 : 0;
		it->second.win_view.y = map_h < half_h ? (half_h-map_h)/2 : 0;
		switch(it->first){
			case 0:
				break;
			case 1:
				it->second.win_view.x += half_w+1;
				it->second.win_view.y += half_h+1;
				break;
			case 2:
				it->second.win_view.y += half_h+1;
				break;
			case 3:
				it->second.win_view.x += half_w+1;
				break;
		}
	}
}

/** @details
 * Pro jednu souřadnici (x nebo y) počítá,
 * o kolik je třeba posunout mapu, aby byl hřáč uprostřed
 * a zároveň byl dostatečně využit pohled (roh mapy nesmí být uvnitř pohledu).
 * @param player_coor souřadnice hráče
 * @param rect_half_size polovina rozměru pohledu
 * @param map_size rozměr mapy
 * @return Posun mapy v pixelech.
 */
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

/** @details
 * Vykreslí pohledy a panely hráčů.
 * @param window surface, do kterého máme kreslit
 */
void Game::draw_(SDL_Surface* window){
	if(players_.empty()) return;

	SDL_Rect * p_view = 0;
	if(!CONFIG->split_screen()){
		// p_view je jeden nemenny
		p_view = &players_.begin()->second.win_view;
		SDL_SetClipRect(window, p_view);
		// vykresluji pouze jeden pohled pro vsechny hrace
		draw_one_view_(window);
	}
	for(players_it player_it = players_.begin() ;
			player_it!=players_.end() ; ++player_it){
		if(CONFIG->split_screen()){
			// p_view je pro kazdeho hrace jiny
			p_view = &player_it->second.win_view;
			SDL_SetClipRect(window, p_view);
			// vykresluji pro kazdeho hrace vlastni pohled
			draw_players_view_(window, player_it->first);
		}
		// ovsem do pohledu (at spolecneho nebo vlastniho)
		// vykresluji kazdemu hraci jeho panel
		player_it->second.player->draw_panel(window, *p_view);
	}
	SDL_SetClipRect(window, 0);
	// zobrazeni na obrazovku
	SDL_Flip(window);
}

/** @details
 * Vykreslí pohled určitého hráče.
 * Zjišťuje posunutí mapy vůči pohledu,
 * vykresluje prvně vše v pozadí, pak vše v popředí.
 * @param window surface, do kterého máme kreslit
 * @param player_num číslo hráče, pro kterého máme vykreslit pohled
 */
void Game::draw_players_view_(SDL_Surface* window, Uint16 player_num){
	player_t & game_player = players_[player_num];
	// obnovim map_view
	Sint16 i,
		shift_x = count_rect_shift_(game_player.player->getX(),
			game_player.win_view.w/2, map_width()*CELL_SIZE),
		shift_y = count_rect_shift_(game_player.player->getY(),
			game_player.win_view.h/2, map_height()*CELL_SIZE);

	shift_x += game_player.shaker.getDiffX();
	shift_y += game_player.shaker.getDiffY();

	game_player.map_view.x=
		game_player.win_view.x - shift_x;
	game_player.map_view.y=
		game_player.win_view.y - shift_y;
	// vykreslim pozadi a pak prisery
	for(i=0 ; i<2 ; ++i){
		draw_map_(i==0, window, game_player.map_view,
			shift_x/CELL_SIZE, shift_y/CELL_SIZE,
			(shift_x+game_player.win_view.w)/CELL_SIZE,
			(shift_y+game_player.win_view.h)/CELL_SIZE );
	}
}

/** @details
 * Jako střed vykreslování vezme aritmetický průměr pozic hráčů.
 * Zjišťuje posunutí mapy vůči pohledu,
 * vykresluje prvně vše v pozadí, pak vše v popředí.
 * @param window surface, do kterého máme kreslit
 */
void Game::draw_one_view_(SDL_Surface* window){
	// obnovim map_view
	Sint16 i, shift_x=0, shift_y=0;

	for(players_it it = players_.begin() ; it!=players_.end() ; ++it){
		shift_x += it->second.player->getX();
		shift_y += it->second.player->getY();
	}
	player_t & game_player = players_.begin()->second;
	shift_x = count_rect_shift_(shift_x / players_.size(),
			game_player.win_view.w/2, map_width()*CELL_SIZE),
	shift_y = count_rect_shift_(shift_y / players_.size(),
			game_player.win_view.h/2, map_height()*CELL_SIZE);

	shift_x += game_player.shaker.getDiffX();
	shift_y += game_player.shaker.getDiffY();


	game_player.map_view.x= game_player.win_view.x - shift_x;
	game_player.map_view.y= game_player.win_view.y - shift_y;
	// vykreslim pozadi a pak prisery
	for(i=0 ; i<2 ; ++i){
		draw_map_(i==0, window, game_player.map_view,
			shift_x/CELL_SIZE, shift_y/CELL_SIZE,
			(shift_x+game_player.win_view.w)/CELL_SIZE,
			(shift_y+game_player.win_view.h)/CELL_SIZE );
	}
}

/** @details
 * Vykreslí část mapy v pohledu.
 * @param window surface okna pro vykreslení
 * @param map_view pohled
 * @param from_x x-ová souřadnice počátku vykreslování
 * @param from_y y-ová souřadnice počátku vykreslování
 * @param to_x x-ová souřadnice konce vykreslování
 * @param to_y y-ová souřadnice konce vykreslování
 */
void Game::draw_map_(bool bg, SDL_Surface* window, SDL_Rect & map_view,
			Uint16 from_x, Uint16 from_y, Uint16 to_x, Uint16 to_y){

	Uint16 column, field;
	// poprve projdu mapu a vykreslim pozadi a objekty na pozadi
	// objekty na policku seradim
	for(field = from_y ; field<map_array_[0].size() ; ++field){
		map_view.h = (field>to_y ? map_array_[0].size() : field+1) *CELL_SIZE;
		for(column= from_x ; column< map_array_.size() ; ++column){
			map_view.w = (column>to_x ? map_array_.size() : column+1) *CELL_SIZE;
			if(bg){
				draw_map_field_bg_(window, map_view, column, field);
			}
			else {
				draw_map_field_fg_(window, map_view, column, field);
			}
			if(column > to_x) break;
		}
		if(field > to_y) break;
	}
}

/** @details
 * Vykreslí pozadí (background, floorobject) jednoho políčka mapy.
 * @param window surface okna pro vykreslení
 * @param map_view pohled
 * @param column x-ová souřadnice políčka mapy
 * @param field y-ová souřadnice políčka mapy
 */
void Game::draw_map_field_bg_(SDL_Surface* window, SDL_Rect & map_view,
			Uint16 column, Uint16 field){
	map_array_t::value_type::value_type::iterator it, end_it;
	it = map_array_[column][field].begin();
	end_it = map_array_[column][field].end();
	// vykreslim pozadi a objekt na zemi
	while( (it=find_if(it, end_it, isTypeOf::isBgType)) !=end_it){
		(*it)->draw(window, map_view);
		++it;
	}
}

/** @details
 * Seřadí objekty na políčku.
 * Vykreslí objekty v popředí jednoho políčka mapy.
 * @param window surface okna pro vykreslení
 * @param map_view pohled
 * @param column x-ová souřadnice políčka mapy
 * @param field y-ová souřadnice políčka mapy
 */
void Game::draw_map_field_fg_(SDL_Surface* window, SDL_Rect & map_view,
			Uint16 column, Uint16 field){
	// objekty na policku seradim
	map_array_[column][field].sort(isUnder);

	map_array_t::value_type::value_type::iterator it, end_it;
	// vykreslim ty co nejsou pozadi
	for(it = map_array_[column][field].begin(), end_it = map_array_[column][field].end() ;
			it != end_it ; ++it){

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

void Game::shake_views(){
	for(players_it it = players_.begin() ; it!=players_.end() ; ++it){
		it->second.shaker.shake(2*tools->getFlamePeriod()/MOVE_PERIOD);
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
	// TODO bombsatend_
	return removed_player;
}

/** @details
 * Projde všechny dynamické i statické objekty hry a zavolá na nich fci update().
 * Obnoví i zatřasení pohledy.
 * @see MapObject::update(), GameShaker::update()
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

	// obnovit shaker pro hrace
	for(players_it it = players_.begin() ; it!=players_.end() ; ++it){
		it->second.shaker.update();
	}
}

/// Info o ukončení hry.
bool Game::success() const{
	if(deathmatch_)
		return players_.size()==1;
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

/** @details
 * Užitečné pro deathmatch.
 * Nutno pro jistotu správnosti výsledku použít success().
 * @return Vrací číslo hráče, který je ještě naživu
 * nebo nulu pokud není na živu nikdo.
 * @warning Vrací také nulu, pokud je naživu první hráč,
 * nutno ověřit žijícího hráče pomocí success().
 * @see success()
 */
Uint16 Game::winner(){
	if(players_.empty())
		return 0;
	return players_.begin()->first;
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

/** @details
 * Všem ostatním hráčům přidá bonus.
 * @param player_num číslo hráče který bonus nedostane
 * @param bonus_name jméno bonusu
 */
void Game::add_others_bonus(Uint16 player_num, const char * bonus_name){
	for(players_it it = players_.begin() ; it!=players_.end() ; ++it){
		if(it->first!=player_num)
			BonusApplication::new_bonus(
				bonus_name, it->second.player);
	}
}


Uint16 Game::map_height() const {
	return map_array_[0].size();
}

Uint16 Game::map_width() const {
	return map_array_.size();
}

/*************** END OF class Game ******************************/
