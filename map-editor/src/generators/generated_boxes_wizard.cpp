#include "generated_boxes_wizard.h"

GeneratedBoxesWizard::GeneratedBoxesWizard(BombicMap * map, QWidget * parent):
		GeneratedObjectsWizard(
			new GeneratedBoxesWizardPage(map), parent) {

}

GeneratedBoxesWizardPage::GeneratedBoxesWizardPage(BombicMap * map):
		GeneratedObjectsWizardPage(map) {
	initObjects();
}

BombicMapObject::Type GeneratedBoxesWizardPage::type() {
	return BombicMapObject::Box;
}

QString GeneratedBoxesWizardPage::objectTypePlural() {
	return tr("boxes");
}

const BombicMap::ObjectListT & GeneratedBoxesWizardPage::generatedObjects() {
	return map_->generatedBoxes();
}

void GeneratedBoxesWizardPage::setGeneratedObjectsCount(
		BombicMapObject * object, int count) {
	map_->setGeneratedBoxesCount(object, count);
}
