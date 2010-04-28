/** @file resource_handler.h
 * Definice singletonu @c ResourceHandler, ktery se stara o vnejsi prostredky.
 * Prevadi vnejsi prostredky (data), do vnitrne pouzivanych objektu.
 * Vyuziva k tomu funkce definovane v prostoru jmen @c ResourceHandlerFunctions
 * a dalsi specializovane resource handlery jako napriklad MapResourceHandler
 * nebo system potomku od MapObjectResourceHandler.
 */

#ifndef RESOURCE_HANDLER_H_GUARD_
#define RESOURCE_HANDLER_H_GUARD_

#include <QObject>
#include <QPixmap>
#include <QString>
#include <QDomElement>
#include <singleton.h>

/// Zkratka, pro pristup k singletonu ResourceHandler.
#define RESOURCE_HANDLER SINGLETON_SHORTCUT(ResourceHandler)

class BombicMap;
class BombicMapBackground;
class BombicMapObject;

class MapResourceHandler;
class MapObjectResourceHandler;

/** Handler vnejsich prostredku.
 * ResourceHandler obstarava vnejsi prostredky (data)
 * a prevadi je na vnitrne pouzivane objekty.
 * Objekty mapy registruje v palete objektu,
 * jejich konstrukci vsak prenechava specializovanym resource handlerum.
 * Poskytuje hlavnim castem programu takovou miru abstrakce,
 * ze mimo system resource handleru neni treba pracovat s daty na disku.
 * Vyuziva funkce z namespace @c ResourceHandlerFunctions.
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

#endif
