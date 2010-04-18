
#include "generated_objects_wizard.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QLabel>
#include <QSpinBox>

#include "../resource_handler.h"
#include "../map_object_palette.h"

#include "../qt/flowlayout.h"

#define GENERATED_OBJECTS_WIZARD_MAX_COUNT 999

#define GENERATED_OBJECTS_WIZARD_FOREACH_OBJECT(it) \
	for( ObjectInfoByNameT::iterator it = objects_.begin() ; \
		it != objects_.end() ; ++it )

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

	mainLayout_ = new FlowLayout(scrollWidget, 10, 15);
}

/**
 * @warning Mela by byt volana prave jednou v konstruktoru.
 */
void GeneratedObjectsWizardPage::initObjects() {
	addObjects();
	// count the generated objects
	foreach(BombicMapObject * o, generatedObjects()) {
		GeneratedObjectInfoT & info = objects_[o->name()];
		++info.count;
		info.spinBox->setValue(info.count);
	}
}

void GeneratedObjectsWizardPage::addObjects() {
	// for new objects init the count
	foreach(BombicMapObject * o,
			MAP_OBJECT_PALETTE->getObjects(type()) ) {
		if(!objects_.contains(o->name())) {
			initObjectInfo(objects_[o->name()], o);
		}
	}
}

void GeneratedObjectsWizardPage::initObjectInfo(
		GeneratedObjectInfoT & objectInfo, BombicMapObject * object) {
	// the properties
	objectInfo.paletteObject = object;
	objectInfo.count = 0;
	objectInfo.spinBox = new QSpinBox(this);
	objectInfo.spinBox->setMaximum(GENERATED_OBJECTS_WIZARD_MAX_COUNT);
	objectInfo.spinBox->setSpecialValueText(tr("Not generated"));

	// and a little cosmetics
	QFrame * objectFrame = new QFrame(this);
	objectFrame->setFrameStyle(QFrame::Panel | QFrame::Raised);
	mainLayout_->addWidget(objectFrame);

	QLayout * layout = new QVBoxLayout(objectFrame);

	QLabel * objectLabel = new QLabel(this);
	objectLabel->setPixmap(object->pixmap());
	objectLabel->setAlignment(Qt::AlignHCenter);
	objectLabel->setBuddy(objectInfo.spinBox);
	layout->addWidget(objectLabel);

	layout->addWidget(objectInfo.spinBox);
}

void GeneratedObjectsWizardPage::initializePage() {
	addObjects();
}

void GeneratedObjectsWizardPage::cleanupPage() {
	GENERATED_OBJECTS_WIZARD_FOREACH_OBJECT(infoIt) {
		infoIt->spinBox->setValue(infoIt->count);
	}
}

void GeneratedObjectsWizardPage::setCountInMap() {
	GENERATED_OBJECTS_WIZARD_FOREACH_OBJECT(infoIt) {
		if(infoIt->count != infoIt->spinBox->value()) {
			infoIt->count = infoIt->spinBox->value();
			setGeneratedObjectsCount(
				infoIt->paletteObject, infoIt->count);
		}
	}
}
