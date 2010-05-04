
#include <config.h>
#include <language.h>
#include "options.h"
#include "options_controls.h"

MenuOptions::MenuOptions():
			speed_(CONFIG->speed()),
			visible_presumption_(CONFIG->visible_presumption()),
			split_screen_(CONFIG->split_screen()),
			fullscreen_(CONFIG->fullscreen()),
			sound_(CONFIG->sound()){
	AG_Box * item;
	AG_UCombo *combo;
	AG_TlistItem * combo_item;
	AG_Checkbox * checkbox;
	// nadpis
	createHeading(LANG_MENU(LANG_OPTIONS, LANG_HEADING));

	// jazyk
	item = createItem(LANG_MENU(LANG_OPTIONS, LANG_LANGUAGE));
	combo = AG_UComboNew(item, AG_UCOMBO_HFILL);
	AG_SetEvent(combo, "ucombo-selected", handlerLanguage,
			"%p", combo->button);
	for(Uint16 i=0 ; i<CONFIG->languages_.size() ; ++i){
		combo_item = AG_TlistAddPtr(combo->list, 0,
			CONFIG->languages_[i].show.c_str(), &CONFIG->languages_[i]);
		if(CONFIG->language_==CONFIG->languages_[i].name)
			AG_ButtonText(combo->button, "%s", combo_item->text);
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
	createCheckboxItem(LANG_MENU(LANG_OPTIONS, LANG_PRESUMPTIONS),
		&visible_presumption_);
	// rozedelni obrazovek
	createCheckboxItem(LANG_MENU(LANG_OPTIONS, LANG_SPLIT_SCREEN),
		&split_screen_);

	// fullscreen
	checkbox = createCheckboxItem(LANG_MENU(LANG_OPTIONS, LANG_FULLSCREEN),
		&fullscreen_);
	AG_AddEvent(items_.back(), "window-mousebuttondown", handlerToggleFullscreen, 0);
	AG_AddEvent(checkbox, "window-mousebuttondown", handlerToggleFullscreen, 0);

	// zvuk - neni implementovano
// 	createCheckboxItem(LANG_MENU(LANG_OPTIONS, LANG_SOUND),
// 		&sound_);

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

void MenuOptions::handlerLanguage(AG_Event * event){
	AG_Button *button = static_cast<AG_Button *>(AG_PTR(1));
	AG_TlistItem *li = static_cast<AG_TlistItem *>(AG_PTR(2));
	Config::language_t * lang = static_cast<Config::language_t *>(li->p1);
	try{
		LANG->set_language_(lang->name);
		CONFIG->language_ = lang->name;
		handlerBack();
		create();
	}
	catch(const TiXmlException & ex){
		for(Uint16 i=0 ; i<CONFIG->languages_.size() ; ++i){
			if(CONFIG->language_==CONFIG->languages_[i].name){
				AG_ButtonText(button, "%s",
					CONFIG->languages_[i].show.c_str());
				break;
			}
		}
		AG_TextError("%s\n%s", LANG_MENU(LANG_OPTIONS, LANG_ERR_SET_LANG),
			ex.what());
	}
}

extern SDL_Surface * g_window;
void MenuOptions::handlerToggleFullscreen(AG_Event * ev){
	SDL_WM_ToggleFullScreen(g_window);
}
