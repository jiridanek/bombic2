/** @file language.h
 * Jazykové mutace hry.
 * Language.h obsahuje stejnojmennou třídu obstarávající jazykové mutace
 * a jiné pomocné třídy.
 */

#ifndef LANGUAGE_H
#define LANGUAGE_H

#include <string>
#include "SDL_lib.h"
#include "tixml_helper.h"

#define LANG Language::get_instance()

#define LANG_MENU(level2, level3) \
	LANG->get_translation("menu", level2, level3)
#define LANG_GAME(level2, level3) \
	LANG->get_translation("game", level2, level3)

#define LANG_ROOT_ELEMENT "language"
#define LANG_HEADING 0
#define LANG_BEFORE_LEVEL 0
#define LANG_AFTER_LEVEL "after"

#define LANG_MAIN "main"
#define LANG_RESUME_GAME "resume_game"
#define LANG_NEW_GAME "new_game"
#define LANG_LOAD_GAME "load_game"
#define LANG_SAVE_GAME "save_game"
#define LANG_DEATHMATCH "deathmatch"
#define LANG_OPTIONS "options"
#define LANG_BONUSES "bonuses"
#define LANG_CREATURES "creatures"
#define LANG_QUIT "quit"

#define LANG_PLAY "play"
#define LANG_PLAYERS_COUNT "players_count"
#define LANG_EPISODE "episode"
#define LANG_BACK "back"

#define LANG_FILE_TYPE "filetype"
#define LANG_DIRECTORY "directory"
#define LANG_FILE "file"
#define LANG_TYPE "type"
#define LANG_OK "ok"
#define LANG_CANCEL "cancel"

#define LANG_MAP "map"
#define LANG_WIN_POINTS "win_points"
#define LANG_BOMBS "bombs_at_end"

#define LANG_LANGUAGE "language"
#define LANG_CONTROLS "controls"
#define LANG_SPEED "speed"
#define LANG_PRESUMPTIONS "presumptions"
#define LANG_SPLIT_SCREEN "split_screen"
#define LANG_FULLSCREEN "fullscreen"
#define LANG_SOUND "sound"
#define LANG_SAVE "save"
#define LANG_ERR_SET_LANG "cant_set_lang"

/** Jazyk hry.
 * Instance třídy Language obstarává jazyk menu a hry.
 * Language může vytvořit pouze jedinou instanci,
 * která je vždy vytvořena při načtení konfigurace
 * a zrušena při ukončení aplikace.
 * Implementuje nástroje,
 * kterými tuto skutečnost hlídá a poskytuje svůj pointer globálně,
 * prostřednictvím fce Language::get_instance().
 */
class Language {
	friend class Config;
	friend class MenuOptions;
	public:
		/// Získat pointer na jedinou instanci třídy.
		static Language * get_instance();

		/// Inicializace jazyka ze souboru.
		Language();
		/// Destruktor.
		~Language();
		/// Překlad.
		const char * get_translation(const char * level1,
			const char * level2, const char * level3);
	private:
		/// Pointer na jedinou instanci třídy.
		static Language* myself_ptr_;
		/// Načte konfiguraci z XML.
		void set_language_(const std::string & lang);
		/// XML dokument s jazykem.
		TiXmlDocument doc_;
		/// Pointer na root element language.
		TiXmlElement * rootEl_;
};

#endif