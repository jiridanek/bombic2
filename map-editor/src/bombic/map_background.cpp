
#include "map_background.h"

#include "wall.h"

BombicMapBackground::BombicMapBackground(const QString & name,
		const QPixmap & texture) : name_(name), texture_(texture) {

}

BombicMapBackground::~BombicMapBackground() {
	BombicWall * wall;
	foreach(wall, walls_) {
		delete wall;
	}
}

const QPixmap & BombicMapBackground::texture() {
	return texture_;
}

BombicWall * BombicMapBackground::getWall(
		BombicMapBackground::WallSituation wallSituation) {
	if(0 <= wallSituation && wallSituation < walls_.size()) {
		return walls_.at(wallSituation);
	} else {
		return 0;
	}
}

void BombicMapBackground::setWall(BombicWall * wall,
		BombicMapBackground::WallSituation wallSituation) {
	walls_[wallSituation] = wall;
}
