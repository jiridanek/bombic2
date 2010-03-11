/** @file wall.h
 */

#ifndef BOMBIC_WALL_H_GUARD_
#define BOMBIC_WALL_H_GUARD_

#include <QString>
#include <QPixmap>

#include "map_object.h"

class BombicWall: public BombicMapObject {

	public:
		BombicWall(const QString & name, const QPixmap & pixmap);

		virtual BombicMapObject::Type type();

};

#endif
