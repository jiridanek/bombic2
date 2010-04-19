

#include <QLinkedList>
#include <constants.h>

#include "map_resource_handler.h"

#include "../resource_handler.h"
#include "../bombic/map.h"
#include "../bombic/map_object.h"
#include "../bombic/map_background.h"
#include "../generators/map_object_generator.h"

#include "../bombic/wall.h"

/// Pocet hracu (deathmatche) nacitanych z mapy.
#define MRH_PLAYERS_COUNT 4

using namespace ResourceHandlerNS;


QString MapResourceHandler::objectsElName(
		BombicMapObject::Type objectType) {
	switch(objectType) {
		case BombicMapObject::Wall:
			return "walls";
		case BombicMapObject::Box:
			return "boxes";
		case BombicMapObject::Player:
			return "players";
		case BombicMapObject::Creature:
			return "creatures";
		case BombicMapObject::Floorobject:
			return "floorobjects";
		default:
			Q_ASSERT_X(false,
				"in MapResourceHandler::objectsElName",
				"Unhandled object type");
			return "";
	}
}

QString MapResourceHandler::positionElName(
		BombicMapObject::Type objectType) {
	switch(objectType) {
		case BombicMapObject::Wall:
			return "wall";
		case BombicMapObject::Box:
			return "box";
		case BombicMapObject::Player:
			return "player";
		case BombicMapObject::Creature:
			return "creature";
		case BombicMapObject::Floorobject:
			return "floorobject";
		default:
			Q_ASSERT_X(false,
				"in MapResourceHandler::positionElName",
				"Unhandled object type");
			return "";
	}
}

/********************** loading **********************/

/** @details
 * Vytvori mapu s defaultnim pozadim @c DEFAULT_MAP_BACKGROUND
 * a rozmery <code>DEFAULT_MAP_WIDTH, DEFAULT_MAP_HEIGHT</code>.
 * Vlastnictvi nove naalokovane mapy prechazi na volajiciho.
 * @return Nove alokovana prazdna mapa s pozadim.
 */
BombicMap * MapResourceHandler::createEmptyMap() {
	BombicMapBackground * defaultBg =
		RESOURCE_HANDLER->loadMapBackground(DEFAULT_MAP_BACKGROUND);
	if(!defaultBg) {
		return 0;
	}
	BombicMap * newMap = new BombicMap("unnamed",
		DEFAULT_MAP_WIDTH, DEFAULT_MAP_HEIGHT, defaultBg);
	if(!newMap) {
		delete defaultBg;
		return 0;
	}
	if(!loadMapPlayers(newMap)) {
		delete newMap;
		return 0;
	}
	return newMap;
}

/** @details
 * Pokusi se vytvorit mapu zadanou @p name a vlozit do ni
 * vsechny objekty.
 * @param name jmeno mapy (nebo primo cesta k souboru)
 * @return Objekt mapy.
 * @retval 0 mapu se nepodarilo vyrobit
 */
BombicMap * MapResourceHandler::createMap(const QString & name) {

	// need filename for map constructor
	QString filename = filenameFromName(name);
	if(!locateFile(filename)) {
		return 0;
	}
	QDomElement rootEl;
	if(!loadXml(filename, rootEl, true, "map")) {
		return 0;
	}

	QString bgName;
	if(!getStringAttr(rootEl, bgName, "background") ) {
		return 0;
	}
	int w, h;
	bool success =
		getIntAttr(rootEl, w, "width") &&
		getIntAttr(rootEl, h, "height");
	if(!success) {
		return 0;
	}
	BombicMapBackground * mapBg =
		RESOURCE_HANDLER->loadMapBackground(bgName);
	if(!mapBg) {
		return 0;
	}
	BombicMap * map = new BombicMap(
		rootEl.attribute("name"), w, h, mapBg, filename);
	if(!map) {
		delete mapBg;
		return 0;
	}

	// load map objects
	QDomElement el;
	success =
		getSubElement(rootEl, el, "players") &&
		loadMapPlayers(el, map) &&
		getSubElement(rootEl, el, "floorobjects", true) &&
		loadMapFloorobjects(el, map) &&
		getSubElement(rootEl, el, "walls", true) &&
		loadMapWalls(el, map) &&
		getSubElement(rootEl, el, "boxes", true) &&
		loadMapBoxes(el, map) &&
		getSubElement(rootEl, el, "creatures", true) &&
		loadMapCreatures(el, map) &&
		getSubElement(rootEl, el, "dont_generate", true) &&
		loadMapNoboxes(el, map) &&
		loadMapNocreatures(el, map);


	if(!success) {
		delete map;
		return 0;
	}

	return map;
}

