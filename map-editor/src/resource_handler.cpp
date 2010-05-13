
#include <QBitmap>
#include <QFileDialog>
#include <QSignalMapper>
#include <QAction>
#include <constants.h>

#include "resource_handler.h"


#include "bombic/map.h"
#include "bombic/map_background.h"
#include "bombic/map_object.h"
#include "generators/map_object_generator.h"

#include "resource_handlers/resource_handler_functions.h"
#include "resource_handlers/map_resource_handler.h"
#include "resource_handlers/wall_resource_handler.h"
#include "bombic/wall.h"

#include "map_object_palette.h"
#include "main_window.h"

SINGLETON_INIT(ResourceHandler);

using namespace ResourceHandlerFunctions;

/// Nazev souboru, ve kterem jsou systemova data pro map editor.
#define RESOURCE_HANDLER_EDITOR_FILE "map-editor"

/**
 * @param parent rodic - predany do QObject::QObject()
 */
ResourceHandler::ResourceHandler(QObject * parent):
		QObject(parent), mapResourceHandler_(new MapResourceHandler) {
	SINGLETON_CONSTRUCT;
	MapObjectResourceHandler::initResourceHandlers();

	loadSetsSignalMapper_ = new QSignalMapper(this);
	connect(loadSetsSignalMapper_, SIGNAL(mapped(QString)),
		this, SLOT(loadSetByName(QString)) );

	parseEditorFile();
}

ResourceHandler::~ResourceHandler() {
	delete mapResourceHandler_;

	SINGLETON_DESTROY;
}

/** @details
 * Necha uzivatele vybrat mapu a tu pote nacte.
 * Vlastnictvi nove naalokovane mapy prechazi na volajiciho.
 * @see MapResourceHandler::createMap()
 * @return Objekt mapy.
 * @retval 0 Mapa nebyla nactena.
 */
BombicMap * ResourceHandler::loadMap() {
	QString filename = QFileDialog::getOpenFileName(
		MAIN_WINDOW, tr("Open map"), "",
		tr("Bombic map files")+" (*"XML_FILE_EXTENSION")" );
	if(filename.isEmpty()) {
		return 0;
	} else {
		return loadMap(filename);
	}
}

/** @details
 * Vlastnictvi nove naalokovane mapy prechazi na volajiciho.
 * @see MapResourceHandler::createEmptyMap()
 * @return Nove alokovana prazdna mapa s pozadim.
 */
BombicMap * ResourceHandler::loadEmptyMap() {
	return mapResourceHandler_->createEmptyMap();
}

/** @details
 * @param name jmeno mapy (nebo primo cesta k souboru)
 * @see MapResourceHandler::createMap()
 * @return Objekt mapy.
 * @retval 0 mapu se nepodarilo vyrobit
 */
BombicMap * ResourceHandler::loadMap(const QString & name) {
	return mapResourceHandler_->createMap(name);
}

/**
 * @param name jmeno pozadi (nebo souboru s pozadim)
 * @return Nove naalokovane pozadi mapy,
	vlastnictvi prechazi na volajiciho.
 * @retval 0 nastala chyba, pozadi nevytvoreno
 */
BombicMapBackground * ResourceHandler::loadMapBackground(
		const QString & name) {
	QDomElement rootEl;
	if(!loadXml(name, rootEl, true, "background")) {
		return 0;
	}
	if(!loadSourcePixmap(rootEl)) {
		return 0;
	}
	QDomElement bgEl;
	if(!getSubElement(rootEl, bgEl, "clean_bg")) {
		return 0;
	}
	int x = 0;
	int y = 0;
	if(!getAttrsXY(bgEl, x, y)) {
		return 0;
	}
	BombicMapBackground * mapBg =
		new BombicMapBackground(name,
			sourcePixmap_.copy(x, y, CELL_SIZE, CELL_SIZE) );

	WallResourceHandler wallRH;

	#define CREATE_AND_ADD(situation, tagName) \
		do { \
			QDomElement wallEl; \
			bool success = getSubElement( \
				rootEl, wallEl, tagName); \
			if(!success) { \
				delete mapBg; \
				return 0; \
			} \
			BombicWall * bombicWall = wallRH.createWall("", wallEl); \
			if(!bombicWall) { \
				delete mapBg; \
				return 0; \
			} \
			mapBg->setWall(bombicWall, situation); \
		} while(0)
	CREATE_AND_ADD(BombicMapBackground::TopLeft, "topleft");
	CREATE_AND_ADD(BombicMapBackground::Bottom, "bottom");
	CREATE_AND_ADD(BombicMapBackground::BottomRight, "bottomright");
	CREATE_AND_ADD(BombicMapBackground::BottomLeft, "bottomleft");
	CREATE_AND_ADD(BombicMapBackground::Top, "top");
	CREATE_AND_ADD(BombicMapBackground::TopRight, "topright");
	CREATE_AND_ADD(BombicMapBackground::Right, "right");;
	CREATE_AND_ADD(BombicMapBackground::Left, "left");
	#undef CREATE_AND_ADD

	return mapBg;
}

