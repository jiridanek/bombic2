/** @file generated_object.h
 * Informace o tom, zda lze na policku generovat nejaky objekt
 * nese trida BombicGeneratedObject.
 */

#ifndef BOMBIC_GENERATED_OBJECT_H_GUARD_
#define BOMBIC_GENERATED_OBJECT_H_GUARD_

#include <QObject>
#include "map.h"
#include "map_object.h"

class QGraphicsItem;

/** Generovany objekt.
 * V mape mame krome pevne umistenych objektu, ktere jsou v konkretni mape
 * pri rozehrati hry vzdy na stejnem miste, jeste generovane objekty,
 * o nichz potrebujeme v mape znat nekolik informaci.
 * Mezi informace, ktere drzi mapa sama o sobe, patri napriklad pocet a typ
 * takto generovanych objektu. Potom pro kazde policko drzi informaci o tom
 * zda lze na tomto policku umistit generovany objekt. Pro praci s touto
 * informaci a pro jeji vizualizaci slouzi potomci teto tridy.
 */
class BombicGeneratedObject: public QObject {

	Q_OBJECT

	public:
		/// Konstrukce.
		BombicGeneratedObject(const BombicMap::Field);
		/// Destrukce.
		~BombicGeneratedObject();

		/// Typ generovaneho objektu.
		virtual BombicMapObject::Type type() =0;

		/// Jestli je generovani blokovano jinym objektem.
		bool blocked();
		/// Jestli je generovani povoleno.
		bool allowed();
		/// Graficky prvek popisku do sceny.
		QGraphicsItem * graphicsItem();

		const BombicMap::Field & field();

		/// Jestli je mozne generovat.
		virtual bool canGenerate();

		virtual void addGeneratedObject(BombicMapObject * mapObject);

		BombicMapObject * takeGeneratedObject();

	public slots:
		/// Zobrazit popisek generovani.
		void showLabel();
		/// Skryt popisek generovani.
		void hideLabel();

		/// Zobrazit popisek generovani.
		void showObject();
		/// Skryt popisek generovani.
		void hideObject();

		/// Blokovat generovani.
		void block();
		/// Odblokovat generovani.
		void unblock();
		/// Nastavit blokovani generovani.
		void setBlocking(bool block);

		/// Povolit generovani.
		void allow();
		/// Zakazat generovani.
		void disallow();
		/// Zmeni povoleni generovani.
		void toggleAllowance();
		/// Nastavit povoleni generovani.
		void setAllowance(bool allow);

	signals:
		void canGenerateChanged();
		void discardingGenerated(BombicMapObject * object);

	protected:
		/// Nastavit pozici grafickeho prvku.
		void setLabelPos();
		/// Nastavit (obnovit) viditelnost popisku.
		void updateLabelVisibility();
		/// Nastavit (obnovit) viditelnost generovaneho objektu.
		void updateObjectsVisibility();

		/// Policko pro ktere objekt generuje.
		BombicMap::Field field_;

		/// Graficky prvek popisku do sceny.
		QGraphicsItem * labelGI_;

		/// Seznam generovanych objektu.
		QList<BombicMapObject *> generatedObjects_;

		/// Jestli je vizualizace popisku generovani skryta.
		bool labelHidden_;
		/// Jestli je vizualizace generovanych objektu skryta.
		bool objectHidden_;

		/// Jestli je generovani blokovano jinym objektem.
		bool blocked_;
		/// Jestli je generovani povoleno.
		bool allowed_;
};

#endif
