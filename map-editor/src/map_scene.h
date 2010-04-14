/** @file map_scene.h
 * Scena mapy MapScene zobrazuje mapu a zprostredkovava
 * uzivatelske akce v mape.
 */

#ifndef MAP_SCENE_H_GUARD_
#define MAP_SCENE_H_GUARD_

#include <QGraphicsScene>
#include <QPointF>
#include <QRect>
#include <QSet>

#include "bombic/map.h"

class MapFieldView;
class QGraphicsRectItem;
class BombicMapBackground;
class BombicMapObject;
class BombicGeneratedObject;

/** Scena (vyzobrazeni) mapy a interakce s uzivatelem.
 * Scena mapy je zobrazena v MapView, zpracovava uzivateluv vstup
 * a sofistikovane zobrazuje mapu. V ramci MapScene je podporovan
 * drag and drop, lze vkladat objekt vybrany v MapObjectPalette.
 */
class MapScene: public QGraphicsScene {

	Q_OBJECT

	friend class MapFieldView;

	public:
		/// Zkonstruuje mapu a jeji scenu.
		MapScene(BombicMap * map, QObject * parent = 0);
		/// Destruuje mapu a jeji scenu.
		~MapScene();

		/// Vlozit objekt do sceny.
		void insert(BombicMapObject * object,
				const BombicMap::Field & dstField);
		/// Odstranit objekt ze sceny.
		void remove(BombicMapObject * object);

	public slots:
		/// Nastavit pracovni objekt.
		void setWorkingObject(BombicMapObject * object);
		/// Zrusit pracovni objekt.
		void unsetWorkingObject();
		/// Schovat pracovni objekt.
		void hideWorkingObject();


		void registerGeneratedBoxChange();
		void registerGeneratedCreatureChange();

		void addGeneratedBox(BombicMapObject * mapObj);
		void addGeneratedCreature(BombicMapObject * mapObj);

		void toggleObjectGenerating();
	protected:
		/// Handler pohybu mysi.
		virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
		/// Handler stisku mysi.
		virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
		/// Handler uvoleni mysi.
		virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
		/// Handler dvojiteho kliknuti.
		virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event);
		/// Handler vstupu tazeni mysi.
		virtual void dragEnterEvent(QGraphicsSceneDragDropEvent * event);
		/// Handler pohybu tazeni mysi.
		virtual void dragMoveEvent(QGraphicsSceneDragDropEvent * event);
		/// Handler vystupu tazeni mysi.
		virtual void dragLeaveEvent(QGraphicsSceneDragDropEvent * event);
		/// Handler uvolneni tazeni mysi.
		virtual void dropEvent(QGraphicsSceneDragDropEvent * event);

	private:
		/// Zacnout tazeni mysi.
		void startDragging(QGraphicsSceneMouseEvent * event);
		/// Pohnout pracovnim objektem ve scene.
		void moveWorkingObject(QGraphicsSceneMouseEvent * event);
		/// Vlozit pracovni objekt do sceny.
		void insertWorkingObject(QGraphicsSceneMouseEvent * event);
		/// Odstranit objekt, na ktery se kliklo, ze sceny.
		void removeClickedObject(QGraphicsSceneMouseEvent * event);
		/// Vybrat (oznacit) policko.
		void selectField(const QPointF & eventPos);
		/// Zrusit vyber (odoznacit policko).
		void unselectField();

		/// Policko mapy, ktereho se tyka pozice mysi.
		BombicMap::Field getField(
				const QPointF & mousePosition,
				const QRect & relatedObjectRect = QRect(0,0,1,1));

		/// Vlozit pozadi policek do sceny.
		void insertBackgroundFields();
		/// Vlozit zastupce generovanych objektu.
		void insertObjectsGraphicsItems();

		void removeGeneratedObjectsFromMap();

		typedef QSet<BombicGeneratedObject *> FieldsToGenerateObjectsT;





void insertGeneratedObjectItem(
		BombicGeneratedObject * genObj, qreal zDiff);
void initFieldsToGenerateObjects();
void initFieldToGenerateObject(
		BombicGeneratedObject * genObj,
		FieldsToGenerateObjectsT & fields,
		const char * registerGeneratedObjectChangeMethod,
		const char * addGeneratedObjectMethod );
void registerGeneratedObjectChange(
		BombicGeneratedObject * genObj,
		BombicMap::ObjectListT & objects,
		FieldsToGenerateObjectsT & fields);
void addGeneratedObject( BombicMapObject * mapObj,
		BombicMap::ObjectListT & objects,
		FieldsToGenerateObjectsT & fields);

void generateObjects();
void generateObjects(
		BombicMap::ObjectListT & objects,
		FieldsToGenerateObjectsT & fields);
BombicGeneratedObject * getRandomField(FieldsToGenerateObjectsT & fields);
BombicMapObject * takeRandomObject(BombicMap::ObjectListT & objects);













		/// Umisti prisery na policku, aby nebyly uplne v zakrytu.
		void sortCreatureGraphics(const BombicMap::Field & field);

		/// Inicializovat pomocne prvky sceny.
		void initHelperItems();
		/// Zobrazit pomocny prvek sceny zobrazujici objekt pri vkladani.
		void showInsertionHelperItem(const QRect & objectRect);
		/// Zobrazit pomocny prvek sceny zobrazujici, ze objekt lze vlozit.
		void showCanInsertItem(const QRect & objectRect);
		/// Zobrazit pomocny prvek sceny zobrazujici, ze objekt nelze vlozit.
		void showCannotInsertItem(const QRect & objectRect);

		/// Mapa hry zobrazovana ve scene.
		BombicMap * map_;
		/// Pracovni objekt (v cizim vlastnictvi).
		BombicMapObject * workingObject_;
		/// Pomocny prvek sceny zobrazujici objekt pri vkladani.
		QGraphicsRectItem * insertionHelperItem_;
		/// Pomocny prvek sceny zobrazujici vybrane policko.
		QGraphicsRectItem * selectedFieldHelperItem_;
		/// Vybrane (oznacene) policko.
		BombicMap::Field selectedField_;
		#define MAP_SCENE_FIELD_NOT_SELECTED BombicMap::Field(-1, -1);

		BombicMap::ObjectListT boxesToGenerate_;
		BombicMap::ObjectListT creaturesToGenerate_;

		FieldsToGenerateObjectsT fieldsToGenerateBoxes_;
		FieldsToGenerateObjectsT fieldsToGenerateCreatures_;

		/// Zda-li je aktualne stisknuto tlacitko mysi,
		/// na ktere se da navazat uvolnenim za vzniku kliknuti.
		bool mousePressed_;
		/// Zda-li bylo nevyuzite kliknuti za vzniku dvojkliku.
		bool mouseClicked_;

		/// Zda-li se maji (nebo spis nemaji) generovat objekty.
		bool doObjectGenerating_;
};

#endif