/** @details
 * Necha uzivatele vybrat soubory s objekty mapy,
 * vsechny vybrane objekty se pokusi nahrat.
 */
void ResourceHandler::loadMapObject() {
	QStringList filenames = QFileDialog::getOpenFileNames(
		MAP_OBJECT_PALETTE, tr("Map object file"), "",
		tr("Map object files")+" (*"XML_FILE_EXTENSION")" );
	foreach(QString filename, filenames) {
		loadMapObject(filename);
	}
}

/** @details
 * Spolupracuje s MapObjectPalette.
 * Pred nahranim noveho objektu se ujisti, ze jeste v palete neni.
 * Pokud objekt v palete jiz je, vraci objekt z palety.
 * Jinak se pokusi objekt nacist pomoci
 * specializovaneho resource handleru. Pokud se podari objekt nacist,
 * novy objekt registruje v palete a zaroven ho vraci.
 * Vlastn�kem objektu je tedy paleta. Pokud chcete objekt vlastnit,
 * z vraceneho objektu si vytvorte kopii.
 * @param name jmeno objektu (nebo primo cesta k souboru)
 * @return Objekt mapy registrovany v palete.
 * @retval 0 objekt se nepodarilo nahrat
 */
BombicMapObject * ResourceHandler::loadMapObject(const QString & name) {
	// try to find it in palette
	QString objectName = attrNameValueFromName(name);
	BombicMapObject * obj = MAP_OBJECT_PALETTE->getObject(objectName);
	if(obj) {
		MAP_OBJECT_PALETTE->selectObject(objectName);
		return obj;
	}
	// ok, load it from disk
	QDomElement rootEl;
	if(!loadXml(name, rootEl, true)) {
		return 0;
	}
	// find the specialized resource handler
	MapObjectResourceHandler * objRH =
		MapObjectResourceHandler::resourceHandler(rootEl);
	if(!objRH) {
		// oops it doesn't exists
		showError(
			tr("Don't know which resource handler create.")+"\n"+
			tr("The object with root element")+" "+
			rootEl.tagName()+" "+
			tr("may be wrong or not supported.") );
		return 0;
	}
	// load the source map of it
	if(!loadSourcePixmap(rootEl)) {
		return 0;
	}
	// and create it
	obj = objRH->createMapObject(rootEl);
	if(!obj) {
		return 0;
	}
	// register the new loaded object in palette
	MAP_OBJECT_PALETTE->addObject(obj);
	return obj;
}

/** @details
 * Pokud ma mapa prirazene jmeno souboru, ulozi mapu do tohoto souboru.
 * Jinak necha vybrat uzivatele.
 * @param map mapa, ktera se ma ulozit
 */
bool ResourceHandler::saveMap(BombicMap * map) {
	const QString & filename = map->filename();
	if(filename.isEmpty()) {
		return saveMapAs(map);
	}
	return mapResourceHandler_->saveMap(map);
}

/**
 * Necha vybrat uzivatele nove jmeno souboru a mapu do nej ulozi.
 * @param map mapa, ktera se ma ulozit
 */
bool ResourceHandler::saveMapAs(BombicMap * map) {
	QString filename = QFileDialog::getSaveFileName(
		MAIN_WINDOW, tr("Save map"), map->filename(),
		tr("Bombic map files")+" (*"XML_FILE_EXTENSION")" );
	if(filename.isEmpty()) {
		return false;
	}
	// add the extension if it is missing
	filename = filenameFromName(filename);

	map->setFilename(filename);
	map->setName(attrNameValueFromName(filename));
	return saveMap(map);
}

