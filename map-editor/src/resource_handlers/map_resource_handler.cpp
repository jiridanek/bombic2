
#include <constants.h>

#include "map_resource_handler.h"

#include "resource_handler_functions.h"

#include "../resource_handler.h"
#include "../bombic/map.h"
#include "../bombic/map_object.h"
#include "../bombic/map_background.h"
#include "../generators/map_object_generator.h"

#include "../bombic/wall.h"

/// Pocet hracu (deathmatche) nacitanych z mapy.
#define MRH_PLAYERS_COUNT 4

using namespace ResourceHandlerFunctions;


/**
 * @param objectType typ objektu, ktery je elementem definovat
 * @return Jmeno elementu, ktery definuje dany typ.
 */
QString MapResourceHandler::objectsElName(BombicMapObject::Type objectType) {
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

/**
 * @param objectType typ objektu, ktery je elementem pozicovan
 * @return Jmeno elementu, ktery pozicuje dany typ.
 */
QString MapResourceHandler::positionElName(BombicMapObject::Type objectType) {
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
 * Vlozi do mapy hrace na prvni vhodnou pozici.
 * Vlastnictvi nove naalokovane mapy prechazi na volajiciho.
 * Nastavi mape ze byla ulozena (nebyla editovana).
 * @return Nove alokovana prazdna mapa s pozadim.
 */
BombicMap * MapResourceHandler::createEmptyMap() {
	// create background
	BombicMapBackground * defaultBg =
		RESOURCE_HANDLER->loadMapBackground(DEFAULT_MAP_BACKGROUND);
	if(!defaultBg) {
		return 0;
	}
	// create map
	BombicMap * newMap = new BombicMap("unnamed",
		DEFAULT_MAP_WIDTH, DEFAULT_MAP_HEIGHT, defaultBg);
	if(!newMap) {
		delete defaultBg;
		return 0;
	}
	// insert players
	if(!loadMapPlayers(newMap)) {
		delete newMap;
		return 0;
	}
	// to aviod need save at closing
	newMap->setModified(false);
	return newMap;
}

/** @details
 * Pokusi se vytvorit mapu zadanou @p name a vlozit do ni
 * vsechny objekty.
 * Nastavi mape ze byla ulozena (nebyla editovana).
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
	// get the xml definition of map
	QDomElement rootEl;
	if(!loadXml(filename, rootEl, true, "map")) {
		return 0;
	}
	// and its properties
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
	// create background
	BombicMapBackground * mapBg =
		RESOURCE_HANDLER->loadMapBackground(bgName);
	if(!mapBg) {
		return 0;
	}
	// and the map
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
		loadMapObjects(rootEl, BombicMapObject::Floorobject, map) &&
		loadMapObjects(rootEl, BombicMapObject::Wall, map) &&
		loadMapObjects(rootEl, BombicMapObject::Box, map) &&
		loadMapObjects(rootEl, BombicMapObject::Creature, map) &&
		getSubElement(rootEl, el, "dont_generate", true) &&
		loadMapNoboxes(el, map) &&
		loadMapNocreatures(el, map);


	if(!success) {
		delete map;
		return 0;
	}

	// to aviod need save at closing
	map->setModified(false);
	return map;
}

/** Cyklus pres vsechny sousedni elementy stejneho jmena.
 * Vytvori cyklus zacinajici @p firstEl, ktery postupne
 * projde vsechny sousedni (sibling) elementy stejneho jmena ( @c tagName() )
 * jako @p firstEl.
 * Promenna cyklu @p itEl by nemela byt definovana, protoze je definovana
 * a inicializovana zde.
 * @param itEl jmeno iteracni promenne cyklu
 * @param firstEl prvni (inicializacni) element
 */
#define MRH_FOREACH_SIBLING_ELEMENT(itEl, firstEl) \
	for(QDomElement itEl = firstEl; \
		!itEl.isNull() ; \
		itEl = itEl.nextSiblingElement(firstEl.tagName()) )

/** @details
 * Nacte hrace mapy a vlozi je na prvni vhodne misto.
 * Hodi se zejmena pro vytvareni prazdne mapy.
 * @param map mapa, do ktere chceme hrace vlozit
 * @return Uspech akce.
 */
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

/** @details
 * Nacte hrace mapy a vlozi je do mapy.
 * Misto vlozeni definuje @p playersEl.
 * @param playersEl element definujici pozice hracu
 * @param map mapa, do ktere chceme hrace vlozit
 * @return Uspech akce.
 */
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

/** @details
 * Nacte objekty mapy daneho typu a vlozi je do mapy.
 * @param rootEl korenovy element definice mapy
 * @param type typ objektu
 * @param map mapa, do ktere chceme objekty vlozit
 * @return Uspech akce.
 */
bool MapResourceHandler::loadMapObjects(const QDomElement & rootEl,
		BombicMapObject::Type type, BombicMap * map) {
	QDomElement objectsEl;
	if(!getSubElement(rootEl, objectsEl, objectsElName(type), true)) {
		return false;
	}
	// for all object prototypes
	MRH_FOREACH_SIBLING_ELEMENT(objEl, objectsEl) {
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
		if(!getSubElement(objEl, posEl, positionElName(type), true)) {
			return false;
		}
		if(!insertMapObjects(posEl, obj, map)) {
			return false;
		}

		// get random generated count
		int randomGenerated = 0;
		if(!getIntAttr(objEl, randomGenerated,
				"random_generated", true)) {
			return false;
		}
		if(randomGenerated > 0) {
			// insert random generated
			switch(type) {
				case BombicMapObject::Box:
					map->setGeneratedBoxesCount(
						obj, randomGenerated);
					break;
				case BombicMapObject::Creature:
					map->setGeneratedCreaturesCount(
						obj, randomGenerated);
					break;
				default:
					// do nothing - other types cannot be generated
					break;
			}
		}
	}
	// all objects loaded
	return true;
}

/** @details
 * Zakaze v mape policka, na ktery se nesmi generovat bedny.
 * @param dontGenerateEl element definice zakazanych policek
 * @param map mapa, ve ktere chceme generovani zakazovat
 * @return Uspech akce.
 */
bool MapResourceHandler::loadMapNoboxes(const QDomElement & dontGenerateEl,
		BombicMap * map) {
	// the first nobox
	QDomElement firstNoboxEl;
	if(!getSubElement(dontGenerateEl, firstNoboxEl, "nobox", true)) {
		return false;
	}
	// over all noboxes
	MRH_FOREACH_SIBLING_ELEMENT(noboxEl, firstNoboxEl) {
		// get the field
		BombicMap::Field field;
		if(!getAttrsXY(noboxEl, field.rx(), field.ry())) {
			return false;
		}
		// and disallow the generator
		MapObjectGenerator * generator =
			map->boxGenerator(field);
		if(generator) {
			generator->disallow();
		}
	}
	return true;
}

/** @details
 * Zakaze v mape policka, na ktery se nesmi generovat prisery.
 * @param dontGenerateEl element definice zakazanych policek
 * @param map mapa, ve ktere chceme generovani zakazovat
 * @return Uspech akce.
 */
bool MapResourceHandler::loadMapNocreatures(const QDomElement & dontGenerateEl,
		BombicMap * map) {
	// the first nobox
	QDomElement firstNocreatureEl;
	if(!getSubElement(dontGenerateEl, firstNocreatureEl, "nocreature", true)) {
		return false;
	}
	// over all noboxes
	MRH_FOREACH_SIBLING_ELEMENT(nocreatureEl, firstNocreatureEl) {
		// get the field
		BombicMap::Field field;
		if(!getAttrsXY(nocreatureEl, field.rx(), field.ry())) {
			return false;
		}
		// and disallow the generator
		MapObjectGenerator * generator =
			map->creatureGenerator(field);
		if(generator) {
			generator->disallow();
		}
	}
	return true;
}

/** @details
 * Na pozice (zacinajici @p positionEl a dalsi)
 * vlozi kopie objektu (pokud to lze).
 * @param positionEl prvni pozicni element
 * @param insertedObject prototyp vkladaneho objektu
 * @param map mapa, do ktere chceme objekty umistit
 * @return Uspech akce.
 */
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

/** @details
 * Na pozici urcenou @p posEl vlozi kopii objektu (pokud to lze).
 * Pokud nelze objekt vlozit, zobrazi chybove hlaseni a selze.
 * @param posEl pozicni element
 * @param insertedObject prototyp vkladaneho objektu
 * @param map mapa, do ktere chceme objekt umistit
 * @return Uspech akce.
 */
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
// 		return false;
	}
	// OK - it can be inserted, so create copy and insert it
	map->insert(insertedObject->createCopy(), field);
	return true;
}

