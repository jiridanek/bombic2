/** @file generated_boxes_wizard.h
 * Pruvodce nastavenim generovani beden @c GeneratedBoxesWizard.
 */

#ifndef GENERATED_BOXES_WIZARD_H_GUARD_
#define GENERATED_BOXES_WIZARD_H_GUARD_

#include "generated_objects_wizard.h"

/** Stranka pruvodce nastavenim generovani beden.
 */
class GeneratedBoxesWizardPage: public GeneratedObjectsWizardPage {

	public:
		/// Konstrukce stranky.
		explicit GeneratedBoxesWizardPage(BombicMap * map);

		/// Typ objektu, ktere chceme nastavovat.
		virtual BombicMapObject::Type type();

		/// Mnozne cislo pojmenovani typu
		virtual QString objectTypePlural();

	protected:
		/// Seznam generovanych objektu mapy.
		virtual const BombicMap::ObjectListT & generatedObjects();

		/// Nastavit (v mape) pocet objektu k vygenerovani.
		virtual void setGeneratedObjectsCount(
				BombicMapObject * object, int count);
};

/** Pruvodce nastavenim generovani beden.
 * Pruvodce zobrazi stranku @c GeneratedBoxesWizardPage.
 */
class GeneratedBoxesWizard: public GeneratedObjectsWizard {

	public:
		/// Konstrukce pruvodce.
		explicit GeneratedBoxesWizard(BombicMap * map,
				QWidget * parent = 0);
};

#endif
