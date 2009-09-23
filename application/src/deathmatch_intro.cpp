
#include <string>
#include "agar_helper.h"
#include "SDL_lib.h"
#include "constants.h"
#include "tixml_helper.h"
#include "stl_helper.h"
#include "deathmatch_intro.h"
#include "game_base.h"
#include "game_tools.h"
#include "game.h"

using namespace std;


DeathmatchIntro::DeathmatchIntro(Uint16 players_count,
		const std::string & mapname, const bonuses_t & bonuses,
		Uint16 wins, bool creatures, bool bombsatend):
	gameBase_( new GameBase(players_count, mapname, bonuses, creatures) ),
	gameTools_(new GameTools), game_(0),
	wins_(wins), bombsatend_(bombsatend) {

	// pripravi prostor pro skore hracu
	score_.insert(score_.end(), players_count, 0);

	load_deathmatch_tools_(players_count);
}

DeathmatchIntro::~DeathmatchIntro(){
	if(gameBase_)
		delete gameBase_;
	if(gameTools_)
		delete gameTools_;
	if(game_)
		delete game_;
}

void DeathmatchIntro::load_deathmatch_tools_(Uint16 players_count){
	TiXmlDocument doc;
	TiXmlElement *root_el;
	string filename("deathmatchtools"), name;
	// nacteni XML souboru
	root_el = TiXmlRootElement(doc, filename, "deathmatchtools", false);
	try{
		readAttr(root_el, "intro", name);
		intro_.push_back(name);
		readAttr(root_el, "intro_draw", name);
		intro_.push_back(name);
		readAttr(root_el, "intro_win", name);
		intro_.push_back(name);

		// nacteni zdrojoveho surface
		Surface sur_src= load_src_surface(root_el);

		load_tool_(root_el, "cup", sur_src);
		for(Uint16 i=0 ; i<players_count ; ++i){
			name = "player"+x2string(i);
			load_tool_(root_el, name.c_str(), sur_src);
			name+= "_burning";
			load_tool_(root_el, name.c_str(), sur_src);
		}
	}
	catch(const string & err){
		TiXmlError(filename, err);
	}
}

void DeathmatchIntro::load_tool_(TiXmlElement * rootEl,
			const char* subEl_name, const Surface & sur_src){
	TiXmlElement * subEl = subElement(rootEl, subEl_name);
	Uint16 width, height;
	readAttr(subEl, "width", width);
	readAttr(subEl, "height", height);

	Animation anim(subEl, width, height, sur_src, sur_src);
	tools_.push_back(anim);
}


extern SDL_Surface *g_window;

/** @details
 * Ukáže úvodní obrazovku matche, vygeneruje konkrétní hru,
 * čeká na klávesu, spustí hru. Po skončení upraví skóre a hraje se dál.
 */
void DeathmatchIntro::show_screen(){
	bool at_end = false;
	Uint16 winner;
	while(!at_end){
		// vygenerovani nove hry z pripraveneho zakladu
		if(game_) delete game_;
		game_ = new Game(*gameBase_, gameTools_, true);

		// hrajeme
		game_->play(g_window);
		// upravime skore
		if(game_->success()){
			winner = game_->winner();
			if(++score_[winner]==wins_)
				at_end = true;
		} else {
			winner = score_.size();
		}

		delete game_;
		game_ = 0;
		// zobrazit skore
		if(!show_score_(winner)){
			return;
		}
	}

	if(game_){
		delete game_;
	}
	game_ = 0;
}

