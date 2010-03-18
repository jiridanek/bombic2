
#include "map_object_palette.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QIcon>
#include <QScrollArea>
#include <QBitmap>
#include <QTabWidget>
#include <QSignalMapper>

#include <constants.h>

#include "qt/flowlayout.h"
#include "bombic/map_object.h"

#include "resource_handler.h"

#define MAP_OBJECT_PALETTE_MIN_WIDTH 240

SINGLETON_INIT(MapObjectPalette);


MapObjectPalette::MapObjectPalette(QWidget * parent):
		QWidget(parent) {
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
// 	connect(signalMapper_, SIGNAL(mapped(int)),
// 		this, SLOT(objectButtonClicked(int)));


	for(int i = 1 ; i < 7 ; ++i) {

		// TODO make this better using filechooser and xml parsing
		QPixmap sourcePixmap("../common/img/maps/mapa0.bmp");
		QPixmap objectPixmap = sourcePixmap.copy(460, 1, (i-(i+3)%4)*CELL_SIZE, (2+i%2)*CELL_SIZE);

		objectPixmap.setMask(
			objectPixmap.createMaskFromColor(Qt::magenta));

		QPushButton * objectButton = new QPushButton();
		objectButton->setCheckable(true);
		objectButton->setChecked(false);

		QIcon buttonIcon(objectPixmap);
		objectButton->setIcon(buttonIcon);
		objectButton->setIconSize(objectPixmap.size());

		tabs_.layouts[wallPage]->addWidget(objectButton);
	}
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
	tabs_.layouts[pageIndex] = scrollAreaLayout;
}
