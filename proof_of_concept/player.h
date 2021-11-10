#ifndef PLAYER_GUARD
#define PLAYER_GUARD

#include <iostream>
#include <string>
#include <vector>
#include <assert.h>
#include "RPSTrainer.h"

class Player
{

private:
    RPSTrainer* trainer;

public:

    std::string player_name;
    
    std::vector<moves> his_moves;

    std::vector<round_result> his_result;

    Player (std::string name, RPSTrainer* trainer) : player_name(name), trainer(trainer) {}

    moves play_a_round() const;

    moves trainer_play_a_round () const;

    void train_iter (moves my_move, moves opp_move) const;

    void update_move (moves move);

    void update_result (round_result result);

    moves get_last_move () const;

    round_result get_last_round_result () const;

    bool is_real_player () const;

    void display_strategy () const;
};

#endif