/** @details
 * Zdrojovy obrazek je vcelku velky, a je v nem ulozeno vice objektu.
 * Proto ResourceHandler drzi naposledy nacteny zdrojovy obrazek,
 * kdyby nekdo priste chtel ten samy, uz se nemusi hledat a nacitat.
 * Tato funkce tedy z xml atributu @p attrName elementu @p el precte
 * nazev zdrojoveho obrazku a pokud tento obrazek jiz ma nacteny,
 * pouze tise uspeje. Pokud je posledni nacteny obrazek jiny,
 * tento obrazek nacte a ulozi ho do @c sourcePixmap_.
 * Pokud nastane chyba, sam zobrazuje relevantni informace.
 * Zdrojovemu obrazku vytvori masku pruhlednosti podle pruhledne barvy.
 * @param el element, ze ktereho se ma ziskat hodnota atributu
 * @param attrName jmeno atributu, jehoz hodnota urci jmeno obrazku
 * @return Uspech operace.
 * @retval true obrazek byl nacten a ulozen do @c sourcePixmap_
 * @retval false obrazek nenacten
 */
bool ResourceHandler::loadSourcePixmap(const QDomElement & el,
		const QString & attrName) {
	// get name of pixmap
	if(!el.hasAttribute(attrName)) {
		showError(tr("Missing attribute")+" "+attrName, el);
		return false;
	}
	QString name = el.attribute(attrName);
	// load the pixmap
	if(name==sourcePixmapName_) {
		// the pixmap is loaded
		return true;
	}
	// first locate the file
	QString filename = name;
	bool fileLocated = locateFile(filename);
	if(!fileLocated) {
		return false;
	}
	// load pixmap
	QPixmap pixmap(filename);
	if(pixmap.isNull()) {
		showError(filename+"\n"+tr("couldn't be opened")+" "+
			tr("or has unknown format"));
		return false;
	}
	// store the pixmap and its name for future use
	sourcePixmap_ = pixmap;
	sourcePixmapName_ = name;

	// set the pixmap transparent
	sourcePixmap_.setMask(
		sourcePixmap_.createMaskFromColor(Qt::magenta));

	return true;
}


/** @details
 * Pokusi se najit souboru, ve kterem jsou systemova data
 * a z nich inicializuje nacitaci sady a seznam automaticky nacitanych objektu.
 */
void ResourceHandler::parseEditorFile() {
	QString filename = RESOURCE_HANDLER_EDITOR_FILE;
	QDomElement rootEl;
	if(!loadXml(filename, rootEl, false, "editor")) {
		return;
	}
	initAutoLoadObjects(rootEl);
	initLoadSets(rootEl);
}

/** Cyklus pres vsechny podelementy zadaneho jmena.
 * Vytvori cyklus zacinajici prvnim podelementem, ktery postupne
 * projde vsechny sousedni (sibling) elementy stejneho jmena.
 * Promenna cyklu @p itEl by nemela byt definovana, protoze je definovana
 * a inicializovana zde.
 * @param itEl jmeno iteracni promenne cyklu
 * @param name nazev elementu
 * @param parentEl rodicovsky element
 */
#define RH_FOREACH_SUB_ELEMENT(itEl, name, parentEl) \
	for(QDomElement itEl = parentEl.namedItem(name).toElement(); \
		!itEl.isNull() ; \
		itEl = itEl.nextSiblingElement(name) )

/** @details
 * Inicializuje seznam automaticky nacitanych objektu.
 * @param rootEl korenovy element definice editoru
 */
void ResourceHandler::initAutoLoadObjects(const QDomElement & rootEl) {
	QDomElement autoLoadEl;
	getSubElement(rootEl, autoLoadEl, "autoload", true);

	xmlObjectNamesToList(autoLoadEl, autoLoadObjects_);
}

/** @details
 * Inicializuje seznam sad, ktere se nacitaji spolecne.
 * @param rootEl korenovy element definice editoru
 */
