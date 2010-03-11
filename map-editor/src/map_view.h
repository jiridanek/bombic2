/** @file map_view.h
 */

#ifndef MAP_VIEW_H_GUARD_
#define MAP_VIEW_H_GUARD_

#include <QGraphicsScene>
#include <singleton.h>

#define MAP_VIEW SINGLETON_SHORTCUT(MapView)

class QGraphicsView;
class BombicMap;
class BombicMapObject;

class MapView: public QGraphicsScene {

	Q_OBJECT

	SINGLETON_DEFINITION(MapView)

	public:
		MapView(QObject * parent = 0);
		~MapView();

		QGraphicsView * getViewport();

	protected:
		virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent);
		virtual void mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent);

	private:
		QGraphicsView * viewport_;
		BombicMap * map_;


};

#endif
