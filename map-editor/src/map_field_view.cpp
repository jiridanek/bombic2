
#include <QVBoxLayout>
#include <QScrollArea>
#include <QCheckBox>

#include <constants.h>

#include "map_field_view.h"

#include "map_field_view_object.h"
#include "map_scene.h"
#include "bombic/map.h"
#include "bombic/map_object_generator.h"
#include "qt/flowlayout.h"

/** @details
 * Vytvori tlacitka generatoru objektu a skrolovaci pole.
 * @param parent rodicovsky widget
 */
MapFieldView::MapFieldView(QWidget * parent):
		QWidget(parent), oldScrollWidget_(0) {
	QLayout * layout = new QVBoxLayout(this);

	scrollArea_ = new QScrollArea(this);
	scrollArea_->setWidgetResizable(true);

	layout->addWidget(scrollArea_);
}

/** @details
 * Zobrazi detail aktualne vybraneho policka sceny mapy @p scene zobrazi.
 * Pokud vybrane policko (@c MapScene::selectedField_) neni v mape,
 * skryje pohled na policko.
 * @param scene scena mapy, jejiz policko chceme zobrazit
 */
void MapFieldView::update(MapScene * scene) {
	// get field of map
	BombicMap * map = scene->map_;
	BombicMap::Field field = scene->selectedField_;
	if(!map->fieldsRect().contains(field)) {
		// the field in not in map
		// the current field view is not valid
		// and view of new field cannot be created
		hide();
		return;
	}

	// create new clean scrolled widget
	QWidget * scrollWidget = new QWidget(this);
	QLayout * scrollLayout = new FlowLayout(scrollWidget);
	// add checkboxes of generators
	QWidget * boxesWidget = new QWidget(this);
	QLayout * boxesLayout = new QVBoxLayout(boxesWidget);
	boxesLayout->addWidget( createCheckbox(
		tr("generate &Box"), map->boxGenerator(field) ) );
	boxesLayout->addWidget( createCheckbox(
		tr("generate &Creature"), map->creatureGenerator(field) ) );
	scrollLayout->addWidget(boxesWidget);

	// add objects to scrolled widget
	foreach(BombicMapObject * mapObj,
			map->objectsOnField(field)) {
		scrollLayout->addWidget(
			new MapFieldViewObject(mapObj, this) );
	}
	// replace scrolled widget with newly created one
	if(oldScrollWidget_) {
		// first delete the old
		delete oldScrollWidget_;
	}
	oldScrollWidget_ = scrollArea_->takeWidget();
	scrollArea_->setWidget(scrollWidget);
	// and show it
	scrollWidget->show();
	show();
}

/** @details
 * Alokuje, nastavi a pripoji tlacitko na generator objektu @p objGen.
 * @param checkboxName popisek tlacitka
 * @param objGen generator objektu, ktery chceme napojit na tlacitko
 */
QCheckBox * MapFieldView::createCheckbox(const QString & checkboxName,
		BombicMapObjectGenerator * objGen) {
	QCheckBox * checkbox = new QCheckBox(checkboxName, this);
	checkbox->setChecked(objGen->allowed());
	connect(checkbox, SIGNAL(stateChanged(int)),
		objGen, SLOT(toggleAllowance()));
	return checkbox;
}
