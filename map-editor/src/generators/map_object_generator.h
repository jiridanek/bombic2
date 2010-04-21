/** @file map_object_generator.h
 * Informace o tom, zda lze na policku generovat nejaky objekt,
 * nese trida MapObjectGenerator. Uchovava take generovane objekty.
 */

#ifndef MAP_OBJECT_GENERATOR_H_GUARD_
#define MAP_OBJECT_GENERATOR_H_GUARD_

#include <QObject>
#include "map.h"
#include "map_object.h"

class QGraphicsItem;

/** Generator objektu mapy.
 * V mape mame krome pevne umistenych objektu, ktere jsou v konkretni mape
 * pri rozehrati hry vzdy na stejnem miste, jeste generovane objekty,
 * o nichz potrebujeme v mape znat nekolik informaci.
 * Mezi informace, ktere drzi mapa sama o sobe, patri napriklad pocet a typ
 * takto generovanych objektu. Potom pro kazde policko drzi informaci o tom
 * zda lze na tomto policku umistit generovany objekt. Pro praci s touto
 * informaci a pro jeji vizualizaci slouzi potomci teto tridy.
 * Ve spolupraci s komponentami, ktere maji pristup k cele mape take
 * prubezne simuluje generovani objektu tak, aby uzivatel v kazdem okamziku
 * priblizne videl, jak muze mapa vypadat i s vygenerovanymi objekty.
 * Proto drzi seznam vygenerovanych objektu a kdyz je generator blokovan,
 * odstrani tyto objekty a vyvola signal, ktere objekty generoval
 * a nyni nemuze.
 */
class MapObjectGenerator: public QObject {

	Q_OBJECT

	public:
		/// Konstrukce.
		MapObjectGenerator(const BombicMap::Field & field);
		/// Destrukce.
		~MapObjectGenerator();

		/// Typ generovanych objektu.
		virtual BombicMapObject::Type type() =0;

		/// Jestli je generovani blokovano jinym objektem.
		bool blocked();
		/// Jestli je generovani povoleno.
		bool allowed();
		/// Graficky prvek popisku generatoru do sceny.
		QGraphicsItem * graphicsItem();
		/// Policko, na kterem generuje.
		const BombicMap::Field & field();

		/// Jestli je mozne generovat dalsi objekt.
		virtual bool canGenerate();

		/// Pridat generovany objekt.
		virtual void addGeneratedObject(BombicMapObject * mapObject);
		/// Odstranit vsechny generovane objekty.
		void removeGeneratedObjects();
		/// Seznam generovanych objektu.
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
		/// Zmenena schopnost generovat dalsi objekt.
		void allowanceChanged();
		/// Zmenena schopnost generovat dalsi objekt.
		void canGenerateChanged();
		/// Odstranovani generovaneho objektu.
		void removingGeneratedObject(BombicMapObject * object);

	protected:
		/// Nastavit pozici grafickeho prvku.
		void setLabelPos();
		/// Nastavit (obnovit) viditelnost popisku.
		void updateLabelVisibility();

		/// Policko, pro ktere se generuje.
		BombicMap::Field field_;

		/// Graficky prvek popisku generatoru do sceny.
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
