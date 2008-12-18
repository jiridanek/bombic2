
#include "menu_load_save.h"
#include "SDL_lib.h"
#include "game_intro.h"

std::string MenuLoadSaveGame::path = "";
std::string MenuLoadSaveGame::filename = "";

void MenuLoadSaveGame::setFilePath(const std::string & filepath){
	std::string::size_type pos = filepath.rfind('/');
	if(pos!=filepath.npos){
		path = filepath.substr(0,pos);
		filename = filepath.substr(pos+1);
	}
}

extern SDL_Surface * g_window;
extern GameIntro * g_gameIntro;

/*********** class MenuLoadGame *******************/
MenuLoadGame::MenuLoadGame(){
	AG_FileDlg * file_dlg;

	AG_WindowMaximize(win);
	AG_WindowSetPadding(win, MENU_OFFSET,
		MENU_OFFSET, MENU_OFFSET, MENU_OFFSET);

	// nadpis
	createHeading("Load Game");

	// filemanager
	file_dlg = AG_FileDlgNew(win, AG_FILEDLG_LOAD);
	AG_Expand(file_dlg);
	if(!path.empty() || get_home_path(path))
		AG_FileDlgSetDirectory(file_dlg, path.c_str());
	if(!filename.empty())
		AG_FileDlgSetFilename(file_dlg, "%s", filename.c_str());

	AG_FileDlgAddType(file_dlg, "Bombic saved game XML",
			TIXML_FILE_EXTENSION, handlerLoadGame, 0);
	AG_FileDlgCancelAction(file_dlg, handlerBack, 0);

	AG_SpacerNewHoriz(win);
}

void MenuLoadGame::handlerLoadGame(AG_Event * event){
	std::string filepath( AG_STRING(1) );

	try{
		g_gameIntro->load_game(filepath);
		setFilePath(filepath);
		handlerBack();
		g_gameIntro->show_screen();
	}
	catch(const TiXmlException & ex){
		AG_TextError("%s", ex.what());
	}

	AG_ResizeDisplay(g_window->w, g_window->h);
}

/*********** class MenuSaveGame *******************/
MenuSaveGame::MenuSaveGame(){
	AG_FileDlg * file_dlg;

	AG_WindowMaximize(win);
	AG_WindowSetPadding(win, MENU_OFFSET,
		MENU_OFFSET, MENU_OFFSET, MENU_OFFSET);

	// nadpis
	createHeading("Save Game");

	// filemanager
	file_dlg = AG_FileDlgNew(win, AG_FILEDLG_SAVE);
	AG_Expand(file_dlg);
	if(!path.empty() || get_home_path(path))
		AG_FileDlgSetDirectory(file_dlg, path.c_str());
	if(filename.empty())
		filename = MENU_SAVE_GAME_DEFAULT_FILENAME;
	AG_FileDlgSetFilename(file_dlg, "%s", filename.c_str());

	AG_FileDlgAddType(file_dlg, "Bombic saved game XML",
			TIXML_FILE_EXTENSION, handlerSaveGame, 0);
	AG_FileDlgCancelAction(file_dlg, handlerBack, 0);

	AG_SpacerNewHoriz(win);
}

void MenuSaveGame::handlerSaveGame(AG_Event * event){
	std::string filepath( AG_STRING(1) );
	// pridat priponu
	std::string ext(TIXML_FILE_EXTENSION);
	Uint16 name_s = filepath.size(), ext_s = ext.size();
	if(name_s<=ext_s || filepath.substr(name_s-ext_s)!=ext)
		filepath+= ext;
	try {
		g_gameIntro->save_game(filepath);
		setFilePath(filepath);
		handlerBack();
	}
	catch(const TiXmlException & ex){
		AG_TextError("%s", ex.what());
	}
}
