
#include <helpers/sdl.h>
#include <constants.h>
#include "game.h"
#include "mapobjects.h"
#include "bonus.h"
#include "player.h"
#include "bonus_application.h"


/**
 * @param bonus_name jméno bonusu
 * @param anim animace normálního stavu
 * @param x souřadnice levého rohu v pixelech
 * @param y souřadnice horního rohu v pixelech
 */
Bonus::Bonus(const std::string & bonus_name,
		const Animation & anim, Uint16 x, Uint16 y):
	DynamicMO(x,y), anim_(anim), visible_(false),
	bonus_name_(bonus_name) {}

/** @details
 * Jakýsi copycontructor, který navíc k okopírování objektu nastaví souřadnice.
 * @param bonus objekt k okopírování
 * @param x souřadnice levého rohu v pixelech
 * @param y souřadnice horního rohu v pixelech
 */
Bonus::Bonus(const Bonus & bonus, Uint16 x, Uint16 y):
	DynamicMO(x,y), anim_(bonus.anim_), visible_(false),
	bonus_name_(bonus.bonus_name_) {}

/** @details
 * Hlídá zviditelnění, odstranění a sežrání bonusu.
 */
bool Bonus::move(){
	Uint16 x = x_/CELL_SIZE, y = y_/CELL_SIZE;
	// pokud nejsem viditelny, zkusim jestli uz se nemam objevit
	if(!visible_ && GAME->field_canGoOver(x, y))
		visible_ = true;
	// jestli nejsem viditelny nic nedelam
	if(!visible_) return false;

	MapObject * obj =
		GAME->field_getObject(x, y, isTypeOf::isFlamePlayer);
	// necham se spalit
	if(obj && obj->type()==FLAME)
		return true;
	// necham se vzit hracem
	if(obj){
		BonusApplication::new_bonus(bonus_name_,
			static_cast< Player *>(obj));
		return true;
	}
	return false;
}

/**
 * @param window surface okna pro vykreslení
 * @param rect obdelnik mapy v okne
 */
void Bonus::draw(SDL_Surface *window, const SDL_Rect & rect){
	if(visible_)
		anim_.draw(window, x_+rect.x, y_+rect.y);
}

void Bonus::update(){
	if(visible_)
		anim_.update();
}
