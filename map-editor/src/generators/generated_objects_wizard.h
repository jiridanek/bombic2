/** @file generated_objects_wizard.h
 * Pruvodce nastavenim generovani objektu mapy @c GeneratedObjectsWizard.
 */

#ifndef GENERATED_OBJECTS_WIZARD_H_GUARD_
#define GENERATED_OBJECTS_WIZARD_H_GUARD_

#include <QWizard>
#include <QWizardPage>
#include <QHash>

#include "map_object.h"

class QLayout;
class BombicMap;

/** Stranka pruvodce nastavenim generovani objektu.
 * Na strance zobrazi vsechny objekty urciteho typu z palety.
 * U kazdeho druhu objektu nabizi moznost zmenit pocet generovanych objektu.
 * Po potvrzeni nastavi pocty generovanych objektu v mape.
 */
class GeneratedObjectsWizardPage: public QWizardPage {

	Q_OBJECT

	public:
		/// Konstrukce stranky.
		explicit GeneratedObjectsWizardPage(BombicMap * map);

		/// Inicializovat (obnovit) stranku.
		virtual void initializePage();
		/// Vycistit (navratit) stranku.
		virtual void cleanupPage();

		/// Typ objektu, ktere chceme nastavovat.
		virtual BombicMapObject::Type type() =0;


	public slots:
		/// Nastavit pocty generovanych objektu v mape.
		void setCountInMap();

	protected:
		void initObjects();
		void addObjects();

		/// Seznam generovanych objektu mapy.
		virtual const BombicMap::ObjectListT & generatedObjects() =0;

		/// The map.
		BombicMap * map_;

		/// Informace o generovanem objektu.
		typedef struct {
			/// Reprezentativni objekt z palety.
			BombicMapObject * paletteObject;
			/// Pocet generovanych objektu.
			int count;
		} GeneratedObjectInfoT;

		/// Generated objects by name.
		QHash<QString, GeneratedObjectInfoT> objects_;

		/// Hlavni layout, do ktereho se pridavaji objekty.
		QLayout * mainLayout_;
};

/** Pruvodce nastavenim generovani objektu.
 * Pruvodce zobrazi stranku @c GeneratedObjectsWizardPage.
 * Po potvrzeni vyvola ulozeni dat na strance.
 */
class GeneratedObjectsWizard: public QWizard {

	Q_OBJECT

	public:
		explicit GeneratedObjectsWizard(
				GeneratedObjectsWizardPage * page,
				QWidget * parent = 0);

	public slots:
		/// Zobrazit widget.
		void show();

	protected:
		GeneratedObjectsWizardPage * page_;
};

#endif
