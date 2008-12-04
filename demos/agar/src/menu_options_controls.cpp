
#include <string>
#include "stl_helper.h"
#include "menu_options_controls.h"
#include "config.h"

#ifndef CONFIG
#define CONFIG Config::get_instance()
#endif

extern SDL_Surface * g_window;

MenuOptionsControls::MenuOptionsControls(){
	background_ = create_surface(
		g_window->w, g_window->h, Color::black);
	set_transparency(background_.getSurface(), 128);

	AG_Box *item, *box;
	AG_Label *label;
	AG_Notebook * notebook;
	AG_NotebookTab * tab;
	std::string tab_name;
	const char *action_name, *key_name;
	// nadpis
	createHeading("Controls");

	// notebook, pro kazdeho hrace jedna zalozka
	notebook = AG_NotebookNew(win, AG_NOTEBOOK_EXPAND);
	setFocusOnMotion( AGWIDGET(notebook) );
	AG_SetEvent(notebook, "window-keydown", handlerKeyBack, 0);
	for(Uint16 player_num = 0 ; player_num<CONFIG->players_.size() ;
							++player_num){
		// TODO lang
		tab_name = "player"+x2string(player_num);
		tab = AG_NotebookAddTab(notebook, tab_name.c_str(), AG_BOX_VERT);

		for(Uint16 action_num=0 ;
				action_num<CONFIG->players_[player_num].size() ;
				++action_num){
			action_name = CONFIG->action2name_(
					static_cast<KEY_ACTIONS>(action_num) );
			key_name = CONFIG->keyNames_.key2name(
					CONFIG->players_[player_num][action_num]);

			item = AG_BoxNewVert(tab, AG_BOX_FRAME | AG_BOX_HFILL);
			setFocusOnMotion( AGWIDGET(item) );

			box = AG_BoxNewHoriz(item, AG_BOX_HOMOGENOUS | AG_BOX_HFILL);
			AG_BoxSetPadding(box, 0);
			AG_LabelNewString(box, 0, action_name);
			label = AG_LabelNewString(box, 0, key_name);

			AG_SetEvent(item, "window-mousebuttondown",
				handlerControls, "%[u16],%[u16],%p,%p",
				player_num, action_num, label, background_.getSurface());
			AG_SetEvent(item, "window-keydown", handlerKeyBack, 0);
		}

		AG_SpacerNewHoriz(tab);

		item = AG_BoxNewVert(tab, AG_BOX_FRAME | AG_BOX_HFILL);
		setFocusOnMotion( AGWIDGET(item) );
		AG_SetEvent(item, "window-mousebuttondown", handlerBack, 0);
		AG_SetEvent(item, "window-keydown", handlerKeyBack, 0);

		label = createLabelJustify(AGWIDGET(item), "OK");
	}

	AG_SpacerNewHoriz(win);
}

void MenuOptionsControls::handlerControls(AG_Event * event){
	Uint16 player_num = AG_UINT16(1);
	Uint16 action_num = AG_UINT16(2);
	AG_Label * label = static_cast<AG_Label *>(AG_PTR(3));

	SDL_Surface * sur = static_cast<SDL_Surface *>(AG_PTR(4));
	draw_surface(0, 0, sur, g_window);
	SDL_Flip(g_window);

	SDLKey key = wait_event();

	if(key==SDLK_ESCAPE) return;
	// nastavim v configu
	CONFIG->players_[player_num][action_num] = key;
	// oznamim zmenu
	CONFIG->players_changed_ = true;
	// zmenim popisek
	AG_LabelString(label, CONFIG->keyNames_.key2name(key));
}

void MenuOptionsControls::handlerKeyBack(AG_Event * event){
	if(AG_SDLKEY(1)==SDLK_ESCAPE) handlerBack();
}
