
#include "generated_objects_wizard.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QLabel>

#include "../resource_handler.h"
#include "../map_object_palette.h"


GeneratedObjectsWizard::GeneratedObjectsWizard(
		GeneratedObjectsWizardPage * page, QWidget * parent):
				QWizard(parent), page_(page) {

	setOption(QWizard::NoBackButtonOnStartPage);

	addPage(page);
	connect(this, SIGNAL(accepted()),
		page, SLOT(setCountInMap()) );
}

void GeneratedObjectsWizard::show() {
	page_->initializePage();
	QWidget::show();
}

GeneratedObjectsWizardPage::GeneratedObjectsWizardPage(BombicMap * map):
		map_(map) {

	QLayout * l = new QVBoxLayout(this);
	QScrollArea * scrollArea = new QScrollArea(this);
	scrollArea->setWidgetResizable(true);
	l->addWidget(scrollArea);
	QWidget * scrollWidget = new QWidget;
	scrollArea->setWidget(scrollWidget);

	mainLayout_ = new QVBoxLayout(scrollWidget);
}

/**
 * @warning Mela by byt volana prave jednou v konstruktoru.
 */
void GeneratedObjectsWizardPage::initObjects() {
	addObjects();
	// count the generated objects
	foreach(BombicMapObject * o, generatedObjects()) {
		++objects_[o->name()].count;
	}
}

void GeneratedObjectsWizardPage::addObjects() {
	// for new objects init the count
	foreach(BombicMapObject * o,
			MAP_OBJECT_PALETTE->getObjects(type()) ) {
		if(!objects_.contains(o->name())) {
			QLabel * objectLabel = new QLabel(this);
			objectLabel->setPixmap(o->pixmap());
			objectLabel->show();
			mainLayout_->addWidget(objectLabel);

			GeneratedObjectInfoT objInfo;
			objInfo.paletteObject = o;
			objInfo.count = 0;
			objects_[o->name()] = objInfo;
		}
	}
}

void GeneratedObjectsWizardPage::initializePage() {
	addObjects();
}

void GeneratedObjectsWizardPage::cleanupPage() {
	// TODO foreach object here turn back the count
}

void GeneratedObjectsWizardPage::setCountInMap() {
	// TODO foreach object here set the count in map
}