#define MRH_FOREACH_SIBLING_ELEMENT(itEl, firstEl) \
	for(QDomElement itEl = firstEl; \
		!itEl.isNull() ; \
		itEl = itEl.nextSiblingElement(firstEl.tagName()) )

bool MapResourceHandler::loadMapPlayers(BombicMap * map) {
	// start of cooperative players
	BombicMapObject * player =
		RESOURCE_HANDLER->loadMapObject(COOPERATIVE_PLAYERS_NAME);
	if(!player) {
		return false;
	}
	BombicMap::Field playersField;
	// starting at 1,1 because of background walls
	for(BombicMap::Field f(1, 1) ; f.x() < map->fieldsRect().right() ;
			++f.rx()) {
		for(f.ry() = 1 ; f.y() < map->fieldsRect().bottom() ;
				++f.ry()) {
			if(map->canInsert(player, f)) {
				playersField = f;
				break;
			}
		}
		if(!playersField.isNull()) {
			break;
		}
	}
	Q_ASSERT(!playersField.isNull());
	// OK - it can be inserted, so create copy and insert it
	map->insert(player->createCopy(), playersField);

	// start of deathmatch players
	for(int i = 0 ; i < MRH_PLAYERS_COUNT ; ++i) {
		QString playerName = "player";
		playerName += QString::number(i);
		player = RESOURCE_HANDLER->loadMapObject(playerName);
		if(!player) {
			return false;
		}
		map->insert(player->createCopy(), playersField);
	}
	// all players successfull loaded
	return true;
}

bool MapResourceHandler::loadMapPlayers(const QDomElement & playersEl,
		BombicMap * map) {
	// start of cooperative players
	BombicMapObject * player =
		RESOURCE_HANDLER->loadMapObject(COOPERATIVE_PLAYERS_NAME);
	if(!player) {
		return false;
	}
	if(!insertMapObject(playersEl, player, map)) {
		return false;
	}
	// start of deathmatch players
	for(int i = 0 ; i < MRH_PLAYERS_COUNT ; ++i) {
		QString playerName = "player";
		playerName += QString::number(i);
		QDomElement playerEl;
		if(!getSubElement(playersEl, playerEl, playerName)) {
			return false;
		}
		player = RESOURCE_HANDLER->loadMapObject(playerName);
		if(!player) {
			return false;
		}
		if(!insertMapObject(playerEl, player, map)) {
			return false;
		}
	}
	// all players successfull loaded
	return true;
}

bool MapResourceHandler::loadMapFloorobjects(const QDomElement & floorsEl,
		BombicMap * map) {
	// for all object prototypes
	MRH_FOREACH_SIBLING_ELEMENT(objEl, floorsEl) {
		// object name
		QString name;
		if(!getStringAttr(objEl, name, "name")) {
			return false;
		}
		// the object
		BombicMapObject * obj = RESOURCE_HANDLER->loadMapObject(name);
		if(!obj) {
			return false;
		}
		// first position element
		QDomElement posEl;
		if(!getSubElement(objEl, posEl, "floorobject")) {
			return false;
		}
		if(!insertMapObjects(posEl, obj, map)) {
			return false;
		}
	}
	// all floor objects loaded
	return true;
}

bool MapResourceHandler::loadMapWalls(const QDomElement & wallsEl,
		BombicMap * map) {
	// for all object prototypes
	MRH_FOREACH_SIBLING_ELEMENT(objEl, wallsEl) {
		// object name
		QString name;
		if(!getStringAttr(objEl, name, "name")) {
			return false;
		}
		// the object
		BombicMapObject * obj = RESOURCE_HANDLER->loadMapObject(name);
		if(!obj) {
			return false;
		}
		// first position element
		QDomElement posEl;
		if(!getSubElement(objEl, posEl, "wall")) {
			return false;
		}
		if(!insertMapObjects(posEl, obj, map)) {
			return false;
		}
	}
	// all walls loaded
	return true;
}

bool MapResourceHandler::loadMapBoxes(const QDomElement & boxesEl,
		BombicMap * map) {
	// for all object prototypes
	MRH_FOREACH_SIBLING_ELEMENT(objEl, boxesEl) {
		// object name
		QString name;
		if(!getStringAttr(objEl, name, "name")) {
			return false;
		}
		// the object
		BombicMapObject * obj = RESOURCE_HANDLER->loadMapObject(name);
		if(!obj) {
			return false;
		}
		// first position element
		QDomElement posEl;
		if(!getSubElement(objEl, posEl, "box", true)) {
			return false;
		}
		// insert the positioned objects
		if(!insertMapObjects(posEl, obj, map)) {
			return false;
		}
		// insert the random generated
		int randomGenerated = 0;
		if(!getIntAttr(objEl, randomGenerated,
				"random_generated", true)) {
			return false;
		}
		if(randomGenerated > 0) {
			map->setGeneratedBoxesCount(obj, randomGenerated);
		}
	}
	// all boxes loaded
	return true;
}

