
#include <QLinkedList>
#include <QBitmap>
#include <QMessageBox>
#include <QFileDialog>
#include <constants.h>

#include "resource_handler.h"

#include "bombic/map.h"
#include "bombic/map_background.h"
#include "bombic/map_object.h"

#include "resource_handlers/wall_resource_handler.h"
#include "bombic/wall.h"
#include "map_object_palette.h"

SINGLETON_INIT(ResourceHandler);

/**
 * @param parent rodic - predany do QObject::QObject()
 */
ResourceHandler::ResourceHandler(QObject * parent):
		QObject(parent) {
	SINGLETON_CONSTRUCT;
	MapObjectResourceHandler::initResourceHandlers();
}

ResourceHandler::~ResourceHandler() {
	SINGLETON_DESTROY;
}

BombicMap * ResourceHandler::loadMap() {
}

/** @details
 * Vytvori mapu s defaultnim pozadim @c DEFAULT_MAP_BACKGROUND
 * a rozmery <code>DEFAULT_MAP_WIDTH, DEFAULT_MAP_HEIGHT</code>.
 * Vlastnictvi nove naalokovane mapy prechazi na volajiciho.
 * @return Nove alokovana prazdna mapa s pozadim.
 */
BombicMap * ResourceHandler::loadEmptyMap() {
	return loadMap("map_concrete_rings");
	BombicMapBackground * defaultBg =
		loadMapBackground(DEFAULT_MAP_BACKGROUND);
	if(!defaultBg) {
		return 0;
	}
	BombicMap * newMap = new BombicMap(
		DEFAULT_MAP_WIDTH, DEFAULT_MAP_HEIGHT, defaultBg);
	if(!newMap) {
		delete defaultBg;
		return 0;
	}
	return newMap;
}

/** @details
 * Pokusi se vytvorit mapu zadanou @p name a vlozit do ni
 * vsechny objekty.
 * @param name jmeno mapy (nebo primo cesta k souboru)
 * @return Objekt mapy.
 * @retval 0 mapu se nepodarilo vyrobit
 */
BombicMap * ResourceHandler::loadMap(const QString & name) {
	QDomElement rootEl;
	if(!loadXml(name, rootEl, true, "map")) {
		return 0;
	}
	QString bgName;
	if(!getStringAttr(rootEl, bgName, "background") ) {
		return 0;
	}
	int w, h;
	bool success =
		getIntAttr(rootEl, w, "width") &&
		getIntAttr(rootEl, h, "height");
	if(!success) {
		return 0;
	}
	BombicMapBackground * mapBg = loadMapBackground(bgName);
	if(!mapBg) {
		return 0;
	}
	BombicMap * map = new BombicMap(w, h, mapBg);
	if(!map) {
		delete mapBg;
		return 0;
	}

	// load map objects
	QDomElement el;
	success =
		getSubElement(rootEl, el, "floorobjects", true) &&
		loadMapFloorobjects(el, map) &&
		getSubElement(rootEl, el, "walls", true) &&
		loadMapWalls(el, map) &&
		getSubElement(rootEl, el, "boxes", true) &&
		loadMapBoxes(el, map) &&
		getSubElement(rootEl, el, "creatures", true) &&
		loadMapCreatures(el, map);

	if(!success) {
		delete map;
		return 0;
	}

	return map;
}

#define RH_FOREACH_SIBLING_ELEMENT(itEl, firstEl) \
	for(QDomElement itEl = firstEl; \
		!itEl.isNull() ; \
		itEl = itEl.nextSiblingElement(firstEl.tagName()) )

bool ResourceHandler::loadMapFloorobjects(const QDomElement & floorsEl,
		BombicMap * map) {
	// for all object prototypes
	RH_FOREACH_SIBLING_ELEMENT(objEl, floorsEl) {
		// object name
		QString name;
		if(!getStringAttr(objEl, name, "name")) {
			return false;
		}
		// the object
		BombicMapObject * obj = loadMapObject(name);
		if(!obj) {
			return false;
		}
		// first position element
		QDomElement posEl;
		if(!getSubElement(objEl, posEl, "floorobject")) {
			return false;
		}
		if(!insertMapObjects(posEl, obj, map)) {
			return false;
		}
	}
	// all floor objects loaded
	return true;
}

