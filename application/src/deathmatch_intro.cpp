
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
	Uint8 image;
	while(!at_end){
		// vygenerovani nove hry z pripraveneho zakladu
		if(game_) delete game_;
		game_ = new Game(*gameBase_, gameTools_, true);

		// hrajeme
		game_->play(g_window);
		// upravime skore
		if(game_->success()){
			if(++score_[game_->winner()]==wins_){
				at_end = true;
				image = 2;
			}
			else image = 0;
		}
		else image = 1;

		delete game_;
		game_ = 0;

		// uvodni obrazovka matche
		clear_surface(Color::black, g_window);
		draw_center_surface(get_image_(image).getSurface(), g_window);
		SDL_Flip(g_window);

		// pockame na klavesu, pri pokusu o ukonceni ukoncime
		SDLKey key;
		while(true){
			switch(wait_event(key)){
				case SDL_VIDEORESIZE:
					// uvodni obrazovka levelu
					clear_surface(Color::black, g_window);
					draw_center_surface(get_image_(image).getSurface(), g_window);
					SDL_Flip(g_window);
					continue;
				case SDL_QUIT:
					AG_Quit();
					return;
				case SDL_KEYUP:
					if(key==SDLK_ESCAPE)
						return;
				default:
					break;
			}
			break;
		}
	}

	if(game_)
		delete game_;
	game_ = 0;
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