void ResourceHandler::initLoadSets(const QDomElement & rootEl) {

	// connect the menu action to load all
	QAction * loadAction = MAIN_WINDOW->addLoadAllSetsAction();
	connect(loadAction, SIGNAL(triggered()),
		this, SLOT(loadAllSets()) );

	// init all sets
	QDomElement loadSetsEl;
	getSubElement(rootEl, loadSetsEl, "loadsets", true);

	RH_FOREACH_SUB_ELEMENT(loadSetEl, "loadset", loadSetsEl) {
		if(loadSetEl.hasAttribute("name")) {
			// name of the set
			QString name = loadSetEl.attribute("name");
			// add and connect the menu action
			QAction * loadAction = MAIN_WINDOW->addLoadSetAction(name);
			connect(loadAction, SIGNAL(triggered()),
				loadSetsSignalMapper_, SLOT(map()) );
			loadSetsSignalMapper_->setMapping(
				loadAction, name);

			// init the background and object lists
			xmlBackgroundNamesToList(loadSetEl,
				loadSets_[name].backgrounds);
			xmlObjectNamesToList(loadSetEl,
				loadSets_[name].objects);
		}
	}
}

/**
 * @param parentEl rodicovsky element seznamu objektu
 * @param objectNames seznam objektu, do ktereho se maji jmena objektu pridat
 */
void ResourceHandler::xmlObjectNamesToList(const QDomElement & parentEl,
		ObjectNamesT & objectNames) {
	RH_FOREACH_SUB_ELEMENT(objEl, "object", parentEl) {
		if(objEl.hasAttribute("name")) {
			objectNames.append(objEl.attribute("name"));
		}
	}
}

/**
 * @param parentEl rodicovsky element seznamu pozadi
 * @param backgrounds seznam pozadi, do ktereho se maji jmena pozadi pridat
 */
void ResourceHandler::xmlBackgroundNamesToList(const QDomElement & parentEl,
		BackgroundNamesT & backgrounds) {
	RH_FOREACH_SUB_ELEMENT(bgEl, "background", parentEl) {
		if(bgEl.hasAttribute("name")) {
			backgrounds.insert(bgEl.attribute("name"));
		}
	}
}

/** @details
 * Ziska z nacitacich sad jmena pozadi.
 * @return Jmena pozadi z nacitacich sad.
 */
ResourceHandler::BackgroundNamesT ResourceHandler::backgroundsInSets() {
	BackgroundNamesT backgrounds;
	// unite backgrounds from all sets
	foreach(LoadSetT set, loadSets_) {
		backgrounds.unite(set.backgrounds);
	}
	return backgrounds;
}

/** @details
 * Nacte objekty, ktere jsou vedene jako automaticky nacitane.
 */
void ResourceHandler::autoLoadObjects() {
	foreach(QString objName, autoLoadObjects_) {
		if(!loadMapObject(objName)) {
			showError(
				tr("Autoloaded object")+" "+
					objName+" "+tr("cannot be loaded.") );
		}
	}
	MAP_OBJECT_PALETTE->unselectObject();
}

/** @details
 * Nacte objekty, ktere jsou vedene v sade pojmenovane jako @p setName.
 * @param setName nazev sady, kterou chceme nacist
 */
void ResourceHandler::loadSetByName(const QString & setName) {
	if(loadSets_.contains(setName)) {
		loadSet(loadSets_.value(setName));
	}
}

/** @details
 * Pokusi se najit sadu podle daneho pozadi.
 * Objekty v sade nacte.
 * @param bgName nazev pozadi
 */
void ResourceHandler::loadSetByBackground(const QString & bgName) {
	foreach(LoadSetT set, loadSets_) {
		if(set.backgrounds.contains(bgName)) {
			loadSet(set);
			return;
		}
	}
}

/** @details
 * Nacte postupne vsechny dostupne sady objektu.
 */
void ResourceHandler::loadAllSets() {
	foreach(LoadSetT set, loadSets_) {
		loadSet(set);
	}
}

/**
 * @param set sada, jejiz objekty chceme nacist
 */
void ResourceHandler::loadSet(const LoadSetT & set) {
	foreach(QString objName, set.objects) {
		if(!loadMapObject(objName)) {
			showError(
				tr("Object")+" "+objName+" "+
				tr("in loaded set cannot be loaded.") );
		}
	}
	MAP_OBJECT_PALETTE->unselectObject();
}