bool ResourceHandler::loadMapWalls(const QDomElement & wallsEl,
		BombicMap * map) {
	// for all object prototypes
	RH_FOREACH_SIBLING_ELEMENT(objEl, wallsEl) {
		// object name
		QString name;
		if(!getStringAttr(objEl, name, "name")) {
			return false;
		}
		// the object
		BombicMapObject * obj = loadMapObject(name);
		if(!obj) {
			return false;
		}
		// first position element
		QDomElement posEl;
		if(!getSubElement(objEl, posEl, "wall")) {
			return false;
		}
		if(!insertMapObjects(posEl, obj, map)) {
			return false;
		}
	}
	// all walls loaded
	return true;
}

bool ResourceHandler::loadMapBoxes(const QDomElement & boxesEl,
		BombicMap * map) {
	// for all object prototypes
	RH_FOREACH_SIBLING_ELEMENT(objEl, boxesEl) {
		// object name
		QString name;
		if(!getStringAttr(objEl, name, "name")) {
			return false;
		}
		// the object
		BombicMapObject * obj = loadMapObject(name);
		if(!obj) {
			return false;
		}
		// first position element
		QDomElement posEl;
		if(!getSubElement(objEl, posEl, "box", true)) {
			return false;
		}
		if(!insertMapObjects(posEl, obj, map)) {
			return false;
		}
		// TODO random generated objects
	}
	// all boxes loaded
	return true;
}

bool ResourceHandler::loadMapCreatures(const QDomElement & creaturesEl,
		BombicMap * map) {
	// for all object prototypes
	RH_FOREACH_SIBLING_ELEMENT(objEl, creaturesEl) {
		// object name
		QString name;
		if(!getStringAttr(objEl, name, "name")) {
			return false;
		}
		// the object
		BombicMapObject * obj = loadMapObject(name);
		if(!obj) {
			return false;
		}
		// first position element
		QDomElement posEl;
		if(!getSubElement(objEl, posEl, "creature", true)) {
			return false;
		}
		if(!insertMapObjects(posEl, obj, map)) {
			return false;
		}
		// TODO random generated objects
	}
	// all creatures loaded
	return true;
}

bool ResourceHandler::insertMapObjects(const QDomElement & positionEl,
		BombicMapObject * insertedObject, BombicMap * map) {
	// for all positions
	RH_FOREACH_SIBLING_ELEMENT(posEl, positionEl) {

		BombicMap::Field field;
		if(!getAttrsXY(posEl, field.rx(), field.ry())) {
			return false;
		}
		// try to insert
		if(!map->canInsert(insertedObject, field)) {
			showError(
				tr("Object cannot be inserted to field") +"\n"+
					"[" + QString::number(field.x()) +","+
					QString::number(field.y()) + "]",
				posEl );
			return false;
		}
		// OK - it can be inserted, so create copy and insert it
		map->insert(insertedObject->createCopy(), field);
	}
	// all positions inserted
	return true;
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
	QString objectName = attrNameValueFromName(name);
	BombicMapObject * obj = MAP_OBJECT_PALETTE->getObject(objectName);
	if(obj) {
		MAP_OBJECT_PALETTE->selectObject(objectName);
		return obj;
	}

	QDomElement rootEl;
	if(!loadXml(name, rootEl, true)) {
		return 0;
	}
	MapObjectResourceHandler * objRH =
		MapObjectResourceHandler::resourceHandler(rootEl);
	if(!objRH) {
		showError(
			tr("Don't know which resource handler create.")+"\n"+
			tr("The object with root element")+" "+
			rootEl.tagName()+" "+
			tr("may be wrong or not supported.") );
		return 0;
	}
	if(!loadSourcePixmap(rootEl)) {
		return 0;
	}
	obj = objRH->createMapObject(rootEl);
	if(!obj) {
		return 0;
	}
	MAP_OBJECT_PALETTE->addObject(obj);
	return obj;
}

