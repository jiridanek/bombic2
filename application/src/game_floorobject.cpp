
#include "SDL_lib.h"
#include "game_mapobjects.h"
#include "game_floorobject.h"

/**
 * @param anim animace objektu
 * @param x souřadnice levého rohu v pixelech
 * @param y souřadnice pravého rohu v pixelech
 */
Floorobject::Floorobject(const Animation & anim, Uint16 x, Uint16 y):
	StaticMO(x, y), anim_(anim) {}

Floorobject::Floorobject(const Floorobject & floorobject, Uint16 x, Uint16 y):
	StaticMO(x, y), anim_(floorobject.anim_) {}

/** @details
 * Aby ležel nad presumpcí plamene.
 * Nemusíme se bát, že by ležel nad ostatními,
 * je totiž definován jako typ pozadí.
 * @see DynamicMO::getZ()
 * @see isTypeOf::isBgType
 */
Uint16 Floorobject::getZ() const {
	return y_+1;
}

/**
 * @param window surface okna pro vykreslení
 */
void Floorobject::draw(SDL_Surface* window, const SDL_Rect & rect){
	if(rect.w >= x_+anim_.width()
	&& rect.h >= y_+anim_.height())
		anim_.draw(window, x_ +rect.x, y_ +rect.y);
}

void Floorobject::update(){
	anim_.update();
}
