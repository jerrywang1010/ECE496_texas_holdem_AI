#pragma once
#include "game_state.h"
#include "player.h"
#include <./OMPEval-master/omp/HandEvaluator.h>

// // Forward declaration to avoid circular dependency
class Game_state;

class Game
{
public:

    std::vector<Player> m_players;

    std::vector<int> winning_records {0, 0};

    Game ();

    inline Game_state* get_current_state() const {return m_state;}

    void process_state();

    void transition(Game_state& new_state);

    void run(unsigned iteration);

    bool round_finished() const;

    void add_to_action_this_round(Action a);

    void clear_action_this_round();

    void clear_his_action();

    void update_his_action();

    void clear_player_cards();

    void display_community_cards();

    void add_to_community_cards(Hand h);

    // void print_round_action() const;

    uint16_t get_hand_ranking(Hand hand) const;

    void shuffle_and_deal();

    void display_round_result() const;

private:
    Game_state* m_state;

    float m_pot;

    // because we only have 2 players, the index of the lossing player will be either 0 or 1
    int m_loss_player_idx;

    size_t m_num_player;

    unsigned m_round_count;

    unsigned m_iteration;

    omp::HandEvaluator m_eval;

    Hand m_cards;

    Hand m_community_cards;

    friend class Pre_flop;
    friend class Flop;
    friend class Turn;
    friend class River;
    friend class Showdown;
    friend class Terminal;
};
