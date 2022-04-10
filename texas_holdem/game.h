#pragma once
#include "game_state.h"
#include "player.h"
#include <./OMPEval-master/omp/HandEvaluator.h>

// // Forward declaration to avoid circular dependency
class Game_state;

const static std::vector<std::vector<std::string> > all_possible_action_history = 
{
    // Preflop
    {"", "B"},
    // Flop
    {"BB","BBC","BBB","BBCB"},
    // Turn
    {"BBCC","BBCCC","BBCCB","BBCCCB","BBBB","BBBBC","BBBBB","BBBBCB","BBCBB","BBCBBC","BBCBBB","BBCBBCB"},
    // River
    {"BBCCCC","BBCCCCC","BBCCCCB","BBCCCCCB","BBCCBB","BBCCBBC","BBCCBBB","BBCCBBCB","BBCCCBB","BBCCCBBC","BBCCCBBB","BBCCCBBCB","BBBBCC","BBBBCCC","BBBBCCB","BBBBCCCB","BBBBBB","BBBBBBC","BBBBBBB","BBBBBBCB","BBBBCBB","BBBBCBBC","BBBBCBBB","BBBBCBBCB","BBCBBCC","BBCBBCCC","BBCBBCCB","BBCBBCCCB","BBCBBBB","BBCBBBBC","BBCBBBBB","BBCBBBBCB","BBCBBCBB","BBCBBCBBC","BBCBBCBBB","BBCBBCBBCB"}
};

class Game
{
public:
    std::vector<Player> m_players;

    std::vector<int> winning_records {0, 0};

    Game (Hand deck);

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

    void output_strategy_as_json(int num_games, std::string file_name="strategy.json");

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

    Hand m_playing_deck;

    Hand m_community_cards;

    friend class Pre_flop;
    friend class Flop;
    friend class Turn;
    friend class River;
    friend class Showdown;
    friend class Terminal;
};
