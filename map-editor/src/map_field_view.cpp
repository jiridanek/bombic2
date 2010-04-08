
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


MapFieldView::MapFieldView(QWidget * parent):
		QWidget(parent) {
	setMaximumHeight(2*CELL_SIZE);

	QGridLayout * grid = new QGridLayout(this);

	generateBox_ = new QCheckBox(tr("generate &Box"), this);
	grid->addWidget(generateBox_, 0, 0);
	generateCreature_ = new QCheckBox(tr("generate &Creature"), this);
	grid->addWidget(generateCreature_, 1, 0);

	scrollArea_ = new QScrollArea(this);
	scrollArea_->setWidgetResizable(true);
	scrollArea_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	grid->addWidget(scrollArea_, 0, 1, 2, 1);
	hide();
}

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
	QWidget * scrollAreaWidget = new QWidget;
	QHBoxLayout * scrollLayout = new QHBoxLayout(scrollAreaWidget);
	scrollLayout->setAlignment(Qt::AlignLeft);
	// add objects to scrolled widget
	foreach(BombicMapObject * mapObj,
			scene->map_->objectsOnField(field)) {
		scrollLayout->addWidget(
			new MapFieldViewObject(mapObj, this) );
	}
	// replace scrolled widget with newly created one
	scrollArea_->setWidget(scrollAreaWidget);
	// and show it
	scrollAreaWidget->show();
	show();
}

void MapFieldView::disconnectCheckboxes() {
	generateBox_->disconnect();
	generateCreature_->disconnect();
}

void MapFieldView::connectCheckboxes(BombicMap * map,
		const BombicMap::Field & field) {
	connectCheckbox(generateBox_, map->generatedBox(field));
	connectCheckbox(generateCreature_, map->generatedCreature(field));
}

void MapFieldView::connectCheckbox(QCheckBox * checkbox,
		BombicGeneratedObject * genObj) {
	checkbox->setChecked(genObj->allowed());
	connect(checkbox, SIGNAL(stateChanged(int)),
		genObj, SLOT(toggleAllowance()));
}