
#include "menu_options.h"
#include "menu_options_controls.h"
#include "config.h"

MenuOptions::MenuOptions():
			speed_(CONFIG->speed()),
			visible_presumption_(CONFIG->visible_presumption()),
			split_screen_(CONFIG->split_screen()),
			fullscreen_(CONFIG->fullscreen()),
			sound_(CONFIG->sound()){
	AG_Box * item;
	AG_Checkbox * checkbox;

	// nadpis
	createHeading("Options");

	// jazyk
	item = createItem("Language");
// TODO u combo

	// klavesy
	item = createItem("Controls");
	AG_SetEvent(item, "window-mousebuttondown",
			MenuOptionsControls::create, 0);

	// rychlost hry
	item = createItemHoriz("Game speed");
	AG_AddEvent(items_.back(), "window-keyup", handlerIntItem,
		"%p,%i,%i", &speed_,
		CONFIG_SPEED_MIN, CONFIG_SPEED_MAX);

	AG_NumericalNewIntR(item, 0, 0, 0, &speed_,
		CONFIG_SPEED_MIN, CONFIG_SPEED_MAX);

	// viditelna presumpce
	item = createItemHoriz("Explosion presumptions");
	AG_AddEvent(items_.back(), "window-mousebuttondown", handlerBoolItem,
		"%p", &visible_presumption_);

	item = AG_BoxNewHoriz(item, AG_BOX_HOMOGENOUS | AG_BOX_HFILL);
	AG_BoxSetPadding(item, 0);
	AG_SpacerNewVert(item);
	AG_CheckboxNewInt(item, 0, "  ", &visible_presumption_);

	// rozedelni obrazovek
	item = createItemHoriz("Split screen");
	AG_AddEvent(items_.back(), "window-mousebuttondown", handlerBoolItem,
		"%p", &split_screen_);

	item = AG_BoxNewHoriz(item, AG_BOX_HOMOGENOUS | AG_BOX_HFILL);
	AG_BoxSetPadding(item, 0);
	AG_SpacerNewVert(item);
	AG_CheckboxNewInt(item, 0, "  ", &split_screen_);

	// fullscreen
	item = createItemHoriz("Play in fullscreen");
	AG_AddEvent(items_.back(), "window-mousebuttondown", handlerBoolItem,
		"%p", &fullscreen_);
	AG_AddEvent(items_.back(), "window-mousebuttondown", handlerToggleFullscreen, 0);

	item = AG_BoxNewHoriz(item, AG_BOX_HOMOGENOUS | AG_BOX_HFILL);
	AG_BoxSetPadding(item, 0);
	AG_SpacerNewVert(item);
	checkbox = AG_CheckboxNewInt(item, 0, "  ", &fullscreen_);
	AG_AddEvent(checkbox, "window-mousebuttondown", handlerToggleFullscreen, 0);

	// zvuk
	item = createItemHoriz("Sound enabled");
	AG_AddEvent(items_.back(), "window-mousebuttondown", handlerBoolItem,
		"%p", &sound_);

	item = AG_BoxNewHoriz(item, AG_BOX_HOMOGENOUS | AG_BOX_HFILL);
	AG_BoxSetPadding(item, 0);
	AG_SpacerNewVert(item);
	AG_CheckboxNewInt(item, 0, "  ", &sound_);


	// back
	item = createItem("Save");
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

extern SDL_Surface * g_window;

void MenuOptions::handlerToggleFullscreen(AG_Event * ev){
	SDL_WM_ToggleFullScreen(g_window);
}
