
#include <helpers/tixml.h>
#include <language.h>
#include "deathmatch_maps.h"

char MenuDeathmatchMaps::map_name[MENU_DEATHMATCH_MAPS_BUFFSIZE] = "";
std::string MenuDeathmatchMaps::map_path = "";
AG_FileDlg * MenuDeathmatchMaps::file_dlg = 0;

MenuDeathmatchMaps::MenuDeathmatchMaps(){
	AG_WindowMaximize(win);
	AG_WindowSetPadding(win, MENU_OFFSET,
		MENU_OFFSET, MENU_OFFSET, MENU_OFFSET);

	// nadpis
	createHeading(LANG_SUBMENU(LANG_DEATHMATCH, LANG_MAPS, LANG_HEADING));

	// tlacitka pro rychlou volbu
	std::string path;
	const char * paths[] = SEARCH_PATHS;
	for( Uint16 i=0 ; paths[i] != 0 ; ++i ){
		if(locate_dir(paths[i], MENU_DEATHMATCH_MAPS_DIR,
					path, true))
			buttons_.push_back(path);
		else if(is_dir(paths[i]))
			buttons_.push_back(paths[i]);
	}
	if(get_home_path(path)) {
		buttons_.push_back(path);
		if(locate_dir(path, MENU_DEATHMATCH_MAPS_DIR,
					path, true))
			buttons_.push_back(path);
	}
	for( Uint16 i=0 ; i < buttons_.size() ; ++i ){
		createDirButton(buttons_[i].c_str());
		if(map_path.empty())
			map_path = buttons_[i];
	}
	// filemanager
	file_dlg = createFileDlg(true, map_path.c_str(), LANG_DEATHMATCH, LANG_MAPS);
	AG_FileDlgAddType(file_dlg,
		LANG_SUBMENU(LANG_DEATHMATCH, LANG_MAPS, LANG_FILE_TYPE),
		TIXML_FILE_EXTENSION, handlerCheckMap, 0);

	AG_SpacerNewHoriz(win);
}

void MenuDeathmatchMaps::createDirButton(const char * label){
	AG_Button * button = AG_ButtonNew(win, 0, label);
	AG_ExpandHoriz(button);
	AG_ButtonJustify(button, AG_TEXT_LEFT);
	AG_SetEvent(button, "button-pushed", handlerDirButton, "%s", label);
	AG_AddEvent(button, "window-keyup", handlerItems, 0);
}

void MenuDeathmatchMaps::handlerDirButton(AG_Event * event){
	AG_FileDlgSetDirectory(file_dlg, AG_STRING(1));
}

void MenuDeathmatchMaps::handlerCheckMap(AG_Event * event){
	std::string filename( AG_STRING(1) );
	try {
		TiXmlDocument doc;
		// nacteni XML souboru
		TiXmlRootElement(doc, filename, "map", true);
		if(setMap(filename))
			handlerBack();
	}
	catch(const TiXmlException & ex){
		AG_TextError("%s:\n%s\n%s", filename.c_str(),
			LANG_SUBMENU(LANG_DEATHMATCH, LANG_MAPS, LANG_ERR_MAP),
			ex.what());
	}
}

bool MenuDeathmatchMaps::setMap(const std::string & file){
	// najdi posledni lomitko
	std::string::size_type pos= file.rfind('/');
	if(pos==file.npos)
		pos=0;
	else ++pos;

	if(file.size()-pos>=MENU_DEATHMATCH_MAPS_BUFFSIZE-1){
		AG_TextError("%s:\n%s", file.c_str(),
			LANG_SUBMENU(LANG_DEATHMATCH, LANG_MAPS, LANG_ERR_FILE));
		return false;
	}

	map_path = file.substr(0,pos);

	// okopiruj nazev soubor
	std::string::size_type i;
	for(i=pos ; i<file.size() ; ++i){
		map_name[i-pos] = file[i];
	}
	map_name[i-pos] = 0;
	return true;
}
