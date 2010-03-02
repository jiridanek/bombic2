
#include <QPushButton>
#include <QVBoxLayout>
#include <QIcon>
#include <QScrollArea>

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
	connect(signalMapper_, SIGNAL(mapped(int)),
		this, SLOT(objectButtonToggled(int)));
}

void MapObjectPalette::addPage(MapObjectPalette::Pages pageIndex,
		const QString & tabLabel) {



	QWidget * page = new QWidget;
// 	QScrollArea * scrollArea = new QScrollArea(page);
// 	scrollArea->setWidgetResizable(false);
// 	new QVBoxLayout(page);

	QVBoxLayout * vLayout = new QVBoxLayout(page);
	QScrollArea * scrollArea = new QScrollArea(page);

	QVBoxLayout * scrollAreaVLayout = new QVBoxLayout;
	QWidget * scrollAreaWidgetContents = new QWidget;

	scrollAreaWidgetContents->setLayout(scrollAreaVLayout);
	scrollAreaVLayout->setSizeConstraint(QLayout::SetFixedSize);

	scrollArea->setWidget(scrollAreaWidgetContents);

	vLayout->addWidget(scrollArea);

	tabs_->insertTab(pageIndex, page, tabLabel);
	tabsWidgets_[pageIndex] = scrollAreaWidgetContents;

}

void MapObjectPalette::loadObject() {
	// TODO make this better using filechooser and xml parsing
	QPixmap sourcePixmap("../../common/img/maps/mapa0.bmp");
	QPixmap objectPixmap = sourcePixmap.copy(460, 1, 50, 100);
	int objectPaletteIndex = objectPalette_.size();
	objectPalette_.append(objectPixmap);

	QPushButton * objectButton = new QPushButton();
	objectButton->setCheckable(true);
	objectButton->setChecked(false);

	QIcon buttonIcon(objectPixmap);
	objectButton->setIcon(buttonIcon);
	objectButton->setIconSize(objectPixmap.size());
	objectButton->setMinimumSize(objectPixmap.size());

	connect(objectButton, SIGNAL(clicked()),
		signalMapper_, SLOT(map()));
	signalMapper_->setMapping(objectButton, objectPaletteIndex);

	tabs_->setCurrentIndex(walls);
	tabsWidgets_[walls]->layout()->addWidget(objectButton);
	tabsWidgets_[walls]->show();
}

#include <iostream>

void MapObjectPalette::objectButtonToggled(int objectPaletteIndex) {
	QPushButton * buttonWidget = static_cast<QPushButton *>(
		signalMapper_->mapping(objectPaletteIndex) );
	std::cout << objectPaletteIndex << ": " << buttonWidget->isChecked() << std::endl;

}







