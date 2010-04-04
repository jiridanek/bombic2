
#include "map_background.h"

#include "wall.h"

/**
 * @param name nazev pozadi
 * @param texture obrazek pozadi policka
 */
BombicMapBackground::BombicMapBackground(const QString & name,
		const QPixmap & texture) : name_(name), texture_(texture) {

}

/** @details
 * Dealokuje prototypy obvodovych zdi.
 */
BombicMapBackground::~BombicMapBackground() {
	BombicWall * wall;
	foreach(wall, walls_) {
		if(wall) {
			delete wall;
		}
	}
}

/**
 * @return Obrazek pozadi policka.
 */
const QPixmap & BombicMapBackground::texture() {
	return texture_;
}
/**
 * @return Barvu pozadi okolo mapy.
 */
QColor BombicMapBackground::ambientColor() {
	BombicWall * topLeftWall = getWall(TopLeft);
	if(topLeftWall) {
		return topLeftWall->pixmap().toImage().pixel(0,0);
	} else {
		return Qt::black;
	}
}

/** @details
 * Vraceny pointer zustava ve vlastnictvi pozadi
 * a bude dealokovan v destruktoru pozadi.
 * @param wallSituation pozice obvodove zdi
 * @return Obvodova zed.
 * @retval 0 Zed na zadane pozici nebyla nastavena.
 */
BombicWall * BombicMapBackground::getWall(
		BombicMapBackground::WallSituation wallSituation) {
	if(walls_.contains(wallSituation)) {
		return walls_.value(wallSituation);
	} else {
		return 0;
	}
}

/** @details
 * Pointer @p wall prechazi do vlastnictvi pozadi a bude dealokovan
 * v destruktoru pozadi.
 * @param wall vkladana zed
 * @param wallSituation pozice vkladane zdi
 */
void BombicMapBackground::setWall(BombicWall * wall,
		BombicMapBackground::WallSituation wallSituation) {
	walls_.insert(wallSituation, wall);
}
