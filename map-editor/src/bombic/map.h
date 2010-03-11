/** @file map.h
 */

#ifndef BOMBIC_MAP_H_GUARD_
#define BOMBIC_MAP_H_GUARD_

#include <QString>
#include <QList>
#include <QLinkedList>

class BombicMapBackground;
class BombicMapObject;

class BombicMap {

	public:
		BombicMap(int width, int height,
				BombicMapBackground * background);
		~BombicMap();

	private:
		QString name_;
		QString filename_;

		int width_;
		int height_;
		BombicMapBackground * background_;

		QList< QList< QLinkedList<BombicMapObject *> > > fields_;


};

#endif
