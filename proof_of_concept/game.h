#ifndef GAME_GUARD
#define GAME_GUARD

#include <iostream>
#include <string>
#include <stdio.h>
#include <cstring>
#include "art.h"
#include "player.h"

class Game
{

public:
    Game (int num_players, GAME_MODE mode, int num_iteration);
    Game (GAME_MODE mode, int num_iteration);
    Game (GAME_MODE mode, std::vector<double> opp_strategy, int num_iteration);

    void play ();

    round_result find_result(moves move_1, moves move_2) const;

    void display_game_board () const;

    void display_player_strategies () const;

private:
    std::vector<Player> players;
    int num_players;
    GAME_MODE mode;
    std::vector<double> opp_strategy;
    int num_iteration;
};

#endif
