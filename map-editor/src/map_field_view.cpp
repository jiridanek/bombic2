
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
	BombicMap::Field field = scene->selectedField_;
	if(!scene->map_->fieldsRect().contains(field)) {
		hide();
		return;
	}

	QWidget * scrollAreaWidget = new QWidget;
	QHBoxLayout * scrollLayout = new QHBoxLayout(scrollAreaWidget);
	scrollLayout->setAlignment(Qt::AlignLeft);

	BombicGeneratedObject * genBox = scene->map_->generatedBox(field);
	generateBox_->disconnect();
	generateBox_->setChecked(genBox->allowed());
	connect(generateBox_, SIGNAL(stateChanged(int)),
		genBox, SLOT(toggleAllowance()));

	foreach(BombicMapObject * o,
			scene->map_->objectsOnField(field)) {
		QLabel * viewObject = new MapFieldViewObject(o, this);
		scrollLayout->addWidget(viewObject);
	}
	scrollArea_->setWidget(scrollAreaWidget);
	scrollAreaWidget->show();
	show();
}
