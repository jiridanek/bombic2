
#include <constants.h>
#include <config.h>
#include "mapobjects.h"
#include "presumption.h"

/**
 * @param anim animace
 * @param x souřadnice levého rohu políčka v pixelech
 * @param y souřadnice horního rohu v pixelech
 */
Presumption::Presumption(Animation * anim, Uint16 x, Uint16 y,
				Uint16 to_flame):
	DynamicMO(x, y), anim_(anim), periods_to_flame_(to_flame) {}

/** @details
 * Sníží počet period do výbuchu.
 * @return Vrací FALSE.
 */
bool Presumption::move(){
	if(periods_to_flame_)
		--periods_to_flame_;
	return false;
}

/**
 * @param window surface okna pro vykreslení
 */
void Presumption::draw(SDL_Surface *window, const SDL_Rect & rect){
	if(CONFIG->visible_presumption())
		anim_->draw(window, x_ +rect.x, y_ +rect.y);
	/*/ DEBUG
	extern Fonts * g_font;
	Surface text = get_text((*g_font)[10],
		x2string(periods_to_flame_).c_str(), Color::white);
	draw_surface(x_ +rect.x +CELL_SIZE/2, y_ +rect.y +CELL_SIZE/2,
		text.getSurface(), window);
	// */
}

/** @details
 * Náhodně zvolí frame animace.
 */
void Presumption::update(){
	if(CONFIG->visible_presumption())
		anim_->random();
}
