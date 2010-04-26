
#include "map_object_single_generator.h"

/** @details
 * @param field policko mapy, pro ktere chceme generovat
 */
MapObjectSingleGenerator::MapObjectSingleGenerator(
		const BombicMap::Field & field):
				MapObjectGenerator(field) {

}

/**
 * @return Zda lze aktualne generovat objekt.
 */
bool MapObjectSingleGenerator::canGenerate() {
	return MapObjectGenerator::canGenerate() &&
		generatedObjects_.isEmpty();
}

/**
 * @param mapObject pridavany objekt
 */
void MapObjectSingleGenerator::addGeneratedObject(
		BombicMapObject * mapObject) {
	MapObjectGenerator::addGeneratedObject(mapObject);
	emit canGenerateChanged();
}

