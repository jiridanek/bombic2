
#include "generated_creatures_wizard.h"

GeneratedCreaturesWizard::GeneratedCreaturesWizard(
		BombicMap * map, QWidget * parent):
				GeneratedObjectsWizard(
					new GeneratedCreaturesWizardPage(map),
					parent) {

}

GeneratedCreaturesWizardPage::GeneratedCreaturesWizardPage(BombicMap * map):
		GeneratedObjectsWizardPage(map) {
	initObjects();
}

BombicMapObject::Type GeneratedCreaturesWizardPage::type() {
	return BombicMapObject::Creature;
}

QString GeneratedCreaturesWizardPage::objectTypePlural() {
	return tr("creatures");
}

const BombicMap::ObjectListT &
		GeneratedCreaturesWizardPage::generatedObjects() {
	return map_->generatedCreatures();
}

void GeneratedCreaturesWizardPage::setGeneratedObjectsCount(
		BombicMapObject * object, int count) {
	map_->setGeneratedCreaturesCount(object, count);
}
