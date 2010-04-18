/** @file map_view.h
 * Pohled na mapu MapView je widget,
 * ktery zastresuje praci s mapou.
 */

#ifndef MAP_VIEW_H_GUARD_
#define MAP_VIEW_H_GUARD_

#include <QGraphicsView>
#include <QList>
#include <QRectF>
#include <singleton.h>

/// Zkratka pro ziskani instance singletonu MapView.
#define MAP_VIEW SINGLETON_SHORTCUT(MapView)

class QEvent;
class QLabel;
class QMimeData;
class ZoomWidget;
class MapScene;
class MapFieldView;
class BombicMapBackground;
class BombicMapObject;

/** Pohled na mapu a prilehle widgety.
 * Centralnim prvkem je QGraphicsView, ktere zobrazuje mapu.
 * Dale jsou pripojeny widgety pro interakci s uzivatelem.
 * Pohled lze zoomovat, zobrazuje aktualni pracovni objekt
 * a detail vybraneho policka.
 * V ramci MapView je podporovan drag and drop na objekty mapy.
 */
class MapView: public QGraphicsView {

	Q_OBJECT

	SINGLETON_DEFINITION(MapView)


	public:
		/// Zkonstruuje pohled.
		explicit MapView(QWidget * parent = 0);
		/// Destruuje pohled.
		~MapView();

		/// Zobrazit obrazek pracovniho objektu.
		void showWorkingObjectLabel(BombicMapObject * object);
		/// Schovat obrazek pracovniho objektu.
		void hideWorkingObjectLabel();

		/// Obnovi policko v detailnim zobrazeni.
		void updateFieldView();

		/// Vytvorit mime data, ktera prenasi objekt.
		static QMimeData * createMimeData(BombicMapObject * object);
		/// Ziskat objekt z mime dat, ktera ho prenasi.
		static BombicMapObject * getMapObject(
				const QMimeData * mimeData);
		/// Spustit tazeni.
		static void execDragging(QWidget * srcWidget,
				BombicMapObject * object);
	public slots:
		/// Nastavi zoom mapy.
		void setZoom(qreal zoomQuotient);
		/// Obnovit oblast sceny.
		void updateScene(const QList<QRectF> & rects);

	signals:
		/// Pohled opusten.
		void leaved();

	protected:
		/// Handler oppusteni pohledu.
		virtual void leaveEvent(QEvent * event);

	private:
		/// Scena mapy.
		MapScene * scene_;
		/// Aktualni (posledni nastaveny) zoom.
		qreal lastZoomQuotient_;
		/// Zoomovaci widget.
		ZoomWidget * zoomWidget_;
		/// Obrazek pracovniho objektu.
		QLabel * workingObjectLabel_;
		/// Detailni pohled na jedno policko mapy.
		MapFieldView * fieldView_;
};

#endif
