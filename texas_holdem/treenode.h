#pragma once

#include <vector>
#include <string>
#include "utils.h"


typedef struct Infoset
{
    std::vector<Action> action_history;
    Hand private_card;
    Hand community_card;
    std::pair<int> committed;
} Infoset;


class BaseNode
{

public:
    BaseNode* parent = nullptr;
    std::vector<BaseNode*> children;
    int round_idx = 0;
    bool is_terminal = false;
    bool is_chance = false;
};


class TerminalNode : public BaseNode
{
    Infoset infoset;
    std::pair<int> utility;

public:
    TerminalNode(Infoset infoset, auto utility) : infoset(infoset), utility(utility), is_terminal(true) {}
};


class ChanceNode : public BaseNode
{
    float chance_prob;
    Hand remainder_deck;

public:
    ChanceNode() is_chance(true) {}
    ChanceNode(float chance_porb, Hand remainder_deck) : chance_porb(chance_prob), remainder_deck(remainder_deck), is_chance(true) {}
};


class ActionNode : public BaseNode
{
    Infoset infoset;
    int active_player_idx;
    Hand remainder_deck;
    std::vector<Action> action_this_round;

public:
    ActionNode(Infoset infoset, int active_player_idx, Hand remainder_deck, auto action_this_round) : 
                infoset(infoset), active_player_idx(active_player_idx), remainder_deck(remainder_deck), action_this_round(action_this_round) {}
}
