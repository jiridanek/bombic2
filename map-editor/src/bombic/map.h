/** @file map.h
 */

#ifndef BOMBIC_MAP_H_GUARD_
#define BOMBIC_MAP_H_GUARD_

#include <QString>
#include <QList>
#include <QLinkedList>

class BombicMapObject;

class BombicMap {

	public:
		BombicMap();
		~BombicMap();

	private:
		QString name_;
		QString filename_;
		QList< QList< QLinkedList<BombicMapObject *> > > fields_;


};

#endif
