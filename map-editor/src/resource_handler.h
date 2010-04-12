/** @file resource_handler.h
 * Definice singletonu ResourceHandler, ktery se stara vnejsi prostredky.
 * Prevadi vnejsi prostredky (data), do vnitrne pouzivanych objektu.
 */

#ifndef RESOURCE_HANDLER_H_GUARD_
#define RESOURCE_HANDLER_H_GUARD_

#include <QObject>
#include <QPixmap>
#include <QString>
#include <QDomElement>
#include <QDomNode>
#include <QDir>
#include <singleton.h>

/// Zkratka, pro pristup k singletonu ResourceHandler.
#define RESOURCE_HANDLER SINGLETON_SHORTCUT(ResourceHandler)


class BombicMap;
class BombicMapBackground;
class BombicMapObject;

class MapObjectResourceHandler;

/** Handler vnejsich prostredku.
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
		void saveMap(BombicMap * bombicMap);
		/// Ulozi mapu do noveho umisteni (vybere uzivatel)
		void saveMapAs(BombicMap * bombicMap);

		/// Nacte pozadi mapy podle jmena.
		BombicMapBackground * loadMapBackground(const QString & name);

	public slots:
		/// Nacte objekt mapy (vybrany uzivatelem).
		void loadMapObject();


	protected:
		/// Nacte mapu podle jmena.
		BombicMap * loadMap(const QString & name);
		/// Nacte zdi mapy.
		bool loadMapFloorobjects(const QDomElement & floorsEl,
				BombicMap * map);
		bool loadMapWalls(const QDomElement & wallsEl,
				BombicMap * map);
		bool loadMapBoxes(const QDomElement & boxesEl,
				BombicMap * map);
		bool loadMapCreatures(const QDomElement & creaturesEl,
				BombicMap * map);
		bool insertMapObjects(const QDomElement & positionEl,
				BombicMapObject * insertedObject, BombicMap * map);

		/// Nacte objekt mapy podle jmena.
		BombicMapObject * loadMapObject(const QString & name);

		/// Najde soubor podle jmena.
		static bool locateFile(QString & filename);
		/// Najde soubor v adresari a podadresarich.
		static bool locateFileInDir(const QDir & dir,
				QString & filename, int depth = 0);

		/// Nacte xml soubor podle jmena.
		static bool loadXml(const QString & name,
			QDomElement & rootEl, bool checkAttrName,
			const QString & rootElTagName = "");
		/// Prevede jmeno souboru na hodnotu predpokladanou v atributu name.
		static QString attrNameValueFromName(const QString & name);

		/// Nacte zdrojovy obrazek.
		bool loadSourcePixmap(const QDomElement & el,
				const QString & attrName = "src");

		/// Najde podelement podle jmena.
		static bool getSubElement(const QDomElement & el,
			QDomElement & subEl,
			const QString & subElTagName,
			bool successIfMissing = false);
		/// Precte stringovou hodnotu atributu.
		static bool getStringAttr(const QDomElement & el,
			QString & attr, const QString & attrName,
			bool successIfMissing = false);
		/// Precte integer hodnotu atributu.
		static bool getIntAttr(const QDomElement & el,
			int & attr, const QString & attrName,
			bool successIfMissing = false);
		/// Precte hodnotu atributu x,y.
		static bool getAttrsXY(const QDomElement & el, int & x, int & y);

		/// Zobrazi chybu.
		static void showError(const QString & message);
		/// Zobrazi chybu, ktera se vyskytla v elementu @p el.
		static void showError(const QString & message,
				const QDomElement & el,
				const QString & filename = "");
		/// Zobrazi chybu, ktera se vyskytla v souboru @p filename.
		static void showError(const QString & message,
				const QString & filename,
				const QDomElement & el = QDomElement());
		/// Zkostruuje cestu v xml souboru k elementu @p el.
		static QString nodePath(const QDomNode & el,
				const QString & delimiter = " > ");
		/// Zdrojovy obrazek.
		QPixmap sourcePixmap_;
		/// Jmeno zdrojoveho obrazku v sourcePixmap_.
		QString sourcePixmapName_;
};

#endif
