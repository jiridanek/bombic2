
#include <string>
#include "menu_deathmatch_bonuses.h"
#include "tixml_helper.h"
#include "game_base.h"
#include "constants.h"

DeathmatchIntro::bonuses_t MenuDeathmatchBonuses::bonuses;

MenuDeathmatchBonuses::MenuDeathmatchBonuses(){
	AG_Box * item;
	AG_Pane * pane;

	// nadpis
	createHeading("Deathmatch bonuses setings");

	// bonuses
	if(bonuses.empty())
		load_bonuses_();

	pane = AG_PaneNewHoriz(win, AG_PANE_FORCE_DIV | AG_PANE_HFILL);
	AG_BoxSetPadding(pane->div[0], 1);
	AG_BoxSetPadding(pane->div[1], 1);
	AG_BoxSetSpacing(pane->div[0], 0);
	AG_BoxSetSpacing(pane->div[1], 0);

	for(int i=0 ; static_cast<Uint16>(i)<bonuses.size() ; ++i){
		// bonus
		createBonusItem(pane->div[i%2], i);
	}

	// ok
	item = createItem("OK");
	AG_SetEvent(item, "window-mousebuttondown", handlerBack, 0);
	setFocus(AGWIDGET(item));

	AG_SpacerNewHoriz(win);
}

void MenuDeathmatchBonuses::load_bonuses_(){
	try {
		TiXmlDocument doc;
		TiXmlElement *el;
		std::string filename("menu_bonuses");
		DeathmatchIntro::bonuses_t::value_type bonus = { "", 0 };
		try{
			el = TiXmlRootElement(doc, filename, "bonuses", false);
			el = el->FirstChildElement("bonus");
			while(el){
				readAttr(el, "name", bonus.n);
				bonuses.push_back(bonus);
				el = el->NextSiblingElement("bonus");
			}
		}
		catch(const std::string & err){
			TiXmlError(filename, err);
		}
	}
	catch(const TiXmlException & ex){
		AG_TextError("%s", ex.what());
	}
}

void MenuDeathmatchBonuses::createBonusItem(AG_Box * parent, int i){
	AG_Box * item = AG_BoxNewHoriz( parent,
			AG_BOX_FRAME | AG_BOX_HFILL | AG_BOX_HOMOGENOUS);
	// funkce polozky
	items_.push_back(item);

	setFocusOnMotion( AGWIDGET(item) );
	AG_SetEvent(item, "window-keyup", handlerItems, 0);

	AG_AddEvent(item, "window-keyup", handlerIntItem,
		"%p,%i,%i", &bonuses[i].c,
		MENU_DEATHMATCH_BONUSES_MIN, MENU_DEATHMATCH_BONUSES_MAX);
	// obrazek
	createBonusPixmap(item, i);
	// numericke policko
	AG_NumericalNewIntR(item, 0, 0, 0, &bonuses[i].c,
		MENU_DEATHMATCH_BONUSES_MIN, MENU_DEATHMATCH_BONUSES_MAX);

}

void MenuDeathmatchBonuses::createBonusPixmap(AG_Box * parent, int i){
	try {
		TiXmlDocument doc;
		TiXmlElement *el;
		std::string filename(bonuses[i].n);
		try{
			el = TiXmlRootElement(doc, filename, "bonus", true);
			Surface sur;
			GameBaseLoader::load_subEl_surface(
				el, "img", sur, CELL_SIZE, CELL_SIZE,
				GameBaseLoader::load_src_surface(el) );

			AG_PixmapFromSurface(parent, 0,
				AG_SurfaceFromSDL(sur.getSurface()));
		}
		catch(const std::string & err){
			TiXmlError(filename, err);
		}
	}
	catch(const TiXmlException & ex){
		AG_TextError("%s", ex.what());
	}
}

