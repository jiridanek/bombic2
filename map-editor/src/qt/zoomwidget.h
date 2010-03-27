/** @file zoomwidget.h
 * Widget, ktery predstavuje zoomovaci nastroj.
 */

#ifndef QT_ZOOM_WIDGET_H_GUARD_
#define QT_ZOOM_WIDGET_H_GUARD_

#include <QWidget>

class QSlider;
class QLabel;
class QPushButton;

/** Widget, ktery predstavuje zoomovaci nastroj.
 * Zapouzdreni nekolik propojenych widgetu.
 * V obecnych jednotkach pracuje se skalou, lze jej resetovat.
 */
class ZoomWidget: public QWidget {

	Q_OBJECT

	public:
		/// Konstruuje widget.
		ZoomWidget(qreal zoomStep, qreal minValue, qreal maxValue,
				QWidget * parent = 0);

	signals:
		/// Zoom zmenen.
		void zoomChanged(qreal zoomQuotient);
	public slots:
		/// Nastavit vychozi zoom.
		void reset();
	protected slots:
		/// Zmenit zoom.
		void change(int sliderValue);

	protected:
		/// Prevod jednotek slideru na obecne jednotky.
		qreal slider2zoomValue(int sliderValue);
		/// Prevod obecnych jednoteck na jednotky slideru.
		int zoom2sliderValue(qreal zoomValue);

	private:
		/// Jezdec, posuvnik po skale. 
		QSlider * slider_;
		/// Popisek s hodnotou zoomu.
		QLabel * label_;
		/// Resetovaci tlacitko.
		QPushButton * resetButton_;
		
		/// Jednotka (pocet obecnych jednotek na jednotku slideru).
		qreal unit_;
};

#endif
