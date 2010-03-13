/** @file map_scene.h
 */

#ifndef MAP_SCENE_H_GUARD_
#define MAP_SCENE_H_GUARD_

#include <QGraphicsScene>

class BombicMap;
class BombicMapBackground;

class MapScene: public QGraphicsScene {

	Q_OBJECT

	public:
		MapScene(int width, int height,
				BombicMapBackground * background,
				QObject * parent = 0);
		~MapScene();

	protected:
		virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent);
		virtual void mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent);

	private:

		BombicMap * map_;


};

#endif
