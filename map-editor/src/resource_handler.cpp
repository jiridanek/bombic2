
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

void ResourceHandler::loadMapObject() {
	QString filename = QFileDialog::getOpenFileName(
		MAP_OBJECT_PALETTE, tr("Map object file"), "",
		tr("Map object files")+" (*"XML_FILE_EXTENSION")" );
	if(!filename.isEmpty()) {
		loadMapObject(filename);
	}
}

BombicMapObject * ResourceHandler::loadMapObject(const QString & name) {
	QString objectName = attrNameValueFromName(name);
	BombicMapObject * obj = MAP_OBJECT_PALETTE->getObject(objectName);
	if(obj) {
		return obj;
	}

	QDomElement rootEl;
	if(!loadXml(name, rootEl, true)) {
		return 0;
	}
	MapObjectResourceHandler * objRH =
		MapObjectResourceHandler::createResourceHandler(rootEl);
	if(!objRH) {
		showError(
			tr("Don't know which resource handler create.")+"\n"+
			tr("The object with root element")+" "+
			rootEl.tagName()+" "+
			tr("may be wrong or not supported.") );
		return 0;
	}
	if(!loadSourcePixmap(rootEl)) {
		delete objRH;
		return 0;
	}
	obj = objRH->createMapObject(rootEl);
	delete objRH;
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
