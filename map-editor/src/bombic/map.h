/** @file map.h
 */

#ifndef BOMBIC_MAP_H_GUARD_
#define BOMBIC_MAP_H_GUARD_

#include <QString>
#include <QVector>
#include <QPoint>
#include <QRect>
#include <QLinkedList>

class BombicMapBackground;
class BombicMapObject;
class BombicGeneratedObject;

class BombicMap {

	public:
		BombicMap(int width, int height,
				BombicMapBackground * background);
		~BombicMap();

		typedef QPoint Field;

		bool canInsert(BombicMapObject * object,
				const BombicMap::Field & dstField);

		void insert(BombicMapObject * object,
				const BombicMap::Field & dstField);
		void remove(BombicMapObject * object);

		BombicMapObject * objectOnTop(const BombicMap::Field & field);

		BombicGeneratedObject * generatedBox(const BombicMap::Field & field);
		BombicGeneratedObject * generatedCreature(const BombicMap::Field & field);

		const QRect & fieldsRect();

	private:
		QString name_;
		QString filename_;

		QRect fieldsRect_;
		BombicMapBackground * background_;

		typedef struct {
			BombicGeneratedObject * genBox;
			BombicGeneratedObject * genCreature;
			QLinkedList<BombicMapObject *> objList;
		} FieldSetT;

		typedef QVector< QVector< FieldSetT > > FieldsT;
		FieldsT fields_;


};

#endif