bool MapResourceHandler::loadMapCreatures(const QDomElement & creaturesEl,
		BombicMap * map) {
	// for all object prototypes
	MRH_FOREACH_SIBLING_ELEMENT(objEl, creaturesEl) {
		// object name
		QString name;
		if(!getStringAttr(objEl, name, "name")) {
			return false;
		}
		// the object
		BombicMapObject * obj = RESOURCE_HANDLER->loadMapObject(name);
		if(!obj) {
			return false;
		}
		// first position element
		QDomElement posEl;
		if(!getSubElement(objEl, posEl, "creature", true)) {
			return false;
		}
		// insert the positioned objects
		if(!insertMapObjects(posEl, obj, map)) {
			return false;
		}
		// insert the random generated
		int randomGenerated = 0;
		if(!getIntAttr(objEl, randomGenerated,
				"random_generated", true)) {
			return false;
		}
		if(randomGenerated > 0) {
			map->setGeneratedCreaturesCount(obj, randomGenerated);
		}
	}
	// all creatures loaded
	return true;
}

bool MapResourceHandler::loadMapNoboxes(const QDomElement & dontGenerateEl,
		BombicMap * map) {
	QDomElement firstNoboxEl;
	if(!getSubElement(dontGenerateEl, firstNoboxEl, "nobox", true)) {
		return false;
	}
	MRH_FOREACH_SIBLING_ELEMENT(noboxEl, firstNoboxEl) {
		BombicMap::Field field;
		if(!getAttrsXY(noboxEl, field.rx(), field.ry())) {
			return false;
		}
		MapObjectGenerator * generator =
			map->boxGenerator(field);
		if(generator) {
			generator->disallow();
		}
	}
	return true;
}

bool MapResourceHandler::loadMapNocreatures(const QDomElement & dontGenerateEl,
		BombicMap * map) {
	QDomElement firstNocreatureEl;
	if(!getSubElement(dontGenerateEl, firstNocreatureEl, "nocreature", true)) {
		return false;
	}
	MRH_FOREACH_SIBLING_ELEMENT(nocreatureEl, firstNocreatureEl) {
		BombicMap::Field field;
		if(!getAttrsXY(nocreatureEl, field.rx(), field.ry())) {
			return false;
		}
		MapObjectGenerator * generator =
			map->creatureGenerator(field);
		if(generator) {
			generator->disallow();
		}
	}
	return true;
}

bool MapResourceHandler::insertMapObjects(const QDomElement & positionEl,
		BombicMapObject * insertedObject, BombicMap * map) {
	// for all positions
	MRH_FOREACH_SIBLING_ELEMENT(posEl, positionEl) {
		if(!insertMapObject(posEl, insertedObject, map)) {
			return false;
		}
	}
	// all positions inserted
	return true;
}

bool MapResourceHandler::insertMapObject(const QDomElement & posEl,
		BombicMapObject * insertedObject, BombicMap * map) {
	BombicMap::Field field;
	if(!getAttrsXY(posEl, field.rx(), field.ry())) {
		return false;
	}
	// try to insert
	if(!map->canInsert(insertedObject, field)) {
		showError(
			tr("Object cannot be inserted to field") +"\n"+
				"[" + QString::number(field.x()) +","+
				QString::number(field.y()) + "]",
			posEl );
		return false;
	}
	// OK - it can be inserted, so create copy and insert it
	map->insert(insertedObject->createCopy(), field);
	return true;
}

/**************************** saving **************************/

bool MapResourceHandler::saveMap(BombicMap * map) {
	MapDataT mapData;
	initMapDataToSave(mapData, map);

	QDomDocument doc;
	QDomElement rootEl = doc.createElement("map");
	doc.appendChild(rootEl);

	mapDataToXml(mapData, rootEl);

	return saveXml(map->filename(), doc);
}

/** Smycka pres vsechny policka mapy.
 * Iteruje pres vsechny policka mapy @p map promennou cyklu @p field
 * Zacina v hornim levem rohu, pokracuje pres vsechny sloupce
 * a v kazdem sloupci postupuje dolu.
 * Promenna cyklu @p field by nemela byt definovana, protoze je definovana
 * a inicializovana zde.
 * @param map mapa, jejiz policka prochazime.
 * @param field jmeno promenne cyklu
 */
#define MRH_FOREACH_FIELD(map, field) \
	for(BombicMap::Field field = map->fieldsRect().topLeft() ; \
			field.x() <= map->fieldsRect().right() ; \
			++field.rx()) \
		for(field.ry() = map->fieldsRect().top() ; \
				field.y() <= map->fieldsRect().bottom() ; \
				++field.ry())

