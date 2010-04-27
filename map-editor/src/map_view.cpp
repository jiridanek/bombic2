
#include "map_view.h"

#include <QApplication>
#include <QAction>
#include <QLabel>
#include <QEvent>
#include <QDrag>
#include <QMimeData>
#include <QMessageBox>

#include <constants.h>

#include "map_scene.h"
#include "map_field_view.h"
#include "qt/zoomwidget.h"
#include "main_window.h"
#include "resource_handler.h"
#include "bombic/map.h"
#include "bombic/map_background.h"
#include "bombic/map_object.h"

#include "map_view_wizards.h"

/// Nazev vlastnosti mime dat uchovavajici pointer na tazeny objekt.
#define MAP_VIEW_DRAGGED_OBJECT_PROPERTY "draggedBombicMapObject"

SINGLETON_INIT(MapView);

/** @details
 * Vytvori pohled na mapu (QGraphicsView),
 * nahled pracovniho objektu, a zoomovaci widget.
 * Vytvori defaultni pozadi mapy a scenu s timto defaultnim pozadim.
 * @param parent rodicovsky widget
 */
MapView::MapView(QWidget * parent):
		QGraphicsView(parent), map_(0), scene_(0),
		lastZoomQuotient_(1.0),
		zoomWidget_( new ZoomWidget(ZOOM_STEP,
			ZOOM_MINIMUM_VALUE, ZOOM_MAXIMUM_VALUE) ),
		workingObjectLabel_(new QLabel),
		fieldView_(new MapFieldView),
		mapSizeWizard_(0) {

	SINGLETON_CONSTRUCT;

	// add docked widgets to main window
	MAIN_WINDOW->addMapView(this);
	MAIN_WINDOW->addWorkingObjectLabel(workingObjectLabel_);
	MAIN_WINDOW->addMapFieldView(fieldView_);
	MAIN_WINDOW->addZoomWidget(zoomWidget_);
	// and also connect them
	connect(zoomWidget_, SIGNAL(zoomChanged(qreal)),
		this, SLOT(setZoom(qreal)) );

	// connect the menu actions
	connect(MAIN_WINDOW->action(MainWindow::NewMapAction),
		SIGNAL(triggered()),
		this, SLOT(openEmptyMap()) );
	connect(MAIN_WINDOW->action(MainWindow::OpenMapAction),
		SIGNAL(triggered()),
		this, SLOT(openMap()) );
	connect(MAIN_WINDOW->action(MainWindow::SaveMapAction),
		SIGNAL(triggered()),
		this, SLOT(saveMap()) );
	connect(MAIN_WINDOW->action(MainWindow::SaveMapAsAction),
		SIGNAL(triggered()),
		this, SLOT(saveMapAs()) );

	connect(MAIN_WINDOW->action(MainWindow::MapSizeAction),
		SIGNAL(triggered()),
		this, SLOT(changeSize()) );
	connect(MAIN_WINDOW->action(MainWindow::MapBackgroundAction),
		SIGNAL(triggered()),
		this, SLOT(changeBackground()) );
}

MapView::~MapView() {
	SINGLETON_DESTROY;
}

/** @details
 * Pokusi se zavrit editaci prave otevrene mapy a otevre
 * novou (defaultni prazdnou) mapu pro editaci.
 */
void MapView::openEmptyMap() {
	if(!closeMap()) {
		return;
	}
	// create default (empty) map
	map_ = RESOURCE_HANDLER->loadEmptyMap();
	if(map_) {
		// and scene for it
		scene_ = new MapScene(map_, this);
		setScene(scene_);
	}
}

/** @details
 * Pokusi se zavrit editaci prave otevrene mapy a otevre
 * novou mapu (vybranou uzivatelem) pro editaci.
 */
void MapView::openMap() {
	if(!closeMap()) {
		return;
	}
	// open new map
	map_ = RESOURCE_HANDLER->loadMap();
	if(map_) {
		// and scene for it
		scene_ = new MapScene(map_, this);
		setScene(scene_);
	}
}

void MapView::changeMap(BombicMap * newMap) {
	if(map_) {
		delete scene_;
	}

	map_ = newMap;
	scene_ = new MapScene(map_, this);
	setScene(scene_);
}

