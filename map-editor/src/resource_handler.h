/** @file resource_handler.h TODO
 * Definice singletonu ResourceHandler, ktery se stara vnejsi prostredky.
 * Prevadi vnejsi prostredky (data), do vnitrne pouzivanych objektu.
 */

#ifndef RESOURCE_HANDLER_H_GUARD_
#define RESOURCE_HANDLER_H_GUARD_

#include <QObject>
#include <QPixmap>
#include <QString>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QDir>
#include <singleton.h>

/// Zkratka, pro pristup k singletonu ResourceHandler.
#define RESOURCE_HANDLER SINGLETON_SHORTCUT(ResourceHandler)


class BombicMap;
class BombicMapBackground;
class BombicMapObject;

class MapResourceHandler;
class MapObjectResourceHandler;

/** Handler vnejsich prostredku. TODO
 * ResourceHandler obstarava vnejsi prostredky (data)
 * a prevadi je na vnitrne pouzivane objekty.
 * Objekty registruje na prislusnych mistech.
 * Konstrukci rozlicnych objektu mapy prenechava
 * specializovanym resource handlerum, kterym umoznuje
 * pouzivat privatni metody pro zpracovani prostredku.
 * Mezi hlavni casti patri:
 * - hledani souboru
 * - parsovani xml
 * - nacitaci obrazku
 * - konstrukce objektu ze ziskanych dat
 * - hlaseni chyb
 */
class ResourceHandler: public QObject {

	Q_OBJECT

	friend class MapResourceHandler;
	friend class MapObjectResourceHandler;

	SINGLETON_DEFINITION(ResourceHandler)

	public:
		/// Konstruuje ResourceHandler.
		ResourceHandler(QObject * parent = 0);
		/// Destruuje ResourceHandler.
		~ResourceHandler();

		/// Nacte mapu (vybranou uzivatelem).
		BombicMap * loadMap();
		/// Nacte prazdnou mapu s defaultnim pozadim.
		BombicMap * loadEmptyMap();
		/// Ulozi mapu (do puvodniho umisteni).
		bool saveMap(BombicMap * map);
		/// Ulozi mapu do noveho umisteni (vybere uzivatel).
		bool saveMapAs(BombicMap * map);

		/// Nacte pozadi mapy podle jmena.
		BombicMapBackground * loadMapBackground(const QString & name);

	public slots:
		/// Nacte objekt mapy (vybrany uzivatelem).
		void loadMapObject();


	protected:
		/// Nacte mapu podle jmena.
		BombicMap * loadMap(const QString & name);

		/// Nacte objekt mapy podle jmena.
		BombicMapObject * loadMapObject(const QString & name);

		/// Nacte zdrojovy obrazek.
		bool loadSourcePixmap(const QDomElement & el,
			const QString & attrName = "src");

		/// Zdrojovy obrazek.
		QPixmap sourcePixmap_;
		/// Jmeno zdrojoveho obrazku v sourcePixmap_.
		QString sourcePixmapName_;

		/// Resource handler obstaravajici mapu.
		MapResourceHandler * mapResourceHandler_;
};

namespace ResourceHandlerNS {

	inline QString tr(const char * sourceText,
			const char * comment = 0, int n = -1) {
		return QObject::tr(sourceText, comment, n);
	}

	/// Najde soubor podle jmena.
	bool locateFile(QString & filename);
	/// Najde soubor v adresari a podadresarich.
	bool locateFileInDir(const QDir & dir,
			QString & filename, int depth = 0);

	/// Nacist xml soubor podle jmena.
	bool loadXml(const QString & name,
			QDomElement & rootEl, bool checkAttrName,
			const QString & rootElTagName = "");
	/// Ulozit xml dokument.
	bool saveXml(const QString & filename,
			const QDomDocument & doc);
	/// Prevede obecne jmeno na hodnotu predpokladanou v atributu name.
	QString attrNameValueFromName(const QString & name);
	/// Prevede obecne jmeno na jmeno souboru.
	QString filenameFromName(const QString & name);

	/// Najde podelement podle jmena.
	bool getSubElement(const QDomElement & el,
		QDomElement & subEl,
		const QString & subElTagName,
		bool successIfMissing = false);
	/// Precte stringovou hodnotu atributu.
	bool getStringAttr(const QDomElement & el,
		QString & attr, const QString & attrName,
		bool successIfMissing = false);
	/// Precte integer hodnotu atributu.
	bool getIntAttr(const QDomElement & el,
		int & attr, const QString & attrName,
		bool successIfMissing = false);
	/// Precte hodnotu atributu x,y.
	bool getAttrsXY(const QDomElement & el, int & x, int & y);

	/// Zobrazi chybu.
	void showError(const QString & message);
	/// Zobrazi chybu, ktera se vyskytla v elementu @p el.
	void showError(const QString & message,
			const QDomElement & el,
			const QString & filename = "");
	/// Zobrazi chybu, ktera se vyskytla v souboru @p filename.
	void showError(const QString & message,
			const QString & filename,
			const QDomElement & el = QDomElement());
	/// Zkostruuje cestu v xml souboru k elementu @p el.
	QString nodePath(const QDomNode & el,
			const QString & delimiter = " > ");

}

#endif
