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
    // the 2 cards that is private to this player
    Hand m_hand;
    // all the cards that this player can use to form a 5 cards hand
    // private hadn + all community card
    Hand m_usable_cards;

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

    Hand get_usable_cards() const;

    int get_balance() const;

    void update_balanace(int money);

    void add_to_hand(Hand h);

    void add_to_usable_cards(Hand h);

    void clear_hand();

    void clear_usable_cards();

    void set_balance(int balanace);

    Action get_last_action_in_round() const;

    void update_his_action();

    void update_round_action(Action a);

    void clear_his_action() {m_his_action.clear();}

    void clear_round_action() {m_round_action.clear();}

    void display_hand();

    void display_usable_cards();
};
