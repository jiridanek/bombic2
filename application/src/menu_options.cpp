
#include "menu_options.h"
#include "menu_options_controls.h"
#include "config.h"
#include "language.h"

MenuOptions::MenuOptions():
			speed_(CONFIG->speed()),
			visible_presumption_(CONFIG->visible_presumption()),
			split_screen_(CONFIG->split_screen()),
			fullscreen_(CONFIG->fullscreen()),
			sound_(CONFIG->sound()){
	AG_Box * item;
	AG_UCombo *combo;
// 	AG_TlistItem * combo_item;
	AG_Checkbox * checkbox;

	// nadpis
	createHeading(LANG_MENU(LANG_OPTIONS, LANG_HEADING));

	// jazyk
	item = createItem(LANG_MENU(LANG_OPTIONS, LANG_LANGUAGE));
	combo = AG_UComboNew(item, AG_UCOMBO_HFILL);
	AG_SetEvent(combo, "ucombo-selected", handlerLanguage, 0);
	for(Uint16 i=0 ; i<CONFIG->languages_.size() ; ++i){
		AG_TlistAddPtr(combo->list, 0,
			CONFIG->languages_[i].show.c_str(), &CONFIG->languages_[i]);

	}

	// klavesy
	item = createItem(LANG_MENU(LANG_OPTIONS, LANG_CONTROLS));
	AG_SetEvent(item, "window-mousebuttondown",
			MenuOptionsControls::create, 0);

	// rychlost hry
	item = createItemHoriz(LANG_MENU(LANG_OPTIONS, LANG_SPEED));
	AG_AddEvent(items_.back(), "window-keyup", handlerIntItem,
		"%p,%i,%i", &speed_,
		CONFIG_SPEED_MIN, CONFIG_SPEED_MAX);

	AG_NumericalNewIntR(item, 0, 0, 0, &speed_,
		CONFIG_SPEED_MIN, CONFIG_SPEED_MAX);

	// viditelna presumpce
	item = createItemHoriz(LANG_MENU(LANG_OPTIONS, LANG_PRESUMPTIONS));
	AG_AddEvent(items_.back(), "window-mousebuttondown", handlerBoolItem,
		"%p", &visible_presumption_);

	item = AG_BoxNewHoriz(item, AG_BOX_HOMOGENOUS | AG_BOX_HFILL);
	AG_BoxSetPadding(item, 0);
	AG_SpacerNewVert(item);
	AG_CheckboxNewInt(item, 0, "  ", &visible_presumption_);

	// rozedelni obrazovek
	item = createItemHoriz(LANG_MENU(LANG_OPTIONS, LANG_SPLIT_SCREEN));
	AG_AddEvent(items_.back(), "window-mousebuttondown", handlerBoolItem,
		"%p", &split_screen_);

	item = AG_BoxNewHoriz(item, AG_BOX_HOMOGENOUS | AG_BOX_HFILL);
	AG_BoxSetPadding(item, 0);
	AG_SpacerNewVert(item);
	AG_CheckboxNewInt(item, 0, "  ", &split_screen_);

	// fullscreen
	item = createItemHoriz(LANG_MENU(LANG_OPTIONS, LANG_FULLSCREEN));
	AG_AddEvent(items_.back(), "window-mousebuttondown", handlerBoolItem,
		"%p", &fullscreen_);
	AG_AddEvent(items_.back(), "window-mousebuttondown", handlerToggleFullscreen, 0);

	item = AG_BoxNewHoriz(item, AG_BOX_HOMOGENOUS | AG_BOX_HFILL);
	AG_BoxSetPadding(item, 0);
	AG_SpacerNewVert(item);
	checkbox = AG_CheckboxNewInt(item, 0, "  ", &fullscreen_);
	AG_AddEvent(checkbox, "window-mousebuttondown", handlerToggleFullscreen, 0);

	// zvuk
	item = createItemHoriz(LANG_MENU(LANG_OPTIONS, LANG_SOUND));
	AG_AddEvent(items_.back(), "window-mousebuttondown", handlerBoolItem,
		"%p", &sound_);

	item = AG_BoxNewHoriz(item, AG_BOX_HOMOGENOUS | AG_BOX_HFILL);
	AG_BoxSetPadding(item, 0);
	AG_SpacerNewVert(item);
	AG_CheckboxNewInt(item, 0, "  ", &sound_);


	// back
	item = createItem(LANG_MENU(LANG_OPTIONS, LANG_SAVE));
	AG_SetEvent(item, "window-mousebuttondown", handlerBack, 0);

	AG_SpacerNewHoriz(win);
}

MenuOptions::~MenuOptions() {
	CONFIG->speed_ = static_cast<Uint16>(speed_);
	CONFIG->visible_presumption_ = visible_presumption_!=0;
	CONFIG->split_screen_ = split_screen_!=0;
	CONFIG->fullscreen_ = fullscreen_!=0;
	CONFIG->sound_ = sound_!=0;
	CONFIG->save_configuration_();
}

#include <iostream>
void MenuOptions::handlerLanguage(AG_Event * event){
	AG_TlistItem *li = static_cast<AG_TlistItem *>(AG_PTR(1));
	Config::language_t * lang = static_cast<Config::language_t *>(li->p1);
	std::cout << lang->name << std::endl;
}

extern SDL_Surface * g_window;
void MenuOptions::handlerToggleFullscreen(AG_Event * ev){
	SDL_WM_ToggleFullScreen(g_window);
}
