/** @file map_object.h
 */

#ifndef BOMBIC_MAP_OBJECT_H_GUARD_
#define BOMBIC_MAP_OBJECT_H_GUARD_

#include <QString>
#include <QPixmap>
#include <QPointF>
#include <QSize>

class QGraphicsItem;

class BombicMapObject {

	public:
		BombicMapObject(const QString & name, const QPixmap & pixmap);
		~BombicMapObject();

		enum Type {
			Floorobject, Wall, Box, Creature, Player
		};

		virtual BombicMapObject::Type type() =0;

		virtual QGraphicsItem * createGraphicsItem(
				const QPointF & position) =0;

		const QPixmap & pixmap();

		virtual const QSize & size();

		virtual bool canBeWith(BombicMapObject * object);

	protected:
		QString name_;
		QPixmap pixmap_;
		/// Velikost v polickach (kolik zabira policek).
		QSize size_;
};

#endif
