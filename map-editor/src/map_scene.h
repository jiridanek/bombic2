/** @file map_scene.h
 */

#ifndef MAP_SCENE_H_GUARD_
#define MAP_SCENE_H_GUARD_

#include <QGraphicsScene>

#include "bombic/map.h"

class BombicMapBackground;
class BombicMapObject;

class MapScene: public QGraphicsScene {

	Q_OBJECT

	public:
		MapScene(int width, int height,
				BombicMapBackground * background,
				QObject * parent = 0);
		~MapScene();

		void insert(BombicMapObject * object,
				const BombicMap::Field & dstField);

	public slots:
		void unsetWorkingObject();
		void setWorkingObject(BombicMapObject * object);

	protected:
		virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent);
		virtual void mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent);

	private:
		void insertBackgroundFields(const QPixmap & texture);
		void insertBackgroundWalls(BombicMapBackground * background);

		BombicMap * map_;

		BombicMapObject * workingObject_;


};

#endif
