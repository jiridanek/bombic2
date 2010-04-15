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
#include <QList>

class BombicMapBackground;
class BombicMapObject;
class BombicMapObjectGenerator;

/** Mapa hry Bombic.
 * Udrzuje strukturu mapy, rozlozeni objektu mapy.
 * V mape, jak  je ulozena na disku jsou napevno umistene objekty
 * a objekty, ktere se pri konstrukci mapy ve hre nahodne vygeneruji.
 * V mape lze urcit, na ktera policka lze tyto objekty generovat,
 * a na ktera ne. Vzhledem k tomu, ze lze generovat pouze prisery a bedny,
 * je implementace primo zamerena na tento konkretni pripad.
 * U kazdeho policka jsou tedy vedeny generatory
 * (jeden pro bednu a jeden pro prisery), kazdy z nich rika, zda-li lze na
 * policku objekt generovat.
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
		typedef QList<BombicMapObject *> ObjectListT;

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

		/// Generator beden na policku.
		BombicMapObjectGenerator * boxGenerator(
				const BombicMap::Field & field);
		/// Generator priser na policku.
		BombicMapObjectGenerator * creatureGenerator(
				const BombicMap::Field & field);

		const ObjectListT & generatedBoxes();
		const ObjectListT & generatedCreatures();

		/// Nastavi pocet boxu k nahodnemu vygenerovani.
		void setGeneratedBoxesCount(BombicMapObject * box, int count);
		/// Nastavi pocet priser k nahodnemu vygenerovani.
		void setGeneratedCreaturesCount(BombicMapObject * creature, int count);

		void updateGeneratorsBlocking(const Field & field);

		/// Obdelnik mapy (v jednotkach policek).
		const QRect & fieldsRect();

		/// Pozadi mapy.
		BombicMapBackground * background();

	private:
		/// Mnozina reprezentujici jedno policko.
		typedef struct {
			/// Generator beden.
			BombicMapObjectGenerator * boxGen;
			/// Generator priser.
			BombicMapObjectGenerator * creatureGen;
			/// Seznam pevnych objektu.
			ObjectListT objList;
		} FieldSetT;

		/// Typ matice policek mapy.
		typedef QVector< QVector< FieldSetT > > FieldsT;


		/// Vlozit zdi pozadi do mapy.
		void insertBackgroundWalls();

		/// Pokusit se odblokovat generovani
		void updateGeneratorsBlocking(const FieldSetT & fieldSet);

		void setGeneratedObjectsCount(ObjectListT & objList,
				BombicMapObject * object, int count);

		void addGeneratedMapObject(ObjectListT & objList,
				BombicMapObject * object);
		void removeGeneratedMapObject(ObjectListT & objList,
				BombicMapObject * object);

		/// Jmeno mapy.
		QString name_;
		/// Soubor, do ktereho se pripadne mapa ulozi.
		QString filename_;

		/// Obdelnik mapy (v jednotkach policek).
		QRect fieldsRect_;
		/// Seznam generovanych beden.
		ObjectListT generatedBoxes_;
		/// Seznam generovanych priser.
		ObjectListT generatedCreatures_;

		/// Pozadi mapy.
		BombicMapBackground * background_;

		/// Matice policek mapy, velikost odpovida @p fieldsRect_.
		FieldsT fields_;


};

#endif
