
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

/// Minimalni sirka palety.
#define MAP_OBJECT_PALETTE_MIN_WIDTH 240
/// Nevalidni index palety
/// (pouzivany pro index vybraneho objektu, kdyz neni zaadny vybrany).
#define PALETTE_INDEX_INVALID (-666)

SINGLETON_INIT(MapObjectPalette);

/**
 * @param parent rodicovsky widget
 */
MapObjectPalette::MapObjectPalette(QWidget * parent):
		QWidget(parent),
		selectedObjectIndex_(PALETTE_INDEX_INVALID) {
	SINGLETON_CONSTRUCT;
	tabsConstruct();

	signalMapper_ = new QSignalMapper(this);
	connect(signalMapper_, SIGNAL(mapped(int)),
		this, SLOT(objectButtonClicked(int)));

}

MapObjectPalette::~MapObjectPalette() {
	SINGLETON_DESTROY;
}

/** @details
 * Zkonstruuje taby palety a tlacitko pro nacitani.
 */
void MapObjectPalette::tabsConstruct() {

	QVBoxLayout * layout = new QVBoxLayout(this);

	tabs_.widget = new QTabWidget(this);
	tabsAddPage(wallPage, tr("Wall"));
	tabsAddPage(boxPage, tr("Box"));
	tabsAddPage(creaturePage, tr("Creature"));
	tabsAddPage(floorobjectPage, tr("Floorobject"));
	tabs_.widget->setMinimumWidth(MAP_OBJECT_PALETTE_MIN_WIDTH);
	layout->addWidget(tabs_.widget);

	QPushButton * loadButton =
		new QPushButton(tr("Load new map object"), this);
	connect(loadButton, SIGNAL(clicked()),
		RESOURCE_HANDLER, SLOT(loadMapObject()));
	layout->addWidget(loadButton);
}

/** @details
 * Prida stranku (tab), zkonstruuje jeji vnitrnosti
 * potrebne pro spravne scrollovani a skladani tlacitek objektu.
 * @param pageIndex index pridavane stranky
 * @param tabLabel popisek pridavaneho tabu
 */
void MapObjectPalette::tabsAddPage(MapObjectPalette::Page pageIndex,
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

/** @details
 * Najde podle jmena @p objectName objekt v palete.
 * Nalezeny objekt vraci, ale zustava vlastnikem tohoto objektu,
 * pro ziskani vlastniho objektu viz BombicMapObject::createCopy().
 * @param objectName jmeno objektu
 * @return Pointer na nalezeny objekt.
 * @retval 0 objekt nenalezen
 */
BombicMapObject * MapObjectPalette::getObject(const QString & objectName) {
	if(objectIndexesByName_.contains(objectName)) {
		return objectPalette_.at(
			objectIndexesByName_.value(objectName) );
	} else {
		return 0;
	}
}

/** @details
 * Vlozi objekt @p object do odpovidajici stranky palety.
 * Prebira vlastnictvi objektu.
 * @param object pointer na vkladany objekt
 */
void MapObjectPalette::addObject(BombicMapObject * object) {
	// add object to the pallete
	int objectPaletteIndex = objectPalette_.size();
	objectPalette_.append(object);
	objectIndexesByName_.insert(
		object->name(), objectPaletteIndex);

	// find the page
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
		case BombicMapObject::Player:
			// nothing to do with player
			// user cannot see it as button in palette
			return;
		default:
			Q_ASSERT_X(false, "MapObjectPalette::addObject",
				"adding unexpected object type");
			return;
	}

	// create object button
	QPushButton * objectButton = new QPushButton;
	objectButton->setCheckable(true);

	QIcon buttonIcon(object->pixmap());
	objectButton->setIcon(buttonIcon);
	objectButton->setIconSize(object->pixmap().size());

	connect(objectButton, SIGNAL(clicked()),
		signalMapper_, SLOT(map()));
	signalMapper_->setMapping(objectButton, objectPaletteIndex);

	// add button to the tab page
	tabs_.layouts.value(page)->addWidget(objectButton);
	tabs_.widget->setCurrentIndex(page);
	// and select it to future use
	selectObject(objectPaletteIndex);
	objectButton->setChecked(true);
}

/** @details
 * Reakce na stisk tlacitka objektu.
 * Vybere, nebo zrusi vyber objektu.
 * @param objectPaletteIndex index do palety objektu
 */
void MapObjectPalette::objectButtonClicked(int objectPaletteIndex) {
	QPushButton * button = getObjectButton(objectPaletteIndex);

	if(button->isChecked()) {
		selectObject(objectPaletteIndex);
	} else {
		unselectObject();
	}
}

/** @details
 * Zrusi zmacknuti tlacitka pokud je nejaky obejkt vybrany.
 * Evokuje signal objectSelected().
 * @param objectPaletteIndex index do palety objektu
 */
void MapObjectPalette::selectObject(int objectPaletteIndex) {
	if(selectedObjectIndex_!=PALETTE_INDEX_INVALID) {
		// unselect old selected object
		getObjectButton(selectedObjectIndex_)
			->setChecked(false);
	}
	selectedObjectIndex_ = objectPaletteIndex;
	emit objectSelected(objectPalette_[selectedObjectIndex_]);
}

/**
 * @param objectName jmeno objektu pro vyber
 */
void MapObjectPalette::selectObject(const QString & objectName) {
	if(!objectIndexesByName_.contains(objectName)) {
		// cant select object outside palette
		return;
	}
	int objectPaletteIndex =
		objectIndexesByName_.value(objectName);
	QPushButton * button =
		getObjectButton(objectPaletteIndex);
	if(!button) {
		// cant select object without button
		return;
	}

	selectObject(objectPaletteIndex);
	button->setChecked(true);
}

/** @details
 * Pokud je nejaky objekt vybran, zrusi vyber a
 * evokuje signal objectUnselected().
 */
void MapObjectPalette::unselectObject() {
	if(selectedObjectIndex_==PALETTE_INDEX_INVALID) {
		return;
	}

	getObjectButton(selectedObjectIndex_)->setChecked(false);

	selectedObjectIndex_ = PALETTE_INDEX_INVALID;
	emit objectUnselected();
}

/**
 * @param objectPaletteIndex index do palety objektu
 * @return Pointer na tlacitko odpovidajici objektu.
 */
QPushButton * MapObjectPalette::getObjectButton(int objectPaletteIndex) {
	return static_cast<QPushButton *>(
		signalMapper_->mapping(objectPaletteIndex) );
}

