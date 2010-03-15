
#include <QLinkedList>
#include <QBitmap>
#include <constants.h>

#include "resource_handler.h"

#include "bombic/map.h"
#include "bombic/map_background.h"
#include "bombic/map_object.h"

#include "resource_handlers/wall_resource_handler.h"
#include "bombic/wall.h"

SINGLETON_INIT(ResourceHandler);

ResourceHandler::ResourceHandler(QObject * parent):
		QObject(parent) {
	SINGLETON_CONSTRUCT;
}

ResourceHandler::~ResourceHandler() {
	SINGLETON_DESTROY;
}

BombicMap * ResourceHandler::loadMap() {
}

#include <QDebug>

BombicMapBackground * ResourceHandler::loadMapBackground(
		const QString & name) {
	QDomElement rootEl;
	if(!loadXmlByName(name, rootEl, "background", true)) {
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
	// will create anonymous walls
	WallResourceHandler wallRH("");

	#define CREATE_AND_ADD(situation, tagName) \
		do { \
			QDomElement wallEl; \
			bool success = getSubElement( \
				rootEl, wallEl, tagName); \
			if(!success) { \
				delete mapBg; \
				return 0; \
			} \
			BombicWall * bombicWall = wallRH.createWall(wallEl); \
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

BombicMapObject * ResourceHandler::loadMapObject() {
}

BombicMapObject * ResourceHandler::loadMapObject(const QString & name) {
}

void ResourceHandler::saveMap(BombicMap * bombicMap) {
}

void ResourceHandler::saveMapAs(BombicMap * bombicMap) {
}

bool ResourceHandler::locateFile(QString & filename) {
	if(filename.isEmpty()) {
		// file cannot have empty name
		return false;
	}
	if(filename[filename.size()-1]=='/') {
		// file cannot end with slash
		return false;
	}
	if(filename[0]=='/') {
		// the filename (its path) is absolute
		return QFile::exists(filename);
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
	return located;
}

bool ResourceHandler::locateFileInDir(const QDir & dir, QString & filename,
		int depth) {
	// qDebug() << "now in dir " << dir.absolutePath();
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

bool ResourceHandler::loadXmlByName(const QString & name,
		QDomElement & rootEl, const QString & rootElTagName,
		bool checkAttrName) {

	QString filename = name + XML_FILE_EXTENSION;
	bool fileLocated = locateFile(filename);
	if(!fileLocated) {
		// TODO handle it
		qDebug() << "file " << filename << " not located";
		return false;
	}

	QFile file(filename);
	if(!file.open(QIODevice::ReadOnly)) {
		// TODO handle it
		qDebug() << "can't open file " << filename;
		return false;
	}

	QDomDocument doc;
	bool fileParsed = doc.setContent(&file);
	file.close();
	if(!fileParsed) {
		// TODO handle it
		qDebug() << "file " << filename << " is not xml file";
		return false;
	}

	rootEl = doc.documentElement();

	if(rootEl.tagName()!=rootElTagName) {
		// TODO handle it
		qDebug() << "the root element of " << filename << " should be " << rootElTagName;
		return false;
	}
	if(checkAttrName && rootEl.attribute("name")!=name) {
		// TODO handle it
		qDebug() << "the name in root element of " << filename << " should be " << name;
		return false;
	}

	return true;
}

bool ResourceHandler::loadSourcePixmap(const QDomElement & el,
		const QString & attrName) {
	// get name of pixmap
	if(!el.hasAttribute(attrName)) {
		// handle it
		qDebug() << "hasn't attr " << attrName;
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
		// TODO handle it
		qDebug() << "file " << filename << " not located";
		return false;
	}
	// store the pixmap and its name
	sourcePixmap_ = QPixmap(filename);
	sourcePixmapName_ = name;

	// set the pixmap transparent
	sourcePixmap_.setMask(
		sourcePixmap_.createMaskFromColor(Qt::magenta));

	return true;
}

bool ResourceHandler::getSubElement(const QDomElement & el,
		QDomElement & subEl, const QString & subElTagName) {

	subEl = el.namedItem(subElTagName).toElement();
	if(subEl.isNull()) {
		// handle it
		qDebug() << "element " << el.tagName() << " hasn't subelement " << subElTagName;
		return false;
	}

	return true;
}

bool ResourceHandler::getIntAttr(const QDomElement & el,
		int & attr, const QString & attrName, bool successIfMissing) {

	if(!el.hasAttribute(attrName)) {
		if(successIfMissing) {
			return true;
		} else {
			// handle it
			qDebug() << "element " << el.tagName() << " hasn't attribute " << attrName;
			return false;
		}
	}
	bool converted;
	int a = el.attribute(attrName).toInt(&converted);
	if(!converted) {
		// handle it
		qDebug() << "attribute " << attrName << " in element "
			<< el.tagName() << " cannot be converted to int";
		return false;
	}

	attr = a;
	return true;
}

bool ResourceHandler::getAttrsXY(const QDomElement & el, int & x, int & y) {
	return getIntAttr(el, x, "x") && getIntAttr(el, y, "y");
}


