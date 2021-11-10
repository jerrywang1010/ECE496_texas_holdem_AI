/*
    ///// scratch pad area for brain storming and TODOs /////

    game of rock paper scissors, assume 2 players

    # TO DO:
    1. move different components into different headers and classes (please add header guards)
    2. please follow fuction comment template

*/

#include <iostream>
#include "game.h"


int main()
{
	Game game(GAME_MODE::FIXED_OPP_STRAT, {0.2, 0.4, 0.4}, 1000);
	game.play();
    game.display_player_strategies();
    return 0;
}

