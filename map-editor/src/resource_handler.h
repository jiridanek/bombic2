/** @file resource_handler.h
 */

#ifndef RESOURCE_HANDLER_H_GUARD_
#define RESOURCE_HANDLER_H_GUARD_

#include <QObject>
#include <QPixmap>
#include <QString>
#include <QDomElement>
#include <QDir>
#include <singleton.h>

#define RESOURCE_HANDLER SINGLETON_SHORTCUT(ResourceHandler)


class BombicMap;
class BombicMapBackground;
class BombicMapObject;

class ResourceHandler: public QObject {

	Q_OBJECT

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
		static bool openXmlByName(const QString & name, QDomElement & rootEl);

		static bool locateFile(QString & filename);
		static bool locateFileInDir(const QDir & dir,
				QString & filename, int depth = 0);

		bool openSourcePixmap(const QString & name);

		QPixmap sourcePixmap_;
		QString sourcePixmapName_;
};

#endif
