
#include <QVBoxLayout>
#include <QScrollArea>
#include <QCheckBox>

#include <constants.h>

#include "map_field_view.h"

#include "map_field_view_object.h"
#include "map_scene.h"
#include "bombic/map.h"
#include "generators/map_object_generator.h"
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
	scrollArea_->hide();

	layout->addWidget(scrollArea_);

	boxesWidget_ = new QWidget(this);
	boxesWidget_->hide();
	QLayout * boxesLayout = new QVBoxLayout(boxesWidget_);

	generateBox_ = new QCheckBox(tr("generate &Box"), this);
	boxesLayout->addWidget(generateBox_);
	generateCreature_ = new QCheckBox(tr("generate &Creature"), this);
	boxesLayout->addWidget(generateCreature_);
}

/** @details
 * Zobrazi detail aktualne vybraneho policka sceny mapy @p scene zobrazi.
 * Pokud vybrane policko (@c MapScene::selectedField_) neni v mape,
 * skryje pohled na policko.
 * @param scene scena mapy, jejiz policko chceme zobrazit
 */
void MapFieldView::update(MapScene * scene) {
	// disconnect old connections
	disconnectCheckboxes();
	// get field of map
	BombicMap * map = scene->map_;
	BombicMap::Field field = scene->selectedField_;
	if(!map->fieldsRect().contains(field)) {
		// the field in not in map
		// the current field view is not valid
		// and view of new field cannot be created
		scrollArea_->hide();
		return;
	}
	// connect checkboxes to new field
	connectCheckboxes(map, field);

	// create new clean scrolled widget
	QWidget * scrollWidget = new QWidget(this);
	QLayout * scrollLayout = new FlowLayout(scrollWidget);
	// add checkboxes of generators
	boxesWidget_->show();
	scrollLayout->addWidget(boxesWidget_);

	// add objects to scrolled widget
	foreach(BombicMapObject * mapObj, map->objectsOnField(field)) {
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
	scrollArea_->show();
}

/** @details
 * Odpoji oba checkboxy manipulujici s generatory objektu.
 */
void MapFieldView::disconnectCheckboxes() {
	generateBox_->disconnect();
	generateCreature_->disconnect();
}

/** @details
 * Pripoji oba checkboxy manipulujici s generatory objektu.
 * @param map mapa jejiz policko chceme propojit
 * @param field policko mapy, jehoz generatory objektu chceme pripojit
 */
void MapFieldView::connectCheckboxes(BombicMap * map,
		const BombicMap::Field & field) {
	connectCheckbox(generateBox_, map->boxGenerator(field));
	connectCheckbox(generateCreature_, map->creatureGenerator(field));
}

/** @details
 * Nastavi a pripoji @p checkbox na generator objektu @p objGen.
 * @param checkbox tlacitko manipulujici s generatorem objektu.
 * @param objGen generator objektu, ktery chceme napojit na tlacitko
 */
void MapFieldView::connectCheckbox(QCheckBox * checkbox,
		MapObjectGenerator * objGen) {
	checkbox->setChecked(objGen->allowed());
	connect(checkbox, SIGNAL(stateChanged(int)),
		objGen, SLOT(toggleAllowance()));
}
