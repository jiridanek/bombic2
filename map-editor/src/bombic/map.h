/** @file map.h
 * Struktura mapy tak, jak je pouzita ve hre.
 * Trida BombicMap reprezentuje mapu hry a resi spravnost polozeni objektu.
 */

#ifndef BOMBIC_MAP_H_GUARD_
#define BOMBIC_MAP_H_GUARD_

#include <QString>
#include <QVector>
#include <QPoint>
#include <QRect>
#include <QLinkedList>

class BombicMapBackground;
class BombicMapObject;
class BombicGeneratedObject;

/** Mapa hry Bombic.
 * Udrzuje strukturu mapy, rozlozeni objektu mapy.
 * V mape, jak  je ulozena na disku jsou napevno umistene objekty
 * a objekty, ktere se pri konstrukci mapy ve hre nahodne vygeneruji.
 * V mape lze urcit, na ktera policka lze tyto objekty generovat,
 * a na ktera ne. Vzhledem k tomu, ze lze generovat pouze prisery a bedny,
 * je implementace primo zamerena na tento konkretni pripad.
 * U kazdeho policka jsou tedy vedeny objekty
 * (jeden pro bednu a jeden pro priseru), kazdy z nich rika, zda-li lze na
 * policku objekt generovat. Pro ucely dokumentace budu nazyvat tyto objekty
 * (instance tridy BombicGeneratedObject) <em>generovane objekty</em>.
 * Nasledne je BombicMap vizualizovana skrze @c MapScene.
 */
class BombicMap {

	public:
		/// Konstrukce prazdne mapy.
		BombicMap(int width, int height,
				BombicMapBackground * background);
		/// Destrukce mapy a jejich objektu.
		~BombicMap();

		/// Policko mapy (souradnice v jednotkach policek).
		typedef QPoint Field;
		/// Seznam objektu mapy.
		typedef QLinkedList<BombicMapObject *> ObjectListT;

		/// Test zda lze vlozit objekt do mapy.
		bool canInsert(BombicMapObject * object,
				const BombicMap::Field & dstField);
		/// Vlozit objekt do mapy.
		void insert(BombicMapObject * object,
				const BombicMap::Field & dstField);
		/// Odstranit objekt z mapy.
		void remove(BombicMapObject * object);

		/// Objekt na vrchu policka.
		BombicMapObject * objectOnTop(const BombicMap::Field & field);
		/// Seznam objektu na policku.
		const ObjectListT & objectsOnField(const BombicMap::Field & field);

		/// Generovany box na policku.
		BombicGeneratedObject * generatedBox(const BombicMap::Field & field);
		/// Generovana prisera na policku.
		BombicGeneratedObject * generatedCreature(const BombicMap::Field & field);

		/// Obdelnik mapy (v jednotkach policek).
		const QRect & fieldsRect();

		/// Pozadi mapy.
		BombicMapBackground * background();

	private:
		/// Mnozina reprezentujici jedno policko.
		typedef struct {
			/// Generovana bedna.
			BombicGeneratedObject * genBox;
			/// Generovana prisera.
			BombicGeneratedObject * genCreature;
			/// Seznam pevnych objektu.
			ObjectListT objList;
		} FieldSetT;

		/// Typ matice policek mapy.
		typedef QVector< QVector< FieldSetT > > FieldsT;


		/// Vlozit zdi pozadi do mapy.
		void insertBackgroundWalls();

		/// Pokusit se odblokovat generovani
		void updateBlockGeneratingObjects(const FieldSetT & fieldSet);

		/// Jmeno mapy.
		QString name_;
		/// Soubor, do ktereho se pripadne mapa ulozi.
		QString filename_;

		/// Obdelnik mapy (v jednotkach policek).
		QRect fieldsRect_;
		/// Pozadi mapy.
		BombicMapBackground * background_;

		/// Matice policek mapy, velikost odpovida @p fieldsRect_.
		FieldsT fields_;


};

#endif
