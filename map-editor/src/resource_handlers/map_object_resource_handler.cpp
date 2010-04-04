
#include "map_object_resource_handler.h"

#include "resource_handler.h"
#include "wall_resource_handler.h"

/** @details
 * Podle dat zadana korenovym elementem @p rootEl vytvori
 * instanci specializovaneho resource handleru, ktery umi data zpracovat.
 * Pokud nema informace o handleru, ktery by umel zpracovat zadana data,
 * tise vraci nulu (0).
 * @param rootEl korenovy element dokumentu definujici nejaky objekt mapy
 * @return Nove vytvoreny resource handler na tento druh objektu.
 * @retval 0 Neznamy druh objektu, ktery neni podporovan.
 */
MapObjectResourceHandler * MapObjectResourceHandler::createResourceHandler(
		const QDomElement & rootEl) {

	#define HANDLER(rootElTagName, className) \
		do { \
			if(rootEl.tagName()==rootElTagName) { \
				return new className; \
			} \
		} while(0)

	HANDLER("wall", WallResourceHandler);



	// here we don't know which resource handler create

	return 0;

	#undef HANDLER
}