/**************************** saving **************************/

/** @details
 * Ulozi mapu do jejiho souboru ( BombicMap::filename() ).
 * Z mapy ziska data pro ulozeni, vytvori xml dokument a ten nasledne
 * ulozi do souboru. Pokud se akce povede, nastavi mape ze byla ulozena.
 * @param map mapa, kterou chceme ulozit
 * @return Uspech akce.
 */
bool MapResourceHandler::saveMap(BombicMap * map) {
	MapDataT mapData;
	initMapDataToSave(mapData, map);

	QDomDocument doc;
	QDomElement rootEl = doc.createElement("map");
	doc.appendChild(rootEl);

	mapDataToXml(mapData, rootEl);

	if(saveXml(map->filename(), doc)) {
		map->setModified(false);
		return true;
	} else {
		return false;
	}
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

/** Smycka pres vsechny druhy objektu.
 * Iteruje pres vsechny typy objektu v @p objectsByType (promenna @p typeIt )
 * a v kazdem typu pres vsechny druhy objektu (ridici promenna @p kindIt ).
 * Promenne cyklu @p typeIt a @p kindIt by nemely byt definovany,
 * protoze jsou definovany a inicializovana zde. Jsou definovany jako
 * iteratory do hash tabulky, takze pouzijte nasledujici konstrukce:
 *	* typ objektu: @c typeIt.key()
 *	* druh (jmeno) objektu: @c kindIt.key()
 *	* pozice umisteni objektu: @c kindIt->positions
 *	* pocet generovanych objektu: @c kindIt->generated
 * @param typeIt jmeno ridici promenne prvniho cyklu pres typy objektu
 * @param kindIt jmeno ridici promenne druheho cyklu pres druhy objektu
 * @param objectsByTyp struktura objektu podle typu pres kterou iterujeme
 */
#define MRH_FOREACH_OBJECT_KIND(typeIt, kindIt, objectsByType) \
	for( ObjectsByTypeT::const_iterator typeIt = objectsByType.begin() ; \
			typeIt != objectsByType.end() ; ++typeIt) \
		for( ObjectsByNameT::const_iterator kindIt = typeIt->begin() ; \
				kindIt != typeIt->end() ; ++kindIt)

/** Smycka pres vsechny hrace.
 * Iteruje pres vsechny hrace v @p players
 * Promenna cyklu @p it by nemela byt definovana,
 * protoze je definovana a inicializovana zde. Je definovana jako
 * iterator do hash tabulky, takze pouzijte nasledujici konstrukce:
 *	* druh (jmeno) hrace: @c it.key()
 *	* pozice (seznam) umisteni objektu: @c it->positions
 *	* pozice (jedna) umisteni objektu: @c it->positions.first()
 * @param it jmeno ridici promenne
 * @param players hraci podle jmena, pres ktere iterujeme
 */
#define MRH_FOREACH_PLAYER(it, players) \
	for( ObjectsByNameT::const_iterator it = players.begin() ; \
			it != players.end() ; ++it)

/** @details
 * Ziska z mapy @p map relevantni data a ulozi je do @p mapData.
 * @param[out] mapData data mapy pro ulozeni
 * @param map mapa, kterou chceme ulozit
 */
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

/** @details
 * Pokud je to vhodne, zaregistruje umisteni objektu @p object na policko @p field.
 * Resi problem rozlehlych objektu, obvodovych zdi a hracu (ty uklada jinam).
 * @param[out] mapData data mapy pro ulozeni
 * @param object objekt, ktery registrujeme
 * @param field policko, na ktere jej pripadne registrujeme
 */
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

/** @details
 * Nastavi pocty generovanych objektu.
 * @param[out] objects objekty mapy
 * @param generatedObjects seznam generovanych objektu mapy
 */
void MapResourceHandler::countGeneratedObjects(ObjectsByTypeT & objects,
		const BombicMap::ObjectListT & generatedObjects) {
	foreach(BombicMapObject * o, generatedObjects) {
		++ objects[o->type()][o->name()].generated;
	}
}

/** @details
 * Nastavi atributy a vytvori podstrom korenoveho elementu definice mapy.
 * @p rootEl by mel byt inicializovan a mel by patrit do platneho dokumentu.
 * @param mapData data mapy k ulozeni
 * @param[in/out] rootEl korenovy element definice mapy
 */
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

/** @details
 * Vytvori podstrom pozic elementu definujiciho objekt.
 * @p parentEl by mel byt inicializovan a mel by patrit do platneho dokumentu.
 * @param positions seznam pozic objektu
 * @param mapData data mapy k ulozeni
 * @param[in/out] parentEl element definujici objekt mapy
 * @param positionElName nazev elementu pozice
 */
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
