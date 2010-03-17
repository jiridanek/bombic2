
#include <QString>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>

#include "zoomwidget.h"

ZoomWidget::ZoomWidget(qreal zoomStep, qreal minValue, qreal maxValue,
		QWidget * parent):
				QWidget(parent), unit_(zoomStep) {

	// label
	label_ = new QLabel;
	label_->setAlignment(Qt::AlignHCenter);

	// reset button
	resetButton_ = new QPushButton(tr("Reset zoom"));
	connect(resetButton_, SIGNAL(clicked()),
		this, SLOT(reset()));

	// the slider
	slider_ = new QSlider(Qt::Horizontal);
	connect(slider_, SIGNAL(valueChanged(int)),
		this, SLOT(change(int)) );
	slider_->setRange(
		zoom2sliderValue(minValue), zoom2sliderValue(maxValue));
	slider_->setValue(zoom2sliderValue(1));

	slider_->setTickPosition(QSlider::TicksBelow);
	slider_->setTickInterval(1);
	slider_->setSingleStep(1);
	slider_->setPageStep(1);

	QGridLayout * layout = new QGridLayout(this);
	layout->addWidget(slider_, 0, 0, 2, 1);
	layout->addWidget(resetButton_, 1, 1);
	layout->addWidget(label_, 0, 1);
}

void ZoomWidget::change(int sliderValue) {
	qreal zoomValue = slider2zoomValue(sliderValue);
	emit zoomChanged(zoomValue);
	label_->setText( QString::number(qRound(zoomValue*100))+" %");
}
void ZoomWidget::reset() {
	slider_->setValue(zoom2sliderValue(1));
}


qreal ZoomWidget::slider2zoomValue(int sliderValue) {
	return sliderValue/100.0/unit_;
}
int ZoomWidget::zoom2sliderValue(qreal zoomValue) {
	return qRound(zoomValue*100*unit_);
}
