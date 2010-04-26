
#include "map_object_resource_handler.h"

#include "resource_handler.h"
#include "wall_resource_handler.h"
#include "box_resource_handler.h"
#include "creature_resource_handler.h"
#include "player_resource_handler.h"
#include "floorobject_resource_handler.h"
#include "bonus_resource_handler.h"

MapObjectResourceHandler::RHListT MapObjectResourceHandler::resourceHandlers_;

/** @details
 * Vytvori instance vsech registrovanych specialnich resource handleru
 * a ulozi si je pro pozdejsi vydavani. Handlery jsou registrovany
 * primo v teto fci a nikde jinde.
 * Inicializace muze byt vyvolana vicekrat, dalsi volani nemaji zadny efekt.
 */
void MapObjectResourceHandler::initResourceHandlers() {
	if(!resourceHandlers_.isEmpty()) {
		// it was already initialized
		return;
	}

	#define REGISTER_HANDLER(className) \
		resourceHandlers_.append(new className)

	// {---  Register your additional handler here  ---} //
	REGISTER_HANDLER(BonusResourceHandler);
	REGISTER_HANDLER(FloorobjectResourceHandler);
	REGISTER_HANDLER(PlayerResourceHandler);
	REGISTER_HANDLER(CreatureResourceHandler);
	REGISTER_HANDLER(BoxResourceHandler);
	REGISTER_HANDLER(WallResourceHandler);

	#undef REGISTER_HANDLER
}

/** @details
 * Podle dat zadanych korenovym elementem @p rootEl najde specializovany
 * resource handler, ktery umi data zpracovat.
 * Pokud nema informace o handleru, ktery by umel zpracovat zadana data,
 * tise vraci nulu (0).
 * Vraceny pointer zustava ve vlastnictvi @c MapObjectResouceHandler
 * a nebude nikdy dealokovan (jeho platnost zustava do konce aplikace).
 * @param rootEl korenovy element dokumentu definujici nejaky objekt mapy
 * @return Resource handler na tento druh objektu.
 * @retval 0 Neznamy druh objektu, ktery neni podporovan (nebo registrovan).
 */
MapObjectResourceHandler * MapObjectResourceHandler::resourceHandler(
		const QDomElement & rootEl) {

	foreach(RHListT::value_type rh, resourceHandlers_) {
		if(rh->canHandle(rootEl)) {
			return rh;
		}
	}

	// here we don't know which resource handler create

	return 0;
}

