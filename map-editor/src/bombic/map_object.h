/** @file map_object.h
 */

#ifndef BOMBIC_MAP_OBJECT_H_GUARD_
#define BOMBIC_MAP_OBJECT_H_GUARD_

#include <QString>
#include <QPixmap>
#include <QPointF>
#include <QRect>
#include <QSize>

#include "map.h"

class QGraphicsItem;
class QGraphicsPixmapItem;

class BombicMapObject {

	public:
		BombicMapObject(const QString & name, const QPixmap & pixmap);
		~BombicMapObject();

		virtual BombicMapObject * createCopy() =0;

		enum Type {
			Floorobject, Wall, Box, Creature, Player
		};

		virtual BombicMapObject::Type type() =0;

		virtual QGraphicsItem * graphicsItem();
		virtual QGraphicsItem * situateGraphicsItem(
				const QPointF & position) =0;

		const QString & name();

		const QPixmap & pixmap();

		const QRect & rect();
		BombicMap::Field field();
		void setField(const BombicMap::Field & field);
		QSize size();
		virtual int toplapping();

		virtual bool canBeWith(BombicMapObject * object);
		virtual bool canBeDragged();
		virtual bool canBeRemoved();

	protected:
		QString name_;
		QPixmap pixmap_;
		QGraphicsPixmapItem * graphicsItem_;
		/// Velikost v polickach (kolik zabira policek).
		QRect rect_;
		/// Pocet presahujicich policek (na ose y).
		int toplapping_;
};

#endif
