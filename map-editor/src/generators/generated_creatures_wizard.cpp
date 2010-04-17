
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

const BombicMap::ObjectListT &
		GeneratedCreaturesWizardPage::generatedObjects() {
	return map_->generatedCreatures();
}