void ResourceHandler::saveMap(BombicMap * bombicMap) {
}

void ResourceHandler::saveMapAs(BombicMap * bombicMap) {
}

/** @details
 * Pokusi se najit zadany soubor v domovskem adresari,
 * v adresarich uvedenych v SEARCH_PATHS a vsech podadresarich
 * az do hloubky SEARCH_DEPTH.
 * Preskakuje skryte adresare.
 * Pokud nastane chyba, sam zobrazuje relevantni informace.
 * @param[in/out] filename jmeno souboru nebo uplna cesta
 * @return Uspech operace.
 * @retval true Soubor nalezen (cesta ulozena v @p filename)
 * @retval false Soubor nenalezen.
 */
bool ResourceHandler::locateFile(QString & filename) {
	if(filename.isEmpty()) {
		showError(tr("Trying to locate file with empty name"));
		// file cannot have empty name
		return false;
	}
	if(filename.endsWith("/")) {
		showError(
			tr("Trying to locate file that ends up with slash (/)")
			+"\n"+filename);
		return false;
	}
	if(filename.startsWith("/")) {
		// the filename (its path) is absolute
		if(QFile::exists(filename)) {
			return true;
		} else {
			showError(filename+"\n"+
				tr("specified by absolute path doesn't exists"));
			return false; \
		}
	}

	bool located = locateFileInDir(
		QDir::homePath()+"/"+SEARCH_DIR_IN_HOME, filename);
	if(located) {
		return true;
	}

	const char * paths[] = SEARCH_PATHS;
	for( int i=0 ; paths[i] != 0 && !located ; ++i ){
		located = locateFileInDir(QString(paths[i]), filename);
	}
	if(located) {
		return true;
	} else {
		showError(tr("File")+" "+filename+" "+tr("wasn't located"));
		return false;
	}
}

/** @details
 * Pokusi se najit zadany soubor v adresari @p dir
 * a vsech podadresarich az do hloubky SEARCH_DEPTH.
 * Preskakuje skryte adresare.
 * @param dir soubor, ktery se ma prohledat
 * @param[in/out] filename jmeno souboru
 * @param depth dosazena hloubka
 * @return Uspech operace.
 * @retval true Soubor nalezen (cesta ulozena v @p filename)
 * @retval false Soubor nenalezen.
 */
bool ResourceHandler::locateFileInDir(const QDir & dir, QString & filename,
		int depth) {

	if(!dir.isReadable()) {
		return false;
	}

	if( dir.exists(filename) ) {
		filename = dir.filePath(filename);
		return true;
	}

	++depth;
	if(depth > SEARCH_DEPTH) {
		return false;
	}

	foreach(QString entryName, dir.entryList(QDir::Dirs)) {
		if(entryName.at(0)=='.') {
			// skip . .. and hidden dirs
			continue;
		}
		bool located = locateFileInDir(
			dir.filePath(entryName), filename, depth);
		if(located) {
			return true;
		}
	}
	return false;
}

/** @details
 * Pokusi se najit a otevrit xml soubor.
 * Provadi kontroly:
 * - hodnota atributu name v korenovem elementu
 * - jmeno korenoveho elementu
 * Pokud nastane chyba, sam zobrazuje relevantni informace.
 * @param name jmeno objektu, souboru, nebo cela cesta
 * @param[out] rootEl korenovy element nove otevreneho xml dokumentu
 * @param checkAttrName zda se ma kontrolovat hodnota atributu name
 *                      podle @p name (viz attrNameValueFromName())
 * @return Uspech operace.
 * @retval true dokument nacten (korenovy element ulozen v @p rootEl)
 * @retval false dokument nenacten
 */
