/** @file generated_object.h
 * Informace o tom, zda lze na policku generovat nejaky objekt
 * nese trida BombicGeneratedObject.
 */

#ifndef BOMBIC_GENERATED_OBJECT_H_GUARD_
#define BOMBIC_GENERATED_OBJECT_H_GUARD_

#include "map.h"

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
class BombicGeneratedObject {

	public:
		/// Konstrukce.
		BombicGeneratedObject();
		/// Destrukce.
		~BombicGeneratedObject();

		/// Zobrazit vizualizaci.
		void show();
		/// Skryt vizualizaci.
		void hide();
		/// Povolit generovani.
		void allow();
		/// Zakazat generovani.
		void disallow();

		/// Jestli je generovani povoleno.
		bool allowed();

		/// Graficky prvek do sceny.
		QGraphicsItem * graphicsItem();

	protected:
		/// Nastavit pozici grafickeho prvku.
		void setPos(const BombicMap::Field & field);

		/// graficky prvek do sceny.
		QGraphicsItem * graphicsItem_;

		/// Jestli je generovani povoleno.
		bool allowed_;
		/// Jestli je vizualicaze skryta.
		bool hidden_;
};

#endif
