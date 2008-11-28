/** @file menu_deathmatch_maps.h
 * Soubor obsahuje třídu obsluhující menu map deathmatche.
 */

#ifndef MENU_DEATHMATCH_MAPS_H
#define MENU_DEATHMATCH_MAPS_H

#include <string>
#include "agar_helper.h"
#include "menu_base.h"

#define MENU_DEATHMATCH_MAPS_BUFFSIZE 256

class MenuDeathmatchMaps: public MenuBase {
	friend class MenuDeathmatch;
	public:
		MenuDeathmatchMaps();
		virtual ~MenuDeathmatchMaps() {};
		static void create(AG_Event * ev=0)
			{ new MenuDeathmatchMaps; }
	private:
		static void handlerCheckMap(AG_Event * event);
		static bool setMap(const std::string & map);
		static char map_name[];
		static char map_path[];

		AG_FileDlg * file_dlg_;
};

#endif
