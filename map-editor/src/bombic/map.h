/** @file map.h
 * Struktura mapy tak, jak je pouzita ve hre.
 * Trida BombicMap reprezentuje mapu hry a resi spravnost polozeni objektu.
 */

#ifndef BOMBIC_MAP_H_GUARD_
#define BOMBIC_MAP_H_GUARD_

#include <QObject>
#include <QString>
#include <QVector>
#include <QPoint>
#include <QRect>
#include <QList>

class BombicMapBackground;
class BombicMapObject;
class MapObjectGenerator;

#define MAP_FIELD_INVALID BombicMap::Field(-1, -1);

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
class BombicMap: public QObject {

	Q_OBJECT

	public:
		/// Konstrukce prazdne mapy.
		BombicMap(const QString & name, int width, int height,
				BombicMapBackground * background, const QString & filename = "");
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
		MapObjectGenerator * boxGenerator(
				const BombicMap::Field & field);
		/// Generator priser na policku.
		MapObjectGenerator * creatureGenerator(
				const BombicMap::Field & field);

		/// Vsechny boxy, ktere se v mape nahodne generuji.
		const ObjectListT & generatedBoxes();
		/// Vsechny prisery, ktere se v mape nahodne generuji.
		const ObjectListT & generatedCreatures();

		/// Obnovit blokovani generatoru policka.
		void updateGeneratorsBlocking(const Field & field);

		/// Jmeno mapy.
		const QString & name();
		/// Soubor, do ktereho se ma mapa ulozit.
		const QString & filename();

		/// Jmeno mapy.
		void setName(const QString & name);
		/// Soubor, do ktereho se ma mapa ulozit.
		void setFilename(const QString & filename);

		/// Obdelnik mapy (v jednotkach policek).
		const QRect & fieldsRect();

		/// Pozadi mapy.
		BombicMapBackground * background();

		/// Jestli je treba mapu ulozit.
		bool needSave();
		/// Mapa ulozena.
		void saved();

	public slots:
		/// Nastavit pocet boxu k nahodnemu vygenerovani.
		void setGeneratedBoxesCount(BombicMapObject * box, int count);
		/// Nastavit pocet priser k nahodnemu vygenerovani.
		void setGeneratedCreaturesCount(BombicMapObject * creature, int count);

	signals:
		/// Pridana generovana bedna.
		void generatedBoxAdded(BombicMapObject * object);
		/// Pridana generovana prisera.
		void generatedCreatureAdded(BombicMapObject * object);
		/// Odstranena generovana bedna.
		void generatedBoxRemoved(BombicMapObject * object);
		/// Odstranena generovana bedna.
		void generatedCreatureRemoved(BombicMapObject * object);

	private slots:
		void edited();

	private:
		/// Mnozina reprezentujici jedno policko.
		typedef struct {
			/// Generator beden.
			MapObjectGenerator * boxGen;
			/// Generator priser.
			MapObjectGenerator * creatureGen;
			/// Seznam pevnych objektu.
			ObjectListT objList;
		} FieldSetT;

		/// Typ matice policek mapy.
		typedef QVector< QVector< FieldSetT > > FieldsT;

		/// Vlozit zdi pozadi do mapy.
		void insertBackgroundWalls();

		/// Pokusit se odblokovat generovani
		void updateGeneratorsBlocking(const FieldSetT & fieldSet);

		/// Nastavit pocet objektu k nahodnemu vygenerovani.
		void setGeneratedObjectsCount(ObjectListT & objList,
				BombicMapObject * object, int count);
		/// Pridat objekt k vygenerovani.
		void addGeneratedMapObject(ObjectListT & objList,
				BombicMapObject * object);
		/// Odstranit objekt k vygenerovani
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

		/// Jestli je mapa editovana.
		bool edited_;
};

#endif
