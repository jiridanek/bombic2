/** @file map_scene.h
 * Scena mapy MapScene zobrazuje mapu a zprostredkovava
 * uzivatelske akce v mape.
 */

#ifndef MAP_SCENE_H_GUARD_
#define MAP_SCENE_H_GUARD_

#include <QGraphicsScene>
#include <QPointF>
#include <QRect>

#include "bombic/map.h"

class QGraphicsRectItem;
class BombicMapBackground;
class BombicMapObject;

/** Scena (vyzobrazeni) mapy a interakce s uzivatelem.
 * Scena mapy je zobrazena v MapView, zpracovava uzivateluv vstup
 * a sofistikovane zobrazuje mapu. V ramci MapScene je podporovan
 * drag and drop, lze vkladat objekt vybrany v MapObjectPalette.
 */
class MapScene: public QGraphicsScene {

	Q_OBJECT

	public:
		/// Zkonstruuje mapu a jeji scenu.
		MapScene(int width, int height,
				BombicMapBackground * background,
				QObject * parent = 0);
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

	protected:
		/// Handler pohybu mysi.
		virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
		/// Handler stisku mysi.
		virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
		/// Handler uvoleni mysi.
		virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
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

		/// Policko mapy, ktereho se tyka pozice mysi.
		BombicMap::Field getField(
				const QPointF & mousePosition,
				const QRect & relatedObjectRect = QRect(0,0,1,1));

		/// Vlozit pozadi policek do sceny.
		void insertBackgroundFields(const QPixmap & texture);
		/// Vlozit zdi pozadi do sceny.
		void insertBackgroundWalls(BombicMapBackground * background);

		/// Inicializovat insertionHelperItem_.
		void initInsertionHelperItem();
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
		/// Zda-li je aktualne stisknuto tlacitko mysi,
		/// na ktere se da navazat uvolnenim za vzniku kliknuti.
		bool mousePressed_;

};

#endif
