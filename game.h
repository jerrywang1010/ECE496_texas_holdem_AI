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

private:
    std::vector<Player> players;
    int num_players;

public:
    Game (int num_players);

    void play ();

    round_result find_result(moves move_1, moves move_2) const;

    void display_game_board () const;
};

#endif