
#include "menu_newgame.h"

int MenuNewGame::players_count = MENU_NEWGAME_PLAYERS_MIN;
int MenuNewGame::episode= MENU_NEWGAME_EPISODE_MIN;

MenuNewGame::MenuNewGame(){
	AG_Box * item;
	AG_Label * label;

	// nadpis
	createHeading("New Game");

	// hrat hru
	item = createItem("Play game");
// 	AG_SetEvent(item, "window-mousebuttondown", TODO, 0);

	// pocet hracu
	item = createItem(0);
	AG_AddEvent(item, "window-keydown", handlerPlayersCount, 0);

	item = AG_BoxNewHoriz(item, AG_BOX_HOMOGENOUS | AG_BOX_HFILL);

	label = AG_LabelNewString(item, 0, "Players");
	AG_LabelJustify(label, AG_TEXT_CENTER);

	AG_NumericalNewIntR(item, 0, 0, 0, &players_count,
		MENU_NEWGAME_PLAYERS_MIN, MENU_NEWGAME_PLAYERS_MAX);

	// epizoda
	item = createItem(0);
	AG_AddEvent(item, "window-keydown", handlerEpisode, 0);

	item = AG_BoxNewHoriz(item, AG_BOX_HOMOGENOUS | AG_BOX_HFILL);

	label = AG_LabelNewString(item, 0, "Episode");
	AG_LabelJustify(label, AG_TEXT_CENTER);

	AG_NumericalNewIntR(item, 0, 0, 0, &episode,
		MENU_NEWGAME_EPISODE_MIN, MENU_NEWGAME_EPISODE_MAX);


	// back
	item = createItem("Back");
	AG_SetEvent(item, "window-mousebuttondown", handlerBack, 0);

	AG_SpacerNewHoriz(win);
}

void MenuNewGame::handlerPlayersCount(AG_Event * event){
	switch(AG_SDLKEY(1)){
		case SDLK_RIGHT:
			if(players_count < MENU_NEWGAME_PLAYERS_MAX)
				++players_count;
			break;
		case SDLK_LEFT:
			if(players_count > MENU_NEWGAME_PLAYERS_MIN)
				--players_count;
			break;
		case SDLK_RETURN:
		case SDLK_KP_ENTER:
		case SDLK_SPACE:
			++players_count;
			if(players_count > MENU_NEWGAME_PLAYERS_MAX)
				players_count = MENU_NEWGAME_PLAYERS_MIN;
		default:
			break;
	}
}

void MenuNewGame::handlerEpisode(AG_Event * event){
	switch(AG_SDLKEY(1)){
		case SDLK_RIGHT:
			if(episode < MENU_NEWGAME_EPISODE_MAX)
				++episode;
			break;
		case SDLK_LEFT:
			if(episode > MENU_NEWGAME_EPISODE_MIN)
				--episode;
			break;
		default:
			break;
	}
}
