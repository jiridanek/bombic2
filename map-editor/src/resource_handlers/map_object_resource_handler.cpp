
#include "map_object_resource_handler.h"

#include "resource_handler.h"
#include "wall_resource_handler.h"

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
	// show error - the message about it
	RESOURCE_HANDLER->showError(
		tr("Don't know which resource handler create.")+"\n"+
		tr("The object with root element")+" "+rootEl.tagName()+" "+
		tr("may be wrong or not supported.");

	return 0;

	#undef HANDLER
}

