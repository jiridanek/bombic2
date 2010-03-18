/** @file resource_handler.h
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

#define RESOURCE_HANDLER SINGLETON_SHORTCUT(ResourceHandler)


class BombicMap;
class BombicMapBackground;
class BombicMapObject;

class WallResourceHandler;

class ResourceHandler: public QObject {

	Q_OBJECT

	friend class WallResourceHandler;

	SINGLETON_DEFINITION(ResourceHandler)

	public:
		ResourceHandler(QObject * parent = 0);
		~ResourceHandler();

		BombicMap * loadMap();

		void saveMap(BombicMap * bombicMap);

		void saveMapAs(BombicMap * bombicMap);

		BombicMapBackground * loadMapBackground(const QString & name);

		BombicMapObject * loadMapObject();

		BombicMapObject * loadMapObject(const QString & name);

	protected:
		static bool locateFile(QString & filename);
		static bool locateFileInDir(const QDir & dir,
				QString & filename, int depth = 0);

		static bool loadXmlByName(const QString & name, QDomElement & rootEl,
			const QString & rootElTagName, bool checkAttrName);


		bool loadSourcePixmap(const QDomElement & el,
				const QString & attrName = "src");

		static bool getSubElement(const QDomElement & el,
			QDomElement & subEl,
			const QString & subElTagName = "img");

		static bool getIntAttr(const QDomElement & el,
			int & attr, const QString & attrName,
			bool successIfMissing = false);

		static bool getAttrsXY(const QDomElement & el, int & x, int & y);

		static void showError(const QString & message);

		static void showError(const QString & message,
				const QDomElement & el,
				const QString & filename = "");

		static void showError(const QString & message,
				const QString & filename,
				const QDomElement & el = QDomElement());

		static QString nodePath(const QDomNode & el,
				const QString & delimiter = " > ");
		QPixmap sourcePixmap_;
		QString sourcePixmapName_;
};

#endif
