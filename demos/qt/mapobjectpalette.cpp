
#include <QPushButton>
#include <QVBoxLayout>

#include "mapobjectpalette.h"

#define MAP_OBJECT_PALETTE_MIN_WIDTH 270

MapObjectPalette::MapObjectPalette(QWidget * parent):
		QWidget(parent) {

	QVBoxLayout * layout = new QVBoxLayout(this);

	tabs_ = new QTabWidget(this);
	addPage(walls, tr("Walls"));
	addPage(boxes, tr("Boxes"));
	addPage(creatures, tr("Creatures"));
	addPage(floorobjects, tr("Floorobjects"));
	tabs_->setMinimumWidth(MAP_OBJECT_PALETTE_MIN_WIDTH);
	layout->addWidget(tabs_);

	QPushButton * loadButton = new QPushButton(tr("Load"), this);
	connect(loadButton, SIGNAL(clicked()), this, SLOT(loadObject()));
	layout->addWidget(loadButton);

	signalMapper_ = new QSignalMapper(this);
	connect(signalMapper_, SIGNAL(mapped(QWidget *)),
		this, SLOT(objectButtonToggled(QWidget *)));
}

void MapObjectPalette::addPage(MapObjectPalette::Pages pageIndex,
		const QString & tabLabel) {

	QWidget * page = new QWidget;
	new QVBoxLayout(page);


	tabs_->insertTab(pageIndex, page, tabLabel);
}

void MapObjectPalette::loadObject() {
	// TODO make this better using filechooser and xml parsing

	QPushButton * objectButton = new QPushButton();
	objectButton->setCheckable(true);
	objectButton->setChecked(false);
	connect(objectButton, SIGNAL(clicked()),
		signalMapper_, SLOT(map()));
	signalMapper_->setMapping(objectButton, objectButton);


	tabs_->setCurrentIndex(walls);
	tabs_->currentWidget()->layout()->addWidget(objectButton);

}

#include <iostream>
void MapObjectPalette::objectButtonToggled(QWidget * buttonWidget) {
	std::cout << ((void *) buttonWidget) << ": " << ((QPushButton *)buttonWidget)->isChecked() << std::endl;

}