/** @file constants.h
 * Soubor obsahující všeobecně používané konstanty
 */

#ifndef CONSTANTS_
#define CONSTANTS_

/********************* Common definitions *********************/

/// Velikost políčka v pixelech.
#define CELL_SIZE 50
/// Pripona XML souboru.
#define XML_FILE_EXTENSION ".xml"

/********************* Game-specific definitions *********************/

/// Perioda, se kterou se provádí hýbnutí světem.
#define MOVE_PERIOD 10
/// Font pro vykreslování textu.
#define FONT_NAME "verdana.ttf"
/// Velikost fontu.
#define FONT_SIZE 15
/// Vzhled menu.
#define AGAR_THEME "bombic.acs"


/********************* Map-editor-specific definitions *********************/

/// Vychozi sirka mapy v map editoru.
#define DEFAULT_MAP_WIDTH 16
/// Vychozi vyska mapy v map editoru.
#define DEFAULT_MAP_HEIGHT 12
/// Pozadi pouzite v map editoru jako vychozi.
#define DEFAULT_MAP_BACKGROUND "bg_forest"


#endif
