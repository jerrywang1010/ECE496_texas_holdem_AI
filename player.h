#ifndef PLAYER_GUARD
#define PLAYER_GUARD

#include <iostream>
#include <string>
#include <vector>
#include <assert.h>
#include "utils.h"

class Player
{
public:

    std::string player_name;
    
    std::vector<moves> his_moves;

    std::vector<round_result> his_result;

    Player (std::string name) : player_name(name) {}

    moves play_a_round() const;

    void update_move (moves move);

    void update_result (round_result result);

    moves get_last_move () const;

    round_result get_last_round_result () const;
};

#endif
