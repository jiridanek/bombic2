/** @file resource_handler_functions.h
 *
 */

#ifndef RESOURCE_HANDLER_FUNCTIONS_H_GUARD_
#define RESOURCE_HANDLER_FUNCTIONS_H_GUARD_

#include <QObject>
#include <QPixmap>
#include <QString>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QDir>


/** Funkce pro resource handlery.
 * Nekolik funkci zamerenych na zpracovani XML, souboru na disku,
 * a hlaseni chyb.
 * Mezi hlavni casti patri:
 * - hledani souboru
 * - parsovani xml
 * - nacitaci obrazku
 * - hlaseni chyb
 */
namespace ResourceHandlerFunctions {

	/// Prelozit. @see QObject::tr()
	inline QString tr(const char * sourceText,
			const char * comment = 0, int n = -1) {
		return QObject::tr(sourceText, comment, n);
	}

	/// Najit soubor podle jmena.
	bool locateFile(QString & filename);
	/// Najit soubor v adresari a podadresarich.
	bool locateFileInDir(const QDir & dir,
			QString & filename, int depth = 0);

	/// Nacist xml soubor podle jmena.
	bool loadXml(const QString & name,
			QDomElement & rootEl, bool checkAttrName,
			const QString & rootElTagName = "");
	/// Ulozit xml dokument.
	bool saveXml(const QString & filename,
			const QDomDocument & doc);
	/// Prevest obecne jmeno na hodnotu predpokladanou v atributu name.
	QString attrNameValueFromName(const QString & name);
	/// Prevest obecne jmeno na jmeno souboru.
	QString filenameFromName(const QString & name);

	/// Najit podelement podle jmena.
	bool getSubElement(const QDomElement & el,
		QDomElement & subEl,
		const QString & subElTagName,
		bool successIfMissing = false);
	/// Precist stringovou hodnotu atributu.
	bool getStringAttr(const QDomElement & el,
		QString & attr, const QString & attrName,
		bool successIfMissing = false);
	/// Precist integer hodnotu atributu.
	bool getIntAttr(const QDomElement & el,
		int & attr, const QString & attrName,
		bool successIfMissing = false);
	/// Precist hodnotu atributu x,y.
	bool getAttrsXY(const QDomElement & el, int & x, int & y);

	/// Zobrazit chybu.
	void showError(const QString & message);
	/// Zobrazit chybu, ktera se vyskytla v elementu @p el.
	void showError(const QString & message,
			const QDomElement & el,
			const QString & filename = "");
	/// Zobrazit chybu, ktera se vyskytla v souboru @p filename.
	void showError(const QString & message,
			const QString & filename,
			const QDomElement & el = QDomElement());
	/// Zkostruovat cestu v xml souboru k elementu @p el.
	QString nodePath(const QDomNode & el,
			const QString & delimiter = " > ");

}

#endif
