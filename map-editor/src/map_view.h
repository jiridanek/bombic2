/** @file map_view.h
 */

#ifndef MAP_VIEW_H_GUARD_
#define MAP_VIEW_H_GUARD_

#include <QWidget>
#include <QPixmap>
#include <singleton.h>

#define MAP_VIEW SINGLETON_SHORTCUT(MapView)

class QGraphicsView;
class QGridLayout;
class QEvent;
class QLabel;
class ZoomWidget;
class MapScene;
class BombicMapBackground;

class MapView: public QWidget {

	Q_OBJECT

	SINGLETON_DEFINITION(MapView)


	public:
		explicit MapView(QWidget * parent = 0);

		~MapView();

		QGridLayout * gridLayout();

		void showWorkingObjectLabel(const QPixmap & objectPixmap);
		void hideWorkingObjectLabel();

	public slots:
		void setZoom(qreal zoomQuotient);

	signals:
		void leaved();

	protected:
		virtual void leaveEvent(QEvent * event);

	private:
		QGraphicsView * viewport_;
		MapScene * scene_;
		ZoomWidget * zoomWidget_;
		qreal lastZoomQuotient_;
		QLabel * workingObjectLabel_;

		// dalsi prvky jako fieldview, viewtoolbar

};

#endif
