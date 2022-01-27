#pragma once

#include <vector>
#include <string>
#include <assert.h>
#include <algorithm>
#include "utils.h"
#include <./OMPEval-master/omp/HandEvaluator.h>



typedef struct Infoset
{
    // use 32 bit to represent action history, 2 bits per action, lower bits are more recent
    // default to be all Action::INVALID 
    uint32_t action_history = 0;

    // use 32 bit to represent upto 5 community cards, each card is 6 bits ranging from 0x0 to 0x33
    uint32_t community_card = 0;

    // use 16 bit to represent 2 private cards, each card is 6 bits ranging from 0x0 to 0x33
    uint16_t private_card = 0;

    // committed can not exceed 255
    std::pair<uint8_t, uint8_t> committed = {0, 0};

    // round_idx ranging from 0 to 3
    uint8_t round_idx = 0;

    void add_to_action_history(Action a)
    {
        assert(a != Action::INVALID);
        ADD_TO_ACTIONS(action_history, a);
    }

    void add_to_private_card(Hand h) 
    {
        for (Card c : h) {ADD_TO_HAND(private_card, c);}
    }

    void add_to_community_card(Hand h) 
    {
        for (Card c : h) {ADD_TO_HAND(community_card, c);}
    }

    void commit(int player_idx, int bet) 
    {
        if(player_idx == 0)
        {
            committed.first += bet;
        }
        else
        {
            committed.second += bet;
        }
    }

    void print_infoset(std::ostream& s)
    {
        s << "========INFORMATION SET========" << "\n";
        if (action_history != 0)
        {
            s << "Action_history: ";
            UTILS::print_action_history(action_history, s);
        }

        if (private_card != 0)
        {
            // 0xCF2 is the largest possible private card = {51, 50} in binary
            assert(private_card >= 0 && private_card <= 0xCF2);
            s << "Private_card: ";
            UTILS::print_hand<uint16_t>(private_card, s, 2);
        }
        if (community_card != 0)
        {
            // 0x33CB1C2F is the largest possible community card = {51, 50, 49, 48, 47} in binary
            assert(community_card >= 0 && community_card <= 0x33CB1C2F);
            s << "Community_card: ";
            // flop
            if (round_idx == 1) UTILS::print_hand<uint32_t>(community_card, s, 3);
            // turn
            else if (round_idx == 2) UTILS::print_hand<uint32_t>(community_card, s, 4);
            // river
            else if (round_idx == 3) UTILS::print_hand<uint32_t>(community_card, s, 5);
            else fprintf(stderr, "round_idx=%d in print_infoset\n", round_idx);
        }

        s << "Committed: [" << static_cast<int>(committed.first) << ", " << static_cast<int>(committed.second) << "]" << "\n";

        s << "===============================" << "\n";
    }
} Infoset;


// this is okay to have bad memory performace, because it is used as an intermediate argument to pass around functions,
// it will not stay in the tree permanately like everthing else in this file.
typedef struct Board_state
{
    Hand private_card;
    Hand community_card;
    Hand remaining_deck;
    void add_to_private_card(const Hand & h) {private_card.insert(private_card.end(), h.begin(), h.end());}
    void add_to_community_card(const Hand & h) {community_card.insert(community_card.end(), h.begin(), h.end());}
    void remove_from_deck(const Hand & h) 
    {
        for (unsigned c : h) {
            auto it = std::find(remaining_deck.begin(), remaining_deck.end(), c);
            assert(it != remaining_deck.end());
            remaining_deck.erase(it);
        } 
    }
} Board_state;


class TreeNode
{
public:
    TreeNode* parent = nullptr;
    std::vector<TreeNode*> children;
    Infoset infoset;
    bool is_terminal = false;
    bool is_chance = false;
    virtual void print_node(int depth, std::ostream& s) {};
};


class ChanceNode : public TreeNode
{
private:
    friend class GameTree;

public:
    float chance_prob;
    ChanceNode() {is_chance = true;}
    void build_chance_node(TreeNode* parent, Action a, const Board_state & args);
    void print_node(int depth, std::ostream& s) override;
};


class ActionNode : public TreeNode
{
private:
    // at most 3 actions per round in cases of check, bet, bet
    uint8_t action_this_round = 0;
    friend class GameTree;

public:
    int active_player_idx;
    ActionNode() {}
    void build_action_node(TreeNode* parent, Action a, const Board_state & args);
    void print_node(int depth, std::ostream& s) override;
};


class TerminalNode : public TreeNode
{
private:
    friend class GameTree;

public:
    int active_player_idx;
    // format will be <Up0, Up1>
    std::pair<int,int> utility;

    TerminalNode() {is_terminal = true;}
    void build_terminal_node(TreeNode* parent, bool is_showdown, Action a, const omp::HandEvaluator & m_eval, const Board_state & args);
    void print_node(int depth, std::ostream& s) override;
};
