
#include "menu_deathmatch_maps.h"
#include "tixml_helper.h"

char MenuDeathmatchMaps::map_name[MENU_DEATHMATCH_MAPS_BUFFSIZE] = "";
std::string MenuDeathmatchMaps::map_path = "";

MenuDeathmatchMaps::MenuDeathmatchMaps(){
	const char * paths[] = SEARCH_PATHS;
	locate_dir(paths[0], MENU_DEATHMATCH_MAPS_DIR, map_path);

	AG_WindowMaximize(win);
	AG_WindowSetPadding(win, MENU_OFFSET,
		MENU_OFFSET, MENU_OFFSET, MENU_OFFSET);

	// nadpis
	createHeading("Deathmatch maps");

	file_dlg_ = AG_FileDlgNew(win, AG_FILEDLG_LOAD);
	AG_Expand(file_dlg_);

	AG_FileDlgSetDirectory(file_dlg_, map_path.c_str());

	AG_FileDlgAddType(file_dlg_, "Bombic map XML", TIXML_FILE_EXTENSION, handlerCheckMap, 0);
	AG_FileDlgCancelAction(file_dlg_, handlerBack, 0);

	AG_SpacerNewHoriz(win);
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
			"File does not represent Bombic map.",
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
			"Filename is too long, rename them first.");
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
