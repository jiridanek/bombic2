/** @file map_object.h
 */

#ifndef BOMBIC_MAP_OBJECT_H_GUARD_
#define BOMBIC_MAP_OBJECT_H_GUARD_

#include <QString>
#include <QPixmap>

class BombicMapObject {

	public:
		BombicMapObject();
		~BombicMapObject();

	private:
		QString name_;
		QPixmap pixmap_;

};

#endif