bool ResourceHandler::loadXml(const QString & name,
		QDomElement & rootEl, bool checkAttrName,
		const QString & rootElTagName) {

	QString filename = name;
	if(!filename.endsWith(XML_FILE_EXTENSION)) {
		filename += XML_FILE_EXTENSION;
	}
	bool fileLocated = locateFile(filename);
	if(!fileLocated) {
		return false;
	}

	QFile file(filename);
	if(!file.open(QIODevice::ReadOnly)) {
		showError(filename+"\n"+tr("couldn't be opened"));
		return false;
	}

	QDomDocument doc(filename);
	QString errMsg;
	int errRow;
	int errCol;
	bool fileParsed = doc.setContent(&file, &errMsg, &errRow, &errCol);
	file.close();
	if(!fileParsed) {
		showError(tr("Parse error")+" "+tr("at line")+" "+
			QString::number(errRow)+" "+tr("and column")+" "+
			QString::number(errCol)+" "+tr("occure")+".\n"+
			tr("Details follow")+":\n"+errMsg, filename);
		return false;
	}

	rootEl = doc.documentElement();

	// some checking
	if(!rootElTagName.isEmpty() && rootEl.tagName()!=rootElTagName) {
		showError(tr("Wrong root element, it should be")+" "+
			rootElTagName, filename);
		return false;
	}
	if(checkAttrName) {
		QString nameValue = attrNameValueFromName(name);
		if(rootEl.attribute("name")!=nameValue) {
			showError(
				tr("Wrong value of attribute name, it should be")
				+" "+nameValue, rootEl, filename);
			return false;
		}
	}

	return true;
}

/** @details
 * Pokud obsahuje @p name priponu xml souboru nebo cestu,
 * tyto kusy se odstrani.
 * @param name jmeno objektu, souboru, nebo cela cesta
 * @return Predpokladane jmeno objektu.
 */
