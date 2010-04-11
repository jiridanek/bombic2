
#include <QHBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QCheckBox>

#include <constants.h>

#include "map_field_view.h"

#include "map_field_view_object.h"
#include "map_scene.h"
#include "bombic/map.h"
#include "bombic/generated_object.h"

/** @details
 * Vytvori tlacitka generovanych objektu a skrolovaci pole.
 * @param parent rodicovsky widget
 */
MapFieldView::MapFieldView(QWidget * parent):
		QWidget(parent), oldScrollWidget_(0) {
	QGridLayout * grid = new QGridLayout(this);

	generateBox_ = new QCheckBox(tr("generate &Box"), this);
	grid->addWidget(generateBox_, 0, 0);
	generateCreature_ = new QCheckBox(tr("generate &Creature"), this);
	grid->addWidget(generateCreature_, 1, 0);

	scrollArea_ = new QScrollArea(this);
	scrollArea_->setWidgetResizable(true);
	scrollArea_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	grid->addWidget(scrollArea_, 0, 1, 2, 1);
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
	BombicMap::Field field = scene->selectedField_;
	if(!scene->map_->fieldsRect().contains(field)) {
		// the field in not in map
		// the current field view is not valid
		// and view of new field cannot be created
		hide();
		return;
	}
	// connect checkboxes to new field
	connectCheckboxes(scene->map_, field);

	// create new clean scrolled widget
	QWidget * scrollWidget = new QWidget(this);
	QHBoxLayout * scrollLayout = new QHBoxLayout(scrollWidget);
	scrollLayout->setAlignment(Qt::AlignLeft);
	// add objects to scrolled widget
	foreach(BombicMapObject * mapObj,
			scene->map_->objectsOnField(field)) {
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
 * Odpoji oba checkboxy manipulujici s generovanymi objekty.
 */
void MapFieldView::disconnectCheckboxes() {
	generateBox_->disconnect();
	generateCreature_->disconnect();
}

/** @details
 * Pripoji oba checkboxy manipulujici s generovanymi objekty.
 * @param map mapa jejiz policko chceme propojit
 * @param field policko mapy, jehoz generovane objekty chceme pripojit
 */
void MapFieldView::connectCheckboxes(BombicMap * map,
		const BombicMap::Field & field) {
	connectCheckbox(generateBox_, map->generatedBox(field));
	connectCheckbox(generateCreature_, map->generatedCreature(field));
}

/** @details
 * Nastavi a pripoji @p checkbox na generovany objekt @p genObj.
 * @param checkbox tlacitko manipulujici s generovanym objektem.
 * @param genObj generovany objekt, ktery chceme napojit na tlacitko
 */
void MapFieldView::connectCheckbox(QCheckBox * checkbox,
		BombicGeneratedObject * genObj) {
	checkbox->setChecked(genObj->allowed());
	connect(checkbox, SIGNAL(stateChanged(int)),
		genObj, SLOT(toggleAllowance()));
}
