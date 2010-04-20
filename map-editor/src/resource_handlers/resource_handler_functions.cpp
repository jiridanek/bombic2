
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <constants.h>

#include "resource_handler_functions.h"

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
bool ResourceHandlerFunctions::locateFile(QString & filename) {
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
bool ResourceHandlerFunctions::locateFileInDir(const QDir & dir,
		QString & filename, int depth) {

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
bool ResourceHandlerFunctions::loadXml(const QString & name,
		QDomElement & rootEl, bool checkAttrName,
		const QString & rootElTagName) {

	// first locate the file
	QString filename = filenameFromName(name);
	bool fileLocated = locateFile(filename);
	if(!fileLocated) {
		return false;
	}
	// open it
	QFile file(filename);
	bool opened = file.open(QIODevice::ReadOnly | QIODevice::Text);
	if(!opened) {
		showError(filename+"\n"+tr("couldn't be opened for reading."));
		return false;
	}
	// and parse the xml
	QDomDocument doc;
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
	// get root element
	rootEl = doc.documentElement();

	// and provide some checking
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
 * Pokusi se otevrit a vyprazdnit soubor @p filename.
 * Pote do nej vypise xml dokument @p doc.
 * Pokud nastane chyba, sam zobrazuje relevantni informace.
 * @param filename cesta k souboru
 * @param doc xml dokument, ktery ma byt ulozen
 * @return Uspech operace.
 * @retval true dokument ulozen
 * @retval false dokument nelze ulozit
 */
bool ResourceHandlerFunctions::saveXml(const QString & filename,
		const QDomDocument & doc) {
	// open the file
	QFile file(filename);
	bool opened = file.open(
		QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text );
	if(!opened) {
		showError(filename+"\n"+tr("couldn't be opened for writing."));
		return false;
	}
	// and write the xml content to it
	QTextStream(&file)
		<< "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
		<< doc.toString();

	// I know, it would be closed automaticaly
	// but it is good habit visualy close the opened
	file.close();

	return true;
}

/** @details
 * Obecne jmeno muze byt jmeno objektu, souboru, nebo cela cesta.
 * Pokud obsahuje @p name priponu xml souboru nebo cestu,
 * tyto kusy se odstrani.
 * @param name jmeno objektu, souboru, nebo cela cesta
 * @return Predpokladane jmeno objektu.
 */
QString ResourceHandlerFunctions::attrNameValueFromName(const QString & name) {
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
 * Obecne jmeno muze byt jmeno objektu, souboru, nebo cela cesta.
 * V prvnim pripade se k @p name prida pripona xml souboru,
 * jinak vraci @p name.
 * @param name jmeno objektu, souboru, nebo cela cesta
 * @return Predpokladane jmeno souboru.
 */
QString ResourceHandlerFunctions::filenameFromName(const QString & name) {
	QString filename = name;
	if(!filename.endsWith(XML_FILE_EXTENSION)) {
		filename += XML_FILE_EXTENSION;
	}
	return filename;
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
bool ResourceHandlerFunctions::getSubElement(const QDomElement & el,
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
bool ResourceHandlerFunctions::getStringAttr(const QDomElement & el,
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
bool ResourceHandlerFunctions::getIntAttr(const QDomElement & el,
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
bool ResourceHandlerFunctions::getAttrsXY(const QDomElement & el, int & x, int & y) {
	return getIntAttr(el, x, "x") && getIntAttr(el, y, "y");
}

/** @details
 * Zobrazi dialogove okno se zpravou @p message.
 * @param message textovy  popis chyby
 * @see QMessageBox::critical()
 */
void ResourceHandlerFunctions::showError(const QString & message) {
	QMessageBox::critical(0, tr("Error in Bombic resource"),
		message, QMessageBox::Ok);
}

/** @details
 * Zobrazi chybove hlaseni korespondujici s urcitym souborem @p filename.
 * @param message textovy popis chyby
 * @param filename jmeno souboru, ve kterem se chyba vyskytla
 * @param el pripadne xml element, ve kterem chyba nastala
 */
void ResourceHandlerFunctions::showError(const QString & message,
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
void ResourceHandlerFunctions::showError(const QString & message,
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
QString ResourceHandlerFunctions::nodePath(const QDomNode & node,
		const QString & delimiter) {
	QString path = node.nodeName();
	for(QDomNode n = node.parentNode() ;
			!n.isNull() && n.nodeType()!=QDomNode::DocumentNode ;
			n = n.parentNode() ) {
		path = n.nodeName() + delimiter + path;
	}
	return path;
}
