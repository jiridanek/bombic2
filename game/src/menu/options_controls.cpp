
#include <string>
#include <string.h>
#include <helpers/stl.h>
#include <config.h>
#include "options_controls.h"

extern SDL_Surface * g_window;
extern Fonts * g_font;

#define MENU_OPTIONS_CONTROLS_EVENT_FMT "%[u16],%[u16],%p,%p,%p,%p"

MenuOptionsControls::MenuOptionsControls(){
	// priprava obrazovky pro cekani na klavesu
	background_ = create_surface(
		g_window->w, g_window->h, Color::black);
	set_transparency(background_.getSurface(), 128);
	background_text_ = get_multiline_text( (*g_font)[20],
		LANG_SUBMENU(LANG_OPTIONS, LANG_CONTROLS, LANG_PRESS_KEY),
		Color::white);
	// prazdne taby
	player_item_t empty_item = {0, 0};
	player_tab_t empty_tab; empty_tab.tab = 0;
	empty_tab.items.insert(empty_tab.items.end(), KEY_ACTIONS_COUNT+1, empty_item);
	player_tabs_.insert(player_tabs_.end(), 4, empty_tab);

	AG_Box *item, *box;
	AG_Label *label;
	AG_Notebook * notebook;
	std::string tab_name;
	const char *action_name, *key_name;
	// nadpis
	createHeading(LANG_SUBMENU(LANG_OPTIONS, LANG_CONTROLS, LANG_HEADING));

	// notebook, pro kazdeho hrace jedna zalozka
	notebook = AG_NotebookNew(win, AG_NOTEBOOK_EXPAND);
	for(Uint16 player_num = 0 ; player_num<CONFIG->players_.size() ;
							++player_num){
		tab_name =
			LANG_SUBMENU(LANG_OPTIONS, LANG_CONTROLS, LANG_PLAYER)
			+ std::string(" ") + x2string(player_num);
		player_tabs_[player_num].tab =
			AG_NotebookAddTab(notebook, tab_name.c_str(), AG_BOX_VERT);

		for(Uint16 action_num=0 ;
				action_num<CONFIG->players_[player_num].size() ;
				++action_num){
			// vytvorit nazvy pro vytisteni
			action_name = LANG_SUBMENU(LANG_OPTIONS, LANG_CONTROLS,
				CONFIG->action2name_(
					static_cast<KEY_ACTIONS>(action_num)) );
			key_name = CONFIG->keyNames_.key2name(
					CONFIG->players_[player_num][action_num]);
			// box
			item = AG_BoxNewVert(player_tabs_[player_num].tab,
				AG_BOX_FRAME | AG_BOX_HFILL);
			setFocusOnMotion( AGWIDGET(item) );

			box = AG_BoxNewHoriz(item, AG_BOX_HOMOGENOUS | AG_BOX_HFILL);
			AG_BoxSetPadding(box, 0);
			AG_LabelNewString(box, 0, action_name);
			// label
			label = AG_LabelNewString(box, 0, key_name);
			// nastavit udalosti na polozku
			AG_SetEvent(item, "window-mousebuttondown",
				handlerControls, MENU_OPTIONS_CONTROLS_EVENT_FMT,
				player_num, action_num, &player_tabs_, notebook,
				background_.getSurface(), background_text_.getSurface());
			AG_SetEvent(item, "window-keyup",
				handlerPlayerItem, MENU_OPTIONS_CONTROLS_EVENT_FMT,
				player_num, action_num, &player_tabs_, notebook,
				background_.getSurface(), background_text_.getSurface());

			// ulozit polozku do seznamu
			player_tabs_[player_num].items[action_num].b = item;
			player_tabs_[player_num].items[action_num].l = label;
		}

		AG_SpacerNewHoriz(player_tabs_[player_num].tab);

		item = AG_BoxNewVert(player_tabs_[player_num].tab,
			 AG_BOX_FRAME | AG_BOX_HFILL);
		setFocusOnMotion( AGWIDGET(item) );
		AG_SetEvent(item, "window-mousebuttondown", handlerBack, 0);
		AG_SetEvent(item, "window-keyup",
			handlerPlayerItem, MENU_OPTIONS_CONTROLS_EVENT_FMT,
			player_num, KEY_ACTIONS_COUNT, &player_tabs_, notebook,
			background_.getSurface(), background_text_.getSurface());

		label = createLabelJustify(AGWIDGET(item),
			LANG_SUBMENU(LANG_OPTIONS, LANG_CONTROLS, LANG_OK));

		// ulozit polozku do seznamu
		player_tabs_[player_num].items.back().b = item;
		player_tabs_[player_num].items.back().l = label;

		// aktivovat prvni polozku
		AG_NotebookSelectTab(notebook, player_tabs_[player_num].tab);
		setFocus(AGWIDGET( player_tabs_[player_num].items[0].b));
	}
	AG_NotebookSelectTab(notebook, player_tabs_[0].tab);
	AG_SpacerNewHoriz(win);
}