bool DeathmatchIntro::show_score_(Uint16 winner){
	Surface bg;
	if(winner>=score_.size()) // mezivysledek(remiza)
		bg = get_image_(1);
	else if(score_[winner]==wins_) // konec(vitez)
		bg = get_image_(2);
	else
		bg = get_image_(0); // mezivysledek(vitez)
	// postupne zobrazeni
	Uint32 fps_last= 0;
	Uint16 trans_diff = SDL_ALPHA_OPAQUE
		/ DEATHMATCH_INTRO_SCORE_FPS / DEATHMATCH_INTRO_SCORE_LENGTH;
	Uint8 trans = winner>=score_.size()
		? SDL_ALPHA_OPAQUE : SDL_ALPHA_TRANSPARENT;
	clear_surface(Color::black, g_window);
	while(true){
		draw_score_(winner, bg, trans);
		fps_last= SDL_fps(fps_last, DEATHMATCH_INTRO_SCORE_FPS);
		if(trans<=SDL_ALPHA_OPAQUE-trans_diff)
			trans+=trans_diff;
		else break;
	}
	// pockame na klavesu, pri pokusu o ukonceni ukoncime
	SDLKey key;
	while(true){
		switch(wait_event(key)){
			case SDL_VIDEORESIZE:
				// uvodni obrazovka levelu
				clear_surface(Color::black, g_window);
				draw_score_(winner, bg, SDL_ALPHA_OPAQUE);
				continue;
			case SDL_QUIT:
				AG_Quit();
				return false;
			case SDL_KEYUP:
				if(key==SDLK_ESCAPE)
					return false;
				if(key==SDLK_RETURN || key==SDLK_KP_ENTER
				|| key==SDLK_SPACE || key==SDLK_RCTRL)
					break;
			default:
				continue;
		}
		return true;
	}
}

void DeathmatchIntro::draw_score_(
			Uint16 winner, Surface & bg, Uint8 trans){
	Sint16
		base_x = ( g_window->w - bg.width() )/2
			+DEATHMATCH_INTRO_SCORE_PADDING,
		base_y = ( g_window->h - bg.height() )/2
			+DEATHMATCH_INTRO_SCORE_PADDING,
		player_h = ( bg.height()-2*DEATHMATCH_INTRO_SCORE_PADDING )
			/score_.size();
	draw_surface(base_x-DEATHMATCH_INTRO_SCORE_PADDING,
		base_y-DEATHMATCH_INTRO_SCORE_PADDING,
		bg.getSurface(), g_window );
	for(Uint16 player = 0 ; player<score_.size() ; ++player){
		draw_player_score_(base_x, base_y+player*player_h, player_h,
			player, winner, trans);
	}
	SDL_Flip(g_window);
}

void DeathmatchIntro::draw_player_score_( Sint16 x, Sint16 y, Uint16 h,
			Uint16 player, Uint16 winner, Uint8 trans){
	Uint16 i, x_diff = tools_[0].width()+DEATHMATCH_INTRO_SCORE_PADDING;
	// obrazek hrace
	// spaleny kdyz neni remiza & vitez ukoncuje match & nezobrazujeme viteze
	if(winner<score_.size() && score_[winner]==wins_ && winner!=player)
		i = player*2+2;
	else // jinak normalni
		i = player*2+1;
	tools_[i].draw(g_window, x, y+ abs_minus(h, tools_[i].height())/2 );
	// pohary
	for(i = 1; i <= score_[player] ; ++i){
		x += x_diff;
		if(player==winner && i==score_[player])
			tools_[0].set_transparency(trans);

		tools_[0].draw(g_window, x,
			y +abs_minus(h, tools_[0].height())/2);

		if(player==winner && i==score_[player])
			tools_[0].set_transparency(SDL_ALPHA_OPAQUE);
	}
}

Surface & DeathmatchIntro::get_image_(Uint8 index){
	std::string & filename = intro_[index];
	if(filename==image_.first)
		return image_.second;

	if(!locate_file("", filename, filename))
		TiXmlError("levels",
			"Unable to load "+filename+": no such file found");
	image_.first = filename;
	// odalokovat
	image_.second = 0;
	// naalokovat novy
	image_.second = IMG_Load(filename.c_str());
	if(!image_.second.getSurface())
		TiXmlError("levels", "Unable to load "+filename);
	return image_.second;
}
