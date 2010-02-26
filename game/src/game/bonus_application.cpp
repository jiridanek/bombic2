
#include <algorithm>
#include <helpers/sdl.h>
#include <helpers/stl.h>
#include <config.h>
#include "game.h"
#include "player.h"
#include "bonus_application.h"


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
	if(bonus_name==BonusIllness::name())
		return BonusIllness::new_illness(player);
	if(bonus_name==BonusOthersIllness::name())
		return new BonusOthersIllness(player);

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
	if(player->flamesize_ < FLAMESIZE_MAX)
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
	++player->boots_;
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
	GAME->tools->draw_panel_bonus(
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
	GAME->tools->draw_panel_bonus(
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
		GAME->remove_bombs_timer(player_->player_num());
		return true;
	}

	--remaining_periods_;
	return false;
}

void BonusTimer::draw_panel(SDL_Surface *window,
					const SDL_Rect & rect) const {
	GAME->tools->draw_panel_bonus(
		window, rect, player_->player_num(), GameTools::TIMER,
		x2string( 100*remaining_periods_/BONUS_TIMER_PERIODS)+"%" );
}

/********* class BonusOtherIllness ************************/

BonusOthersIllness::BonusOthersIllness(Player * player):
				BonusApplication(player){
	GAME->add_others_bonus(player->player_num(), BonusIllness::name());
}

/********* class BonusIllness ************************/

BonusIllness * BonusIllness::new_illness(Player * player){
	switch(rand()%LAST_){
		case CONFUSED: return new BonusIllnessConfused(player);
		case STOP: return new BonusIllnessStop(player);
		case SLOW: return new BonusIllnessSlow(player);
		case FAST: return new BonusIllnessFast(player);
		default: return new BonusIllness(player);
	}
}

BonusIllness::BonusIllness(Player * player):
				BonusApplication(player), remaining_periods_(BONUS_ILLNESS_PERIODS){
	// vyhodit starou nemoc
	Player::bonuses_t::iterator it;
	for(it= player_->bonuses_.begin() ; it!=player_->bonuses_.end() ; ++it){
		if( type()==(*it)->type() && this!=(*it)){
			delete *it;
			player_->bonuses_.erase(it);
			break;
		}
	}
}

bool BonusIllness::update() {
	if(!remaining_periods_)
		return true;

	--remaining_periods_;
	return false;
}

void BonusIllness::draw_panel(SDL_Surface *window,
					const SDL_Rect & rect) const {
	GAME->tools->draw_panel_bonus(
		window, rect, player_->player_num(), GameTools::ILLNESS,
		x2string( 100*remaining_periods_/BONUS_ILLNESS_PERIODS)+"%" );
}

/******* bonus illness CONFUSED ********/
BonusIllnessConfused::BonusIllnessConfused(Player * player):
			BonusIllness(player) {
	Config::players_t::value_type & keys =
		CONFIG->players_[player_->player_num()];
	std::swap(keys[KEY_UP], keys[KEY_DOWN]);
	std::swap(keys[KEY_LEFT], keys[KEY_RIGHT]);
}

BonusIllnessConfused::~BonusIllnessConfused(){
	Config::players_t::value_type & keys =
		CONFIG->players_[player_->player_num()];
	std::swap(keys[KEY_UP], keys[KEY_DOWN]);
	std::swap(keys[KEY_LEFT], keys[KEY_RIGHT]);
}

/****** bonus illness STOP ************/
BonusIllnessStop::BonusIllnessStop(Player * player):
			BonusIllness(player), old_speed_(player->speed_diff_) {
	player_->speed_diff_ = 0;
}
BonusIllnessStop::~BonusIllnessStop(){
	player_->speed_diff_ = old_speed_;
}
/****** bonus illness SLOW ************/
BonusIllnessSlow::BonusIllnessSlow(Player * player):
			BonusIllnessStop(player){
	player_->speed_diff_ = old_speed_/2;
}
/****** bonus illness FAST ************/
BonusIllnessFast::BonusIllnessFast(Player * player):
			BonusIllnessStop(player){
	player_->speed_diff_ = old_speed_*3;
}
