/** @file generated_bonuses_wizard.h
 * Pruvodce nastavenim generovani bonusu @c GeneratedBonusesWizard.
 */

#ifndef GENERATED_BONUSES_WIZARD_H_GUARD_
#define GENERATED_BONUSES_WIZARD_H_GUARD_

#include "generated_objects_wizard.h"

/** Stranka pruvodce nastavenim generovani bonusu.
 */
class GeneratedBonusesWizardPage: public GeneratedObjectsWizardPage {

	public:
		/// Konstrukce stranky.
		explicit GeneratedBonusesWizardPage(BombicMap * map);

		/// Typ objektu, ktere chceme nastavovat.
		virtual BombicMapObject::Type type();

		/// Mnozne cislo pojmenovani typu
		virtual QString objectTypePlural();

	protected:
		/// Ziskat (z mapy) seznam generovanych objektu.
		virtual const BombicMap::ObjectListT & generatedObjects();

		/// Nastavit (v mape) pocet objektu k vygenerovani.
		virtual void setGeneratedObjectsCount(
				BombicMapObject * object, int count);
};

/** Pruvodce nastavenim generovani bonusu.
 * Pruvodce zobrazi stranku @c GeneratedBonusesWizardPage.
 */
class GeneratedBonusesWizard: public GeneratedObjectsWizard {

	public:
		/// Konstrukce pruvodce.
		explicit GeneratedBonusesWizard(BombicMap * map,
				QWidget * parent = 0);
};

#endif
