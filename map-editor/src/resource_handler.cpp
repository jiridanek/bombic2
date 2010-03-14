
#include <constants.h>

#include "resource_handler.h"

#include "bombic/map.h"
#include "bombic/map_background.h"
#include "bombic/map_object.h"

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
	if(!openXmlByName(name, rootEl)) {
		return 0;
	}

	if(!rootEl.hasAttribute("src")) {
		// handle it
		qDebug() << "hasn't attr src";
		return 0;
	}
	if(!openSourcePixmap(rootEl.attribute("src"))) {
		return 0;
	}

	QDomElement bgEl = rootEl.namedItem("clean_bg").toElement();
	if(bgEl.isNull()) {
		// handle it
		qDebug() << "hasn't clean_bg element";
		return 0;
	}
	if(!bgEl.hasAttribute("x")) {
		// handle it
		qDebug() << "hasn't attr x";
		return 0;
	}
	bool converted;
	int x = bgEl.attribute("x").toInt(&converted);
	if(!converted) {
		// handle it
		qDebug() << "attr x not converted";
		return 0;
	}
	if(!bgEl.hasAttribute("y")) {
		// handle it
		qDebug() << "hasn't attr y";
		return 0;
	}
	int y = bgEl.attribute("y").toInt(&converted);
	if(!converted) {
		// handle it
		qDebug() << "attr y not converted";
		return 0;
	}
	BombicMapBackground * mapBg =
		new BombicMapBackground(name,
			sourcePixmap_.copy(x, y, CELL_SIZE, CELL_SIZE) );
	// TODO load walls


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

bool ResourceHandler::openSourcePixmap(const QString & name) {

	if(name==sourcePixmapName_) {
		// the pixmap is loaded 
		return true;
	}

	QString filename = name;
	bool fileLocated = locateFile(filename);
	if(!fileLocated) {
		// TODO handle it
		qDebug() << "file " << filename << " not located";
		return false;
	}

	sourcePixmap_ = QPixmap(filename);
	sourcePixmapName_ = name;
	return true;
}

bool ResourceHandler::openXmlByName(const QString & name,
		QDomElement & rootEl) {

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
	return true;
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


