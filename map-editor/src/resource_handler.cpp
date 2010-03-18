
#include <QLinkedList>
#include <QBitmap>
#include <QMessageBox>
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
		showError(tr("File")+" "+filename+" "+tr("didn't located"));
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

	if(rootEl.tagName()!=rootElTagName) {
		showError(tr("Wrong root element, it should be")+" "+
			rootElTagName, filename);
		return false;
	}
	if(checkAttrName && rootEl.attribute("name")!=name) {
		showError(tr("Wrong value of attribute name, it should be")+" "+
			name, rootEl, filename);
		return false;
	}

	return true;
}

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
		showError(tr("File")+" "+filename+" "+tr("didn't located"));
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

bool ResourceHandler::getSubElement(const QDomElement & el,
		QDomElement & subEl, const QString & subElTagName) {

	subEl = el.namedItem(subElTagName).toElement();
	if(subEl.isNull()) {
		showError(tr("Missing subelement")+" "+subElTagName, el);
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
			showError(tr("Missing attribute")+" "+attrName, el);
			return false;
		}
	}
	bool converted;
	int a = el.attribute(attrName).toInt(&converted);
	if(!converted) {
		showError(tr("Attribute")+" "+attrName+" "
			+tr("cannot be converted to integer"), el);
		return false;
	}

	attr = a;
	return true;
}

bool ResourceHandler::getAttrsXY(const QDomElement & el, int & x, int & y) {
	return getIntAttr(el, x, "x") && getIntAttr(el, y, "y");
}

void ResourceHandler::showError(const QString & message) {
	QMessageBox::critical(0, tr("Error in Bombic resource"),
		message, QMessageBox::Ok);
}

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

void ResourceHandler::showError(const QString & message,
		const QDomElement & el,
		const QString & filename) {
	showError(message, filename, el);
}

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
