#pragma once
#include "utils.h"

/*
    CARDS hand
    History

    ACTION get_action()

*/

class Player
{

private:
    Hand m_hand;
    int m_balance;

    // history action in a game
    std::vector<Action> m_his_action;

    // history action in a round
    std::vector<Action> m_round_action;

public:
    std::string player_name;

    Player (std::string player_name) : player_name(player_name) {}

    Player (std::string n, int b) : player_name(n), m_balance(b) {}

    Action get_action(bool allow_check=true) const;

    Hand get_hand() const;

    int get_balance() const;

    void update_balanace(int money);

    void add_to_hand(Hand h);

    void clear_hand();

    void set_balance(int balanace);

    Action get_last_action_in_round() const;

    void update_his_action();

    void update_round_action(Action a);

    void clear_his_action() {m_his_action.clear();}

    void clear_round_action() {m_round_action.clear();}

    void display_hand();
};