
#include "map_object_palette.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QIcon>
#include <QScrollArea>
#include <QBitmap>
#include <QTabWidget>
#include <QSignalMapper>
#include <QMessageBox>

#include <constants.h>

#include "qt/flowlayout.h"
#include "bombic/map_object.h"

#include "resource_handler.h"

#define MAP_OBJECT_PALETTE_MIN_WIDTH 240

#define PALETTE_INDEX_INVALID (-666)

SINGLETON_INIT(MapObjectPalette);


MapObjectPalette::MapObjectPalette(QWidget * parent):
		QWidget(parent),
		selectedObjectIndex_(PALETTE_INDEX_INVALID) {
	SINGLETON_CONSTRUCT;
	tabsConstruct_();
}

MapObjectPalette::~MapObjectPalette() {
	SINGLETON_DESTROY;
}


void MapObjectPalette::tabsConstruct_() {

	QVBoxLayout * layout = new QVBoxLayout(this);

	tabs_.widget = new QTabWidget(this);
	tabsAddPage_(wallPage, tr("Wall"));
	tabsAddPage_(boxPage, tr("Box"));
	tabsAddPage_(creaturePage, tr("Creature"));
	tabsAddPage_(floorobjectPage, tr("Floorobject"));
	tabs_.widget->setMinimumWidth(MAP_OBJECT_PALETTE_MIN_WIDTH);
	layout->addWidget(tabs_.widget);

	QPushButton * loadButton =
		new QPushButton(tr("Load new map object"), this);
	connect(loadButton, SIGNAL(clicked()),
		RESOURCE_HANDLER, SLOT(loadMapObject()));
	layout->addWidget(loadButton);

	signalMapper_ = new QSignalMapper(this);
	connect(signalMapper_, SIGNAL(mapped(int)),
		this, SLOT(objectButtonClicked(int)));

}

void MapObjectPalette::tabsAddPage_(MapObjectPalette::Page pageIndex,
		const QString & tabLabel) {

	QWidget * page = new QWidget;

	QVBoxLayout * pageLayout = new QVBoxLayout(page);
	QScrollArea * scrollArea = new QScrollArea(page);
	scrollArea->setWidgetResizable(true);

	QWidget * scrollAreaWidget = new QWidget;
	FlowLayout * scrollAreaLayout = new FlowLayout(scrollAreaWidget);

	scrollArea->setWidget(scrollAreaWidget);
	pageLayout->addWidget(scrollArea);

	tabs_.widget->insertTab(pageIndex, page, tabLabel);
	tabs_.layouts.insert(pageIndex, scrollAreaLayout);
}

BombicMapObject * MapObjectPalette::getObject(const QString & objectName) {
	if(objectIndexesByName_.contains(objectName)) {
		return objectPalette_.at(
			objectIndexesByName_.value(objectName) );
	} else {
		return 0;
	}
}

void MapObjectPalette::addObject(BombicMapObject * object) {
	Page page;
	switch(object->type()) {
		case BombicMapObject::Wall:
			page = wallPage;
			break;
		case BombicMapObject::Box:
			page = boxPage;
			break;
		case BombicMapObject::Creature:
			page = creaturePage;
			break;
		case BombicMapObject::Floorobject:
			page = floorobjectPage;
			break;
		default:
			QMessageBox::critical(this,
				tr("Error in Bombic object palette"),
				tr("Object has unexpected type and cannot"
					" be added to the object palette.")+"\n"+
				tr("It's going to be deleted.") );
			delete object;
			return;
	}

	QIcon buttonIcon(object->pixmap());

	QPushButton * objectButton = new QPushButton;
	objectButton->setCheckable(true);

	objectButton->setIcon(buttonIcon);
	objectButton->setIconSize(object->pixmap().size());

	int objectPaletteIndex = objectPalette_.size();
	objectPalette_.append(object);
	objectIndexesByName_.insert(
		object->name(), objectPaletteIndex);

	connect(objectButton, SIGNAL(clicked()),
		signalMapper_, SLOT(map()));
	signalMapper_->setMapping(objectButton, objectPaletteIndex);

	tabs_.layouts.value(page)->addWidget(objectButton);
	tabs_.widget->setCurrentIndex(page);

	selectObject(objectPaletteIndex);
	objectButton->setChecked(true);
}

void MapObjectPalette::objectButtonClicked(int objectPaletteIndex) {
	QPushButton * button = getObjectButton(objectPaletteIndex);

	if(button->isChecked()) {
		selectObject(objectPaletteIndex);
	} else {
		unselectObject();
	}
}

void MapObjectPalette::selectObject(int objectPaletteIndex) {
	if(selectedObjectIndex_!=PALETTE_INDEX_INVALID) {
		getObjectButton(selectedObjectIndex_)
			->setChecked(false);
	}
	selectedObjectIndex_ = objectPaletteIndex;
	emit objectSelected(objectPalette_[selectedObjectIndex_]);
}

void MapObjectPalette::selectObject(const QString & objectName) {
	if(objectIndexesByName_.contains(objectName)) {
		int objectPaletteIndex =
			objectIndexesByName_.value(objectName);
		selectObject(objectPaletteIndex);
		getObjectButton(objectPaletteIndex)
			->setChecked(true);
	}
}

void MapObjectPalette::unselectObject() {
	if(selectedObjectIndex_==PALETTE_INDEX_INVALID) {
		return;
	}

	getObjectButton(selectedObjectIndex_)->setChecked(false);

	selectedObjectIndex_ = PALETTE_INDEX_INVALID;
	emit objectUnselected();
}

QPushButton * MapObjectPalette::getObjectButton(int objectPaletteIndex) {
	return static_cast<QPushButton *>(
		signalMapper_->mapping(objectPaletteIndex) );
}
