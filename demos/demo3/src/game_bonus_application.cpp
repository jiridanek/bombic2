
#include <iostream>
#include "SDL_lib.h"
#include "stl_helper.h"
#include "game.h"
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
	if(bonus_name==BonusSpeed::name())
		return new BonusSpeed(player);
	if(bonus_name==BonusLive::name())
		return new BonusLive(player);
	if(bonus_name==BonusKicker::name())
		return new BonusKicker(player);
	if(bonus_name==BonusSlider::name())
		return new BonusSlider(player);
	if(bonus_name==BonusShield::name())
		return new BonusShield(player);
	if(bonus_name==BonusFireman::name())
		return new BonusFireman(player);
	if(bonus_name==BonusTimer::name())
		return new BonusTimer(player);

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

/********* class BonusSpeed ************************/

BonusSpeed::BonusSpeed(Player * player):
				BonusApplication(player){
	++player->speed_rate_;
}

/********* class BonusLive ************************/

BonusLive::BonusLive(Player * player):
				BonusApplication(player){
	++player->lives_;
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

/********* class BonusShield ************************/

BonusShield::BonusShield(Player * player):
				BonusApplication(player), remaining_periods_(BONUS_SHIELD_PERIODS){
	// vyhodit stary stit
	Player::bonuses_t::iterator it;
	for(it= player_->bonuses_.begin() ; it!=player_->bonuses_.end() ; ++it){
		if( type()==(*it)->type() && this!=(*it)){
			delete *it;
			player_->bonuses_.erase(it);
			break;
		}
	}

	player_->last_die_ = 0;
}

BonusShield::~BonusShield() {
	player_->last_die_ = CREATURE_PROTECTION_LENGTH/3;
}

bool BonusShield::update() {
	if(!remaining_periods_)
		return true;

	--remaining_periods_;
	player_->last_die_ = 0;

	return false;
}

void BonusShield::draw_panel(SDL_Surface *window,
					const SDL_Rect & rect) const {
	Game::get_instance()->tools->draw_panel_bonus(
		window, rect, player_->player_num(), GameTools::SHIELD,
		x2string( 100*remaining_periods_/BONUS_SHIELD_PERIODS)+"%" );
}

/********* class BonusFireman ************************/

BonusFireman::BonusFireman(Player * player):
				BonusApplication(player), remaining_periods_(BONUS_FIREMAN_PERIODS){
	// vyhodit stareho firemana
	Player::bonuses_t::iterator it;
	for(it= player_->bonuses_.begin() ; it!=player_->bonuses_.end() ; ++it){
		if( type()==(*it)->type() && this!=(*it)){
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

void BonusFireman::draw_panel(SDL_Surface *window,
					const SDL_Rect & rect) const {
	Game::get_instance()->tools->draw_panel_bonus(
		window, rect, player_->player_num(), GameTools::FIREMAN,
		x2string( 100*remaining_periods_/BONUS_FIREMAN_PERIODS)+"%" );
}

/********* class BonusTimer ************************/

BonusTimer::BonusTimer(Player * player):
				BonusApplication(player), remaining_periods_(BONUS_TIMER_PERIODS){
	// vyhodit stary timer
	Player::bonuses_t::iterator it;
	for(it= player_->bonuses_.begin() ; it!=player_->bonuses_.end() ; ++it){
		if( type()==(*it)->type() && this!=(*it)){
			delete *it;
			player_->bonuses_.erase(it);
			break;
		}
	}

	player_->bonus_timer_ = true;
}

BonusTimer::~BonusTimer() {
	player_->bonus_timer_ = false;
}

bool BonusTimer::update() {
	if(!remaining_periods_){
		Game::get_instance()->remove_bombs_timer(player_->player_num());
		return true;
	}

	--remaining_periods_;
	return false;
}

void BonusTimer::draw_panel(SDL_Surface *window,
					const SDL_Rect & rect) const {
	Game::get_instance()->tools->draw_panel_bonus(
		window, rect, player_->player_num(), GameTools::TIMER,
		x2string( 100*remaining_periods_/BONUS_TIMER_PERIODS)+"%" );
}
