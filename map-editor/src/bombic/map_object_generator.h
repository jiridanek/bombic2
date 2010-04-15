/** @file map_object_generator.h
 * Informace o tom, zda lze na policku generovat nejaky objekt
 * nese trida TODO.
 */

#ifndef BOMBIC_MAP_OBJECT_GENERATOR_H_GUARD_
#define BOMBIC_MAP_OBJECT_GENERATOR_H_GUARD_

#include <QObject>
#include "map.h"
#include "map_object.h"

class QGraphicsItem;

/** Generovany objekt. TODO
 * V mape mame krome pevne umistenych objektu, ktere jsou v konkretni mape
 * pri rozehrati hry vzdy na stejnem miste, jeste generovane objekty,
 * o nichz potrebujeme v mape znat nekolik informaci.
 * Mezi informace, ktere drzi mapa sama o sobe, patri napriklad pocet a typ
 * takto generovanych objektu. Potom pro kazde policko drzi informaci o tom
 * zda lze na tomto policku umistit generovany objekt. Pro praci s touto
 * informaci a pro jeji vizualizaci slouzi potomci teto tridy.
 */
class BombicMapObjectGenerator: public QObject {

	Q_OBJECT

	public:
		/// Konstrukce.
		BombicMapObjectGenerator(const BombicMap::Field & field);
		/// Destrukce.
		~BombicMapObjectGenerator();

		/// Typ generovaneho objektu.
		virtual BombicMapObject::Type type() =0;

		/// Jestli je generovani blokovano jinym objektem.
		bool blocked();
		/// Jestli je generovani povoleno.
		bool allowed();
		/// Graficky prvek popisku do sceny.
		QGraphicsItem * graphicsItem();

		const BombicMap::Field & field();

		/// Jestli je mozne generovat dalsi objekt.
		virtual bool canGenerate();

		virtual void addGeneratedObject(BombicMapObject * mapObject);

		void removeGeneratedObjects();

		const BombicMap::ObjectListT & generatedObjects();

		/// Jestli objekt aktualne blokuje generovani beden.
		virtual bool blocksBoxGenerating();
		/// Jestli objekt aktualne blokuje generovani priser.
		virtual bool blocksCreatureGenerating();

	public slots:
		/// Zobrazit popisek generatoru.
		void show();
		/// Skryt popisek generatoru.
		void hide();

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
		/// Zmeneno zda lze generovat dalsi objekt.
		void canGenerateChanged();
		void removingGeneratedObject(BombicMapObject * object);

	protected:
		/// Nastavit pozici grafickeho prvku.
		void setLabelPos();
		/// Nastavit (obnovit) viditelnost popisku.
		void updateLabelVisibility();

		/// Policko pro ktere se generuje.
		BombicMap::Field field_;

		/// Graficky prvek popisku do sceny.
		QGraphicsItem * labelGI_;

		/// Seznam generovanych objektu.
		BombicMap::ObjectListT generatedObjects_;

		/// Jestli je vizualizace popisku skryta.
		bool hidden_;

		/// Jestli je generovani blokovano jinym objektem.
		bool blocked_;
		/// Jestli je generovani povoleno.
		bool allowed_;
};

#endif
