
#include "resource_handler.h"

#include "bombic/map.h"
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

BombicMapBackground * ResourceHandler::loadMapBackground(
		const QString & name) {
	QDomElement rootEl;
	if(!openXmlByName(name, rootEl)) {
		return 0;
	}

	if(!rootEl.hasAttribute("src")) {
		// handle it
		return 0;
	}
	if(!openSourcePixmap(rootEl.attribute("src"))) {
		return 0;
	}

	QDomElement bgEl = rootEl.namedItem("clean_bg").toElement();
	if(bgEl.isNull()) {
		// handle it
		return 0;
	}
	if(!bgEl.hasAttribute("x")) {
		// handle it
		return 0;
	}
	int x = bgEl.hasAttribute("x");
	if(!bgEl.hasAttribute("y")) {
		// handle it
		return 0;
	}
	int y = bgEl.hasAttribute("y");
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

	QString filename = name;
	bool fileLocated = locateFile(filename);
	if(!fileLocated) {
		// TODO handle it
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
		return false;
	}

	QFile file(filename);
	if(!file.open(QIODevice::ReadOnly)) {
		// TODO handle it
		return false;
	}

	QDomDocument doc;
	bool fileParsed = doc.setContent(&file);
	file.close();
	if(!fileParsed) {
		// TODO handle it
		return false;
	}

	rootEl = doc.documentElement();
	return true;
}

bool ResourceHandler::locateFile(QString & filename) {
	if(filename.empty()) {
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

	bool located = locateFileInPath(
		QDir::homePath()+SEARCH_DIR_IN_HOME, filename);
	if(located) {
		return true;
	}

	const char * paths[] = SEARCH_PATHS;
	for( int i=0 ; paths[i] != 0 && !located ; ++i ){
		located = locateFileInDir(paths[i], filename);
	}
	return located;
}

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
		bool located = locateFileInDir(entryName, filename, depth);
		if(located) {
			return true;
		}
	}
	return false;
}