void MenuOptionsControls::handlerControls(AG_Event * event){
	Uint16 argi = 0;
	Uint16 player_num = AG_UINT16(++argi);
	KEY_ACTIONS action =
		static_cast<KEY_ACTIONS>(AG_UINT16(++argi));

	player_tabs_t & player_tabs =
		*static_cast<player_tabs_t*>(AG_PTR(++argi));
	// preskoceni notebooku
	++argi;

	SDL_Surface * sur_bg = static_cast<SDL_Surface *>(AG_PTR(++argi));
	SDL_Surface * sur_text = static_cast<SDL_Surface *>(AG_PTR(++argi));
	draw_center_surface(sur_bg, g_window);
	draw_center_surface(sur_text, g_window);
	SDL_Flip(g_window);

	// pockat az se neco stane
	SDLKey key = SDLK_FIRST;
	while(true){
		switch(wait_event(key)){
			case SDL_VIDEORESIZE:
				draw_center_surface(sur_bg, g_window);
				draw_center_surface(sur_text, g_window);
				SDL_Flip(g_window);
				continue;
			case SDL_QUIT:
				AG_Quit();
				return;
			default: break;
		}
		break;
	}
	// prekreslit cele okno
	AG_ResizeDisplay(g_window->w, g_window->h);

	// zruseni akce
	if(key==SDLK_ESCAPE) return;

	Uint16 conflict_pl;
	KEY_ACTIONS conflict_ac;
	if(!CONFIG->set_key_action_(player_num, action,
				key, conflict_pl, conflict_ac) ){
		AG_LabelString(player_tabs[conflict_pl].items[conflict_ac].l,
			CONFIG->keyNames_.key2name(SDLK_FIRST));
	}
	// zmenim popisek
	AG_LabelString(player_tabs[player_num].items[action].l,
		CONFIG->keyNames_.key2name(key));
}

void MenuOptionsControls::handlerPlayerItem(AG_Event * event){
	Uint16 argi = 0;
	Uint16 player_num = AG_UINT16(++argi);
	Uint16 action_num = AG_UINT16(++argi);

	player_tabs_t & player_tabs =
		*static_cast<player_tabs_t*>(AG_PTR(++argi));
	AG_Notebook * notebook =
		static_cast<AG_Notebook*>(AG_PTR(++argi));

	switch(AG_SDLKEY(7)){
		case SDLK_RETURN:
		case SDLK_KP_ENTER:
		case SDLK_SPACE:
			strcpy(event->name, "window-mousebuttondown");
			AG_ForwardEvent(AG_SELF(), AG_SELF(), event);
			return;
		case SDLK_ESCAPE:
			handlerBack();
			return;
		case SDLK_UP:
			if(action_num == 0)
				action_num = KEY_ACTIONS_COUNT;
			else
				--action_num;
			break;
		case SDLK_DOWN:
			if(action_num == KEY_ACTIONS_COUNT)
				action_num = 0;
			else
				++action_num;
			break;
		case SDLK_LEFT:
			if(player_num==0)
				player_num = player_tabs.size()-1;
			else --player_num;
			AG_NotebookSelectTab(notebook, player_tabs[player_num].tab);
			action_num = 0;
			break;
		case SDLK_RIGHT:
			if(player_num==player_tabs.size()-1)
				player_num = 0;
			else ++player_num;
			AG_NotebookSelectTab(notebook, player_tabs[player_num].tab);
			action_num = 0;
			break;
		default:
			return;
	}

	setFocus(AGWIDGET( player_tabs[player_num].items[action_num].b));
}
