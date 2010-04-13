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
		BombicGeneratedObject();
		/// Destrukce.
		~BombicGeneratedObject();

		/// Jestli je generovani blokovano jinym objektem.
		bool blocked();
		/// Jestli je generovani povoleno.
		bool allowed();
		/// Jestli je mozne generovat.
		bool canGenerate();

		/// Graficky prvek do sceny.
		QGraphicsItem * graphicsItem();

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


	protected:
		/// Nastavit pozici grafickeho prvku.
		void setPos(const BombicMap::Field & field);
		/// Nastavit (obnovit) viditelnost grafickeho prvku.
		void updateLabelVisibility();

		/// graficky prvek do sceny.
		QGraphicsItem * labelGI_;

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
