#pragma once

#include <vector>
#include <string>
#include <assert.h>
#include <algorithm>
#include "utils.h"
#include <./OMPEval-master/omp/HandEvaluator.h>



typedef struct Infoset
{
    std::vector<Action> action_history;
    Hand private_card;
    Hand community_card;
    std::pair<int, int> committed;

    void add_to_action_history(Action a) {action_history.push_back(a);}
    void add_to_private_card(Hand h) {private_card.insert(private_card.end(), h.begin(), h.end());}
    void add_to_community_card(Hand h) {community_card.insert(community_card.end(), h.begin(), h.end());}
    void commit(int player_idx, int bet) {
        if(player_idx == 0)
        {
            committed.first += bet;
        }
        else
        {
            committed.second += bet;
        }
    }

    void print_infoset()
    {
        std::cout << "+++++++info_set+++++++" << std::endl;
        std::cout << "Action_history: ";
        UTILS::print_action_vec(action_history);

        std::cout << "Private_card: ";
        UTILS::print_hand(private_card);

        std::cout << "Community_card: ";
        UTILS::print_hand(community_card);

        std::cout << "Committed: [" << committed.first << ", " << committed.second << "]" << std::endl;

        std::cout << "+++++++++++++++++++++" << std::endl;
    }

} Infoset;


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
    int round_idx = 0;
    bool is_terminal = false;
    bool is_chance = false;
    virtual void print_node() {};
};


class ChanceNode : public TreeNode
{
private:
    friend class GameTree;

public:
    float chance_prob;
    ChanceNode() {is_chance = true;}
    void build_chance_node(TreeNode* parent, Action a, const Board_state & args);
    void print_node() override;
};


class ActionNode : public TreeNode
{
private:
    std::vector<Action> action_this_round;
    friend class GameTree;

public:
    int active_player_idx;
    ActionNode() {}
    void build_action_node(TreeNode* parent, Action a, const Board_state & args);
    void print_node() override;
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
    void print_node() override;
};
