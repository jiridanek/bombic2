/** @file map_scene.h
 */

#ifndef MAP_SCENE_H_GUARD_
#define MAP_SCENE_H_GUARD_

#include <QGraphicsScene>

#include "bombic/map.h"

class QGraphicsRectItem;
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
		void remove(BombicMapObject * object);

	public slots:
		void setWorkingObject(BombicMapObject * object);
		void unsetWorkingObject();
		void hideWorkingObject();

	protected:
		virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
		virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
		virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
		virtual void dragEnterEvent(QGraphicsSceneDragDropEvent * event);
		virtual void dragMoveEvent(QGraphicsSceneDragDropEvent * event);
		virtual void dragLeaveEvent(QGraphicsSceneDragDropEvent * event);
		virtual void dropEvent(QGraphicsSceneDragDropEvent * event);

	private:
		BombicMap::Field getEventField(
				QGraphicsSceneMouseEvent * mouseEvent);
		void insertBackgroundFields(const QPixmap & texture);
		void insertBackgroundWalls(BombicMapBackground * background);

		BombicMap * map_;

		BombicMapObject * workingObject_;

		QGraphicsRectItem * cantInsertItem_;

};

#endif
