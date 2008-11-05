
#include <iostream>
#include "SDL_lib.h"
#include "stl_helper.h"
#include "game_player.h"
#include "game_bonus_application.h"


BonusApplication * BonusApplication::new_bonus(
				const std::string & bonus_name, Player * player){
	if(bonus_name==BonusFlame::name())
		return new BonusFlame(player);
	if(bonus_name==BonusBomb::name())
		return new BonusBomb(player);
	if(bonus_name==BonusMegabomb::name())
		return new BonusMegabomb(player);
	if(bonus_name==BonusKicker::name())
		return new BonusKicker(player);
	if(bonus_name==BonusSlider::name())
		return new BonusSlider(player);
	if(bonus_name==BonusFireman::name())
		return new BonusFireman(player);

	return new BonusApplication(player);
}

BonusApplication::BonusApplication(Player * player):
			player_(player) {
	// odalokovat v Player::update()
	// nebo nejpozdeji v Player::~Player()
	player_->bonuses_.push_back(this);
}

/********* class BonusFlame ************************/

BonusFlame::BonusFlame(Player * player):
				BonusApplication(player){
	++player->flamesize_;
}

/********* class BonusMegabomb ************************/

BonusBomb::BonusBomb(Player * player):
				BonusApplication(player){
	++player->bombs_;
}

/********* class BonusMegabomb ************************/

BonusMegabomb::BonusMegabomb(Player * player):
				BonusApplication(player){
	player->megabombs_ += BONUS_MEGABOMB_COUNT;
}

/********* class BonusKicker ************************/

BonusKicker::BonusKicker(Player * player):
				BonusApplication(player){
	player->bonus_kicker_ = true;
}

/********* class BonusMegabomb ************************/

BonusSlider::BonusSlider(Player * player):
				BonusApplication(player){
	player->bonus_slider_ = true;
}

/********* class BonusFireman ************************/

BonusFireman::BonusFireman(Player * player):
				BonusApplication(player), remaining_periods_(BONUS_FIREMAN_PERIODS){
	// vyhodit stareho firemana
	Player::bonuses_t::iterator it;
	std::string my_name(name());
	for(it= player_->bonuses_.begin() ; it!=player_->bonuses_.end() ; ++it){
		if( my_name==(*it)->name()){
			delete *it;
			player_->bonuses_.erase(it);
			break;
		}
	}

	player_->bonus_fireman_ = true;
	player_->last_die_ = 0;
}

BonusFireman::~BonusFireman() {
	player_->bonus_fireman_ = false;
	player_->last_die_ = CREATURE_PROTECTION_LENGTH/2;
}

bool BonusFireman::update() {
	if(!remaining_periods_)
		return true;

	--remaining_periods_;
	player_->last_die_ = 0;
	return false;
}

void BonusFireman::draw_panel(SDL_Surface *window) const {
	Game::get_instance()->tools->draw_panel_bonus(
		window, player_->player_num(), GameTools::FIREMAN,
		x2string( 100*remaining_periods_/BONUS_FIREMAN_PERIODS)+"%" );
}
