

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

bool MapResourceHandler::saveMap(BombicMap * map) {
	/*
	MapToSaveT mapToSave(map);
	if(!initMapToSave(mapToSave,))
QDomDocument doc("MyML");
 QDomElement root = doc.createElement("MyML");
 doc.appendChild(root);

 QDomElement tag = doc.createElement("Greeting");
 root.appendChild(tag);

 QDomText t = doc.createTextNode("Hello World");
 tag.appendChild(t);

 QString xml = doc.toString();

	QFile file(filename);

	*/

	return true;
}
