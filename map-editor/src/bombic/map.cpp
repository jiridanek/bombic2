
#include "map.h"

#include "map_background.h"
#include "map_object.h"

BombicMap::BombicMap(int width, int height,
		BombicMapBackground * background):
			width_(width), height_(height),
			background_(background) {

}

BombicMap::~BombicMap() {
}
