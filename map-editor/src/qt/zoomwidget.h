/** @file zoomwidget.h
 */

#ifndef QT_ZOOM_WIDGET_H_GUARD_
#define QT_ZOOM_WIDGET_H_GUARD_

#include <QWidget>

class QSlider;
class QLabel;
class QPushButton;

class ZoomWidget: public QWidget {

	Q_OBJECT

	public:
		ZoomWidget(qreal zoomStep, qreal minValue, qreal maxValue,
				QWidget * parent = 0);

	signals:
		void zoomChanged(qreal zoomQuotient);

	protected slots:
		void change(int sliderValue);
		void reset();

	protected:
		qreal slider2zoomValue(int sliderValue);
		int zoom2sliderValue(qreal zoomValue);

	private:
		QSlider * slider_;
		QLabel * label_;
		QPushButton * resetButton_;

		qreal unit_;
};

#endif
