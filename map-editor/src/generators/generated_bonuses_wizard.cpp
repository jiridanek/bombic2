#include "generated_bonuses_wizard.h"

GeneratedBonusesWizard::GeneratedBonusesWizard(BombicMap * map, QWidget * parent):
		GeneratedObjectsWizard(
			new GeneratedBonusesWizardPage(map), parent) {

}

GeneratedBonusesWizardPage::GeneratedBonusesWizardPage(BombicMap * map):
		GeneratedObjectsWizardPage(map) {
	initObjects();
}

BombicMapObject::Type GeneratedBonusesWizardPage::type() {
	return BombicMapObject::Bonus;
}

QString GeneratedBonusesWizardPage::objectTypePlural() {
	return tr("bonuses");
}

const BombicMap::ObjectListT & GeneratedBonusesWizardPage::generatedObjects() {
	return map_->generatedBonuses();
}

void GeneratedBonusesWizardPage::setGeneratedObjectsCount(
		BombicMapObject * object, int count) {
	map_->setGeneratedBonusesCount(object, count);
}
