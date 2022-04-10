#pragma once
#include "utils.h"
#include "CFR_Trainer.h"

/*
    CARDS hand
    History

    ACTION get_action()

*/

typedef std::pair<std::vector<float>, std::vector<float> > fixed_strat; 
class Player
{

private:
    bool is_cfr_bot=false, is_fixed_strat_bot=false;

    CFR_Trainer* trainer;

    // fixed_strat<0> = strat[bet, fold] (when check is not allowed)
    // fixed_strat<1> = strat[bet, fold, check] (when check is allowed)
    fixed_strat fs;

public:
    Hand private_cards;

    Hand community_cards;

    Hand deck;

    float m_balance;

    // history action in a game
    std::vector<Action> action_history;

    // history action in a round
    std::vector<Action> action_this_round;

    std::string player_name;
    
    omp::HandEvaluator m_eval = omp::HandEvaluator();

    Player (CFR_Trainer* trainer, Hand deck, std::string n, float b) : is_cfr_bot(true), trainer(std::move(trainer)), deck(std::move(deck)), m_balance(b), player_name(n) {}

    Player (fixed_strat strat, std::string n, float b) : is_fixed_strat_bot(true), fs(std::move(strat)), m_balance(b), player_name(n) {}

    Player (std::string player_name) : player_name(player_name) {}

    Player (std::string n, float b) : m_balance(b), player_name(n) {}

    Action get_action(bool allow_check=true) const;

    void display_private_cards();

    std::vector<float> get_strategy(const Hand& private_hand, const Hand& community_cards, const std::string& action_history, bool allow_check);

    /*
    Hand get_private_cards() const;

    Hand get_community_cards() const;

    void add_to_private_cards(Hand h);

    void add_to_community_cards(Hand h);

    void clear_community_cards();

    void clear_private_cards();

    void update_balanace(int money);

    int get_balance() const;

    void set_balance(int balanace);

    Action get_last_action_in_round() const;

    void update_action_history();

    void update_action_this_round(Action a);

    void clear_action_history() {m_his_action.clear();}

    void clear_action_this_round() {m_round_action.clear();}

    void display_usable_cards();
    */
};