/** @details
 * Kontroluje, zda byla mapa ulozena.
 * Pokud ne, dotaze se uzivatele jestli mapu ulozit,
 * zrusit zavirani mapy nebo zahodit zmeny.
 * Pokud je tedy potom mozne editaci mapy ukoncit,
 * dealokuje scenu a jeji mapu.
 * @return Uspech ukonceni editace mapy.
 * @retval true Editace mapy je uspesne ukoncena,
 *              muzeme pokracovat nactenim jine mapy nebo ukoncit program.
 * @retval false Editace mapy neukoncena, meli bychom uzivateli umoznit
 *               pokracovat v editaci mapy.
 */
bool MapView::closeMap() {
	if(!map_) {
		// map is already closed
		return true;
	}
	if(map_->wasModified() && !askToCloseModifiedMap()) {
		return false;
	}

	// realy close and deallocate the scene (and its map)
	delete scene_;
	map_ = 0;
	scene_ = 0;
	return true;
}

/** @details
 * Zobrazi dialog s moznostmi <em>ulozit, zahodit, zrusit</em>.
 * Pokud uzivatel zvoli ulozit, pokusi se ulozit mapu.
 * @return Zda muze byt editace mapy ukoncena.
 */
bool MapView::askToCloseModifiedMap() {
	QMessageBox::StandardButton result =
		QMessageBox::warning(this,
			tr("Close the map - Bombic map editor"),
			tr("The map")+" "+map_->name()+" "+
				tr("has been modified.")+"\n"+
				tr("Do you want to save your changes?"),
			QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
	switch(result) {
		case QMessageBox::Save:
			return RESOURCE_HANDLER->saveMap(map_);
		case QMessageBox::Discard:
			return true;
		case QMessageBox::Cancel:
			return false;
		default:
			Q_ASSERT_X(false, "askToCloseModifiedMap()",
				"unhandled result button");
			return false;
	}
}

/** @details
 * Pokud je treba, ulozi mapu do jejiho umisteni.
 * @see ResourceHandler::saveMap()
 */
void MapView::saveMap() {
	if(!map_ || !map_->wasModified()) {
		return;
	}
	RESOURCE_HANDLER->saveMap(map_);
}

/** @details
 * Ulozi mapu do noveho umisteni.
 * @see ResourceHandler::saveMapAs()
 */
void MapView::saveMapAs() {
	if(!map_) {
		return;
	}
	RESOURCE_HANDLER->saveMapAs(map_);
}

#include <QDebug>
void MapView::changeSize() {
	if(!map_) {
		return;
	}
	if(!mapSizeWizard_) {
		mapSizeWizard_ = new MapSizeWizard;
		connect(mapSizeWizard_, SIGNAL(mapResized(BombicMap *)),
			this, SLOT(changeMap(BombicMap *)) );
	}
	mapSizeWizard_->setMap(map_);
	mapSizeWizard_->show();
}

void MapView::changeBackground() {
	if(!map_) {
		return;
	}
}


/** @details
 * Nastavi zoom pohledu na mapu.
 * Pokud je novy zoom 1, resetuje transformacni matici,
 * pak je tedy prace se scenou mnohem rychlejsi (netransformuje se).
 * @param zoomQuotient novy transformacni kvocient
 */
void MapView::setZoom(qreal zoomQuotient) {
	// may take a while
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	if(zoomQuotient==1.0) {
		// no zoom - reset for responsibility
		resetTransform();
	} else {
		// find the difference
		qreal dz = zoomQuotient/lastZoomQuotient_;
		// and scale last zoomed viewport
		scale(dz, dz);
		// wait for repaint (to show the cursor)
		repaint();
	}
	lastZoomQuotient_ = zoomQuotient;
	QApplication::restoreOverrideCursor();
}

/** @details
 * Tento slot byva napojen na @c QGraphicsScene::changed(), ktery je vyvolan
 * po zmene sceny pri navrtu do event loop. To je sice vymysleno pekne,
 * @c updateScene() se nevola kvuli kazde kravine a vse krasne funguje.
 * Pri vetsich upravach se ale dostavame do podivne situace, kdy
 * jednou vyvolany @c updateScene() dostane i nekolik milionu
 * obdelniku (ktere jsou bud malinke nebo se hodne prekryvaji),
 * a pro kazdy obdelnik nejspis tupe zavola obnoveni sceny.
 * To muze trvat opravdu dlouho.
 * Prisel jsem tedy s resenim, ktere situaci rozdeli do tri kategorii
 * podle pomeru poctu obdelniku a velikosti sceny.
 * Prvni (nejcastejsi) kategorie zpracuje obdelniky po staru, jeden po druhem.
 * Druha, kdy uz mame obdelniku mezi obvodem @c w+h a obsahem @c w*h obdelnika
 * sceny, sjednoti vsechny obdelniky do jednoho, zachovava tedy
 * v urcitych pripadech alespon lokalitu.
 * Treti kategorie, kdy uz mame obdelniku dokonce vice
 * nez je pixelu v cele scene se s obdelniky nepracuje a obnovi se cela scena.
 * @param rects obdelniky, ktere se maji obnovit
 */
void MapView::updateScene(const QList<QRectF> & rects) {
	// get dimensions
	qreal x, y, w, h;
	sceneRect().getRect(&x, &y, &w, &h);

	// edited rects
	QList<QRectF> myRects;
	if(rects.size() < w+h) {
		// small amount of rects will be dealt original way
		myRects = rects;
	} else if(rects.size() < w*h) {
		// I unite the rects to preserve locality
		QRectF rect(rects.first());
		for(QList<QRectF>::const_iterator it = rects.begin() ;
				it != rects.end() ; ++it) {
			rect |= *it;
		}
		myRects.append(rect);
	} else {
		// update the whole rect
		myRects.append(sceneRect());
	}

	QGraphicsView::updateScene(myRects);
}

/** @details
 * Evokuje signal leaved().
 * @param event pointer na udalost, ktera handler vyvolala
 */
void MapView::leaveEvent(QEvent * event) {
	Q_UNUSED(event);
	emit leaved();
}

/** @details
 * @param object pracovniho objektu, ktery chceme zobrazit
 */
void MapView::showWorkingObjectLabel(BombicMapObject * object) {
	workingObjectLabel_->setPixmap(object->thumbnail());
	workingObjectLabel_->show();
}

void MapView::hideWorkingObjectLabel() {
	workingObjectLabel_->hide();
}

void MapView::updateFieldView() {
	fieldView_->update(scene_);
}

/** @details
 * Naalokuje @c QMimeData a nastavi je tak,
 * aby prenasela pointer na zadany objekt.
 * Vlastnictvi vracenych mime dat prechazi na volajiciho.
 * @param object pointer na neseny objekt
 * @return Nove alokovana mime data prenasejici @p object.
 * @see getMapObject()
 */
QMimeData * MapView::createMimeData(BombicMapObject * object) {
	QMimeData * mimeData = new QMimeData;
	// the property is only for internal use as the pointer to object
	mimeData->setProperty(
		MAP_VIEW_DRAGGED_OBJECT_PROPERTY,
		qVariantFromValue(static_cast<void *>(object)) );
	// for external use set the object name
	mimeData->setText(object->name());
	return mimeData;
}

/** @details
 * Z mime dat, ktera prenasi pointer na objekt ziska tento objekt.
 * @param mimeData mime data prnasejici objekt
 * @return Objekt prenaseny mime daty.
 * @retval 0 @p mimeData nenesou zadny objekt.
 * @see createMimeData()
 */
BombicMapObject * MapView::getMapObject(const QMimeData * mimeData) {
	// the internal mimedata bring internal pointer to the object
	QVariant objVar =
		mimeData->property(MAP_VIEW_DRAGGED_OBJECT_PROPERTY);
	if(!objVar.isValid()) {
		// property was not set
		// maybe external or different type of mimedata
		return 0;
	}
	BombicMapObject * obj = static_cast<BombicMapObject *>(
		qVariantValue<void *>(objVar) );
	return obj;
}

/** @details
 * Spusti tazeni objektu mapy @p object.
 * @param srcWidget zdrojovy widget, ze ktereho byl drag vytvoren
 * @param object objekt mapy, ktery ma byt tazen
 * @see QDrag::exec(), createMimeData()
 */
void MapView::execDragging(QWidget * srcWidget, BombicMapObject * object) {
	QDrag * drag = new QDrag(srcWidget);
	drag->setMimeData(createMimeData(object));
	drag->setPixmap(object->thumbnail());
	drag->exec();
}
