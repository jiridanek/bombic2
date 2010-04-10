
#include "map_view.h"

#include <QGraphicsView>
#include <QGridLayout>
#include <QLabel>
#include <QEvent>
#include <QDrag>
#include <QMimeData>

#include <constants.h>

#include "map_scene.h"
#include "map_field_view.h"
#include "qt/zoomwidget.h"
#include "resource_handler.h"
#include "bombic/map.h"
#include "bombic/map_background.h"
#include "bombic/map_object.h"

/// Nazev vlastnosti mime dat uchovavajici tazeny objekt.
#define MAP_VIEW_DRAGGED_OBJECT_PROPERTY "draggedBombicMapObject"

SINGLETON_INIT(MapView);

/** @details
 * Vytvori pohled na mapu (QGraphicsView),
 * nahled pracovniho objektu, a zoomovaci widget.
 * Vytvori defaultni pozadi mapy a scenu s timto defaultnim pozadim.
 * @param parent rodicovsky widget
 */
MapView::MapView(QWidget * parent):
		QWidget(parent),
		viewport_(new QGraphicsView), scene_(0),
		lastZoomQuotient_(1.0),
		zoomWidget_( new ZoomWidget(ZOOM_STEP,
			ZOOM_MINIMUM_VALUE, ZOOM_MAXIMUM_VALUE) ),
		workingObjectLabel_(new QLabel),
		fieldView_(new MapFieldView) {

	SINGLETON_CONSTRUCT;

	gridLayout()->addWidget(fieldView_, 0, 0, 1, 2);
	gridLayout()->addWidget(viewport_, 1, 0, 1, 2);
	gridLayout()->addWidget(workingObjectLabel_, 2, 0);
	connect(zoomWidget_, SIGNAL(zoomChanged(qreal)),
		this, SLOT(setZoom(qreal)) );
	gridLayout()->addWidget(zoomWidget_, 2, 1);

	BombicMap * defaultMap = RESOURCE_HANDLER->loadEmptyMap();
	if(defaultMap) {
		scene_ = new MapScene(defaultMap, this);
		viewport_->setScene(scene_);
	}
}

MapView::~MapView() {
	SINGLETON_DESTROY;
}

/** @details
 * Widget MapView ma jako layout @c QGridLayout.
 * Abychom v kazde chvili meli k tomuto layoutu pristup,
 * slouzi tato fce @c gridLayout() jako getter. Zaroven,
 * pokud jeste layout nebyl vytvoren, novy layout vytvori.
 * @return Grid layout rodicovskeho widgetu.
 */
QGridLayout * MapView::gridLayout() {
	QLayout * myLayout = layout();
	if(myLayout) {
		return static_cast<QGridLayout *>( myLayout );
	} else {
		return new QGridLayout(this);
	}
}

/** @details
 * Nastavi zoom pohledu na mapu.
 * Pokud je novy zoom 1, resetuje transformacni matici,
 * pak je tedy prace se scenou mnohem rychlejsi (netransformuje se).
 * @param zoomQuotient novy transformacni kvocient
 */
void MapView::setZoom(qreal zoomQuotient) {
	if(zoomQuotient==1.0) {
		viewport_->resetTransform();
	} else {
		qreal dz = zoomQuotient/lastZoomQuotient_;
		viewport_->scale(dz, dz);
	}
	lastZoomQuotient_ = zoomQuotient;
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
 */
QMimeData * MapView::createMimeData(BombicMapObject * object) {
	QMimeData * mimeData = new QMimeData;
	mimeData->setProperty(
		MAP_VIEW_DRAGGED_OBJECT_PROPERTY,
		qVariantFromValue(static_cast<void *>(object)) );
	mimeData->setText(object->name());
	return mimeData;
}

/** @details
 * Z mime dat, ktera prenasi pointer na objekt ziska tento objekt.
 * @param mimeData mime data prnasejici objekt
 * @return Objekt prenaseny mime daty.
 * @retval 0 @p mimeData nenesou zadny objekt.
 */
BombicMapObject * MapView::getMapObject(const QMimeData * mimeData) {
	QVariant objVar =
		mimeData->property(MAP_VIEW_DRAGGED_OBJECT_PROPERTY);
	if(!objVar.isValid()) {
		// property was not set
		return 0;
	}
	BombicMapObject * obj = static_cast<BombicMapObject *>(
		qVariantValue<void *>(objVar) );
	return obj;
}

void MapView::execDragging(QWidget * srcWidget, BombicMapObject * object) {
	QDrag * drag = new QDrag(srcWidget);
	drag->setMimeData(createMimeData(object));
	drag->setPixmap(object->thumbnail());
	drag->exec();
}
