/** @file generated_creatures_wizard.h
 * Pruvodce nastavenim generovani priser @c GeneratedCreatureWizard.
 */

#ifndef GENERATED_CREATURES_WIZARD_H_GUARD_
#define GENERATED_CREATURES_WIZARD_H_GUARD_

#include "generated_objects_wizard.h"


/** Stranka pruvodce nastavenim generovani priser.
 */
class GeneratedCreaturesWizardPage: public GeneratedObjectsWizardPage {

	public:
		/// Konstrukce stranky.
		explicit GeneratedCreaturesWizardPage(BombicMap * map);

		/// Typ objektu, ktere chceme nastavovat.
		virtual BombicMapObject::Type type();

	protected:
		/// Seznam generovanych objektu mapy.
		virtual const BombicMap::ObjectListT & generatedObjects();

		/// Nastavit (v mape) pocet objektu k vygenerovani.
		virtual void setGeneratedObjectsCount(
				BombicMapObject * object, int count);
};

/** Pruvodce nastavenim generovani priser.
 * Pruvodce zobrazi stranku @c GeneratedCreaturesWizardPage.
 */
class GeneratedCreaturesWizard: public GeneratedObjectsWizard {

	public:
		explicit GeneratedCreaturesWizard(BombicMap * map,
				QWidget * parent = 0);
};

#endif
