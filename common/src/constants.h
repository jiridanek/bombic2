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

#ifndef INSTALL_DIR
/// Adresar, do ktereho se bude instalovat.
#define INSTALL_DIR ".."
#endif
/// Adresar v domovskem adresari uzivatele, ktery bude take prohledavany.
#define SEARCH_DIR_IN_HOME ".bombic"
/// Inicializator pole adresaru pro prohledavani (zakonceny nulou (0)).
#define SEARCH_PATHS { INSTALL_DIR"/common", INSTALL_DIR"/game", 0 }
/// Hloubka, do ktere se ma adresarova struktura prohledavat.
#define SEARCH_DEPTH 5


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

/// Jednotka zoomovaciho posuvniku.
#define ZOOM_STEP 0.1
/// Minimalni hodnota zoomovani.
#define ZOOM_MINIMUM_VALUE 0.3
/// Maximalni hodnota.
#define ZOOM_MAXIMUM_VALUE 1.5

/// Okraj pomocne graficke polozky.
#define HELPER_ITEM_PEN \
	/* color, width, style, capStyle, joinStyle*/ \
	QPen(QColor(255, 255, 255, 128), 3.0)
/// Plocha graficke pomocne polozky pokud vkladat lze.
#define CAN_INSERT_ITEM_BRUSH \
	QBrush(QColor(50, 50, 255, 96))
/// Plocha graficke pomocne polozky pokud vkladat nelze.
#define CANNOT_INSERT_ITEM_BRUSH \
	QBrush(QColor(255, 50, 50, 96))
/// Plocha graficke pomocne polozky pro zvyrazneni policka, ktere je v detailu.
#define SELECTED_FIELD_ITEM_BRUSH \
	QBrush(QColor(255, 255, 255, 52))

#endif
