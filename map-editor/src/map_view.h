/** @file map_view.h
 */

#ifndef MAP_VIEW_H_GUARD_
#define MAP_VIEW_H_GUARD_

#include <QWidget>
#include <singleton.h>

#define MAP_VIEW SINGLETON_SHORTCUT(MapView)

class QGraphicsView;
class QGridLayout;
class QSlider;
class MapScene;
class BombicMapBackground;

class MapView: public QWidget {

	Q_OBJECT

	SINGLETON_DEFINITION(MapView)

	public slots:
		void setZoom(int zoom);

	public:
		explicit MapView(QWidget * parent = 0);

		~MapView();

		QGridLayout * gridLayout();

	private:
		QGraphicsView * viewport_;
		MapScene * scene_;
		QSlider * zoomSlider_;
		// dalsi prvky jako fieldview, viewtoolbar

};

#endif