#define MRH_FOREACH_OBJECT_KIND(typeIt, kindIt, objectsByType) \
	for( ObjectsByTypeT::const_iterator typeIt = objectsByType.begin() ; \
			typeIt != objectsByType.end() ; ++typeIt) \
		for( ObjectsByNameT::const_iterator kindIt = typeIt->begin() ; \
				kindIt != typeIt->end() ; ++kindIt)

#define MRH_FOREACH_PLAYER(it, players) \
	for( ObjectsByNameT::const_iterator it = players.begin() ; \
			it != players.end() ; ++it)

void MapResourceHandler::initMapDataToSave(MapDataT & mapData,
		BombicMap * map) {
	// init the map properties
	mapData.name = map->name();
	mapData.width = map->fieldsRect().width();
	mapData.height = map->fieldsRect().height();
	mapData.background = map->background()->name();

	// collect information from fields
	MRH_FOREACH_FIELD(map, f) {
		if(!map->boxGenerator(f)->allowed()) {
			mapData.noBoxes.append(f);
		}
		if(!map->creatureGenerator(f)->allowed()) {
			mapData.noCreatures.append(f);
		}
		foreach(BombicMapObject * o, map->objectsOnField(f)) {
			registerPlacedObject(mapData, o, f);
		}
	}
	// count the generated objects
	countGeneratedObjects(mapData.objects, map->generatedBoxes());
	countGeneratedObjects(mapData.objects, map->generatedCreatures());
}

void MapResourceHandler::registerPlacedObject(MapDataT & mapData,
		BombicMapObject * object, BombicMap::Field field) {
	if(object->field() != field) {
		// the object starts somewhere else
		return;
	}
	if(object->type() == BombicMapObject::Wall) {
		BombicWall * wall = static_cast<BombicWall *>(object);
		if(wall->isBackground()) {
			// the object is background wall
			return;
		}
	}
	if(object->type() == BombicMapObject::Player) {
		mapData.players[object->name()].positions.append(field);
		return;
	}

	// add the object position
	mapData.objects[object->type()][object->name()]
		.positions.append(field);
}

void MapResourceHandler::countGeneratedObjects(ObjectsByTypeT & objects,
		const BombicMap::ObjectListT & generatedObjects) {
	foreach(BombicMapObject * o, generatedObjects) {
		++ objects[o->type()][o->name()].generated;
	}
}

void MapResourceHandler::mapDataToXml(const MapDataT & mapData,
		QDomElement & rootEl) {
	// map attributes
	rootEl.setAttribute("name", mapData.name);
	rootEl.setAttribute("width", mapData.width);
	rootEl.setAttribute("height", mapData.height);
	rootEl.setAttribute("background", mapData.background);

	QDomDocument doc = rootEl.ownerDocument();

	// players
	QDomElement playersEl = doc.createElement(
		objectsElName(BombicMapObject::Player) );
	rootEl.appendChild(playersEl);
	MRH_FOREACH_PLAYER(p, mapData.players) {
		// right one position of each player is needed
		Q_ASSERT(p->positions.size() == 1);
		if(p.key() == "allplayers") {
			BombicMap::Field position = p->positions.first();
			playersEl.setAttribute("x", position.x());
			playersEl.setAttribute("y", position.y());
		} else {
			positionsToXml(p->positions, playersEl, p.key());
		}
	}

	// objects
	MRH_FOREACH_OBJECT_KIND(type, kind, mapData.objects) {
		QDomElement objectsEl = doc.createElement(
			objectsElName(type.key()) );
		rootEl.appendChild(objectsEl);

		objectsEl.setAttribute("name", kind.key());
		if(kind->generated != 0) {
			objectsEl.setAttribute(
				"random_generated", kind->generated);
		}
		positionsToXml(kind->positions, objectsEl,
			positionElName(type.key()) );
	}

	// dont generate fields
	if(!mapData.noBoxes.isEmpty() || !mapData.noCreatures.isEmpty()) {
		QDomElement noObjectsEl = doc.createElement("dont_generate");
		rootEl.appendChild(noObjectsEl);
		positionsToXml(mapData.noBoxes, noObjectsEl, "nobox");
		positionsToXml(mapData.noCreatures, noObjectsEl, "nocreature");
	}
}

void MapResourceHandler::positionsToXml(const PositionsT & positions,
		QDomElement & parentEl, const QString & positionElName) {

	QDomDocument doc = parentEl.ownerDocument();

	foreach(BombicMap::Field f, positions) {
		QDomElement posEl = doc.createElement(positionElName);
		parentEl.appendChild(posEl);

		posEl.setAttribute("x", f.x());
		posEl.setAttribute("y", f.y());
	}
}






