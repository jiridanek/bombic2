/** @file deathmatch_maps.h
 * Soubor obsahuje třídu obsluhující menu map deathmatche.
 */

#ifndef MENU_DEATHMATCH_MAPS_H
#define MENU_DEATHMATCH_MAPS_H

#include <string>
#include <vector>
#include <helpers/agar.h>
#include "base.h"

#define MENU_DEATHMATCH_MAPS_DIR "maps"
#define MENU_DEATHMATCH_MAPS_BUFFSIZE 250
#define MENU_DEATHMATCH_MAX_PATH_LENGTH 1000

class MenuDeathmatchMaps: public MenuBase {
	friend class MenuDeathmatch;
	public:
		MenuDeathmatchMaps();
		virtual ~MenuDeathmatchMaps() {};
		static void create(AG_Event * ev=0)
			{ new MenuDeathmatchMaps; }
	private:
		void createDirButton(const char * label);
		std::vector<std::string> buttons_;

		static void handlerDirButton(AG_Event * event);
		static void handlerCheckMap(AG_Event * event);
		static bool setMap(const std::string & map);
		static char map_name[];
		static std::string map_path;
		static AG_FileDlg * file_dlg;
};

#endif