QString ResourceHandler::attrNameValueFromName(const QString & name) {
	QString attrNameValue = name;
	// strip extension
	QString ext = XML_FILE_EXTENSION;
	if(attrNameValue.endsWith(ext)) {
		attrNameValue.chop(ext.size());
	}
	// remove path
	int slashPos = attrNameValue.lastIndexOf("/");
	if(slashPos>=0) {
		attrNameValue.remove(0, slashPos+1);
	}
	return attrNameValue;
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
 * Pokusi se ziskat podelement elementu @p el.
 * Pokud element neexistuje, sam zobrazuje relevantni informace.
 * Pokud subelement chybi, @p subEl je nastaven jako nulovy (@c isNull()).
 * @param el element, o jehoz podelement mame zajem
 * @param[out] subEl nalezeny (ci nulovy) podelement
 * @param subElTagName jmeno podelementu, ktery hledame
 * @param successIfMissing jestli ma uspet pokud element uplne chybi
 * @return Uspech operace.
 * @retval true podelement nalezen a vracen v @p subEl
 * @retval false podelement chybi
 */
bool ResourceHandler::getSubElement(const QDomElement & el,
		QDomElement & subEl, const QString & subElTagName,
		bool successIfMissing) {

	subEl = el.namedItem(subElTagName).toElement();
	if(subEl.isNull() && !successIfMissing) {
		showError(tr("Missing subelement")+" "+subElTagName, el);
		return false;
	}

	return true;
}

/** @details
 * Pokusi se ziskat hodnotu atributu @p attrName elementu @p el.
 * Pokud nastane chyba, sam zobrazuje relevantni informace.
 * @param el element, o jehoz atribut mame zajem
 * @param[out] attr hodnota atributu
 * @param attrName jmeno atributu, ktery hledame
 * @param successIfMissing jestli ma uspet pokud atribut uplne chybi
 * @return Uspech operace.
 * @retval true hodnota atributu nactena
 *              (nebo chybi a @p successIfMissing je @c true )
 * @retval false chybi a @p successIfMissing je @c false
 */
bool ResourceHandler::getStringAttr(const QDomElement & el,
		QString & attr, const QString & attrName,
		bool successIfMissing) {

	if(!el.hasAttribute(attrName)) {
		if(successIfMissing) {
			return true;
		} else {
			showError(tr("Missing attribute")+" "+attrName, el);
			return false;
		}
	}
	// all ok - store the value
	attr = el.attribute(attrName);
	return true;
}

/** @details
 * Pokusi se ziskat hodnotu atributu @p attrName elementu @p el
 * a prevest ji na integer.
 * Pokud nastane chyba, sam zobrazuje relevantni informace.
 * @param el element, o jehoz atribut mame zajem
 * @param[out] attr hodnota atributu
 * @param attrName jmeno atributu, ktery hledame
 * @param successIfMissing jestli ma uspet pokud atribut uplne chybi
 * @return Uspech operace.
 * @retval true hodnota atributu nactena
 *              (nebo chybi a @p successIfMissing je true)
 * @retval false hodnota atributu nelze interpretovat jako int
 *              (nebo chybi a @p successIfMissing je false)
 */
bool ResourceHandler::getIntAttr(const QDomElement & el,
		int & attr, const QString & attrName, bool successIfMissing) {

	QString stringAttr;
	bool success = getStringAttr(el, stringAttr,
		attrName, successIfMissing);
	if(stringAttr.isNull()) {
		// attribute is missing
		return success;
	}

	// attribute in not missing - convert it
	bool converted;
	int intAttr = stringAttr.toInt(&converted);
	if(!converted) {
		showError(tr("Attribute")+" "+attrName+" "
			+tr("cannot be converted to integer"), el);
		return false;
	}
	// all ok - store converted value
	attr = intAttr;
	return true;
}

/** @details
 * Ziska integer hodnoty atributu x,y.
 * Atributy nesmi chybet.
 * @param el element, jehoz atributy nas zajimaji
 * @param[out] x hodnota atributu x
 * @param[out] y hodnota atributu y
 * @return Uspech operace.
 * @see getIntAttr()
 */
bool ResourceHandler::getAttrsXY(const QDomElement & el, int & x, int & y) {
	return getIntAttr(el, x, "x") && getIntAttr(el, y, "y");
}

/** @details
 * Zobrazi dialogove okno se zpravou @p message.
 * @param message textovy  popis chyby
 * @see QMessageBox::critical()
 */
void ResourceHandler::showError(const QString & message) {
	QMessageBox::critical(0, tr("Error in Bombic resource"),
		message, QMessageBox::Ok);
}

/** @details
 * Zobrazi chybove hlaseni korespondujici s urcitym souborem @p filename.
 * @param message textovy popis chyby
 * @param filename jmeno souboru, ve kterem se chyba vyskytla
 * @param el pripadne xml element, ve kterem chyba nastala
 */
void ResourceHandler::showError(const QString & message,
		const QString & filename,
		const QDomElement & el ) {

	QString final_message;
	if(!filename.isEmpty()) {
		final_message = tr("In the file:") + "\n"
			+ filename + "\n";
	}
	if(!el.isNull()) {
		final_message += tr("In element:") + "\n"
			+ nodePath(el) + "\n";
	}
	final_message += message;
	showError(final_message);
}

/** @details
 * Zobrazi chybove hlaseni korespondujici s urcitym xml elementem @p el.
 * Vypisuje cestu k elementu.
 * @param message textovy popis chyby
 * @param el xml element, ve kterem chyba nastala
 * @param pripadne jmeno souboru, ve kterem chyba nastala
 * @see nodePath()
 */
void ResourceHandler::showError(const QString & message,
		const QDomElement & el,
		const QString & filename) {
	showError(message, filename, el);
}

/** @details
 * Zkonstruuje (textove) cestu k xml uzlu @p node.
 * Cesta uzlu jsou jmena nadrazenych elementu az ke koreni
 * xml stromu.
 * @param node uzel jehoz cesta nas zajima
 * @param delimiter oddelovac jednotlivych elementu cesty
 * @return Zkonstruovana cesta uzlu.
 */
QString ResourceHandler::nodePath(const QDomNode & node,
		const QString & delimiter) {
	QString path = node.nodeName();
	for(QDomNode n = node.parentNode() ;
			!n.isNull() && n.nodeType()!=QDomNode::DocumentNode ;
			n = n.parentNode() ) {
		path = n.nodeName() + delimiter + path;
	}
	return path;
}
