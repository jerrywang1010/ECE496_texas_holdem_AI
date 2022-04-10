#pragma once

#include <vector>
#include <string>
#include <assert.h>
#include <algorithm>
#include "utils.h"
#include "infoset.h"
#include <./OMPEval-master/omp/HandEvaluator.h>


// this is okay to have bad memory performace, because it is used as an intermediate argument to pass around functions,
// it will not stay in the tree permanately like everthing else in this file.
typedef struct Build_node_args
{
    Hand private_card;
    Hand community_card;
    Hand remaining_deck;
    int round_idx=0, active_player_idx=0;
    std::vector<Action> action_history;
    std::vector<Action> action_this_round;
    std::pair<uint8_t, uint8_t> committed = {0, 0};
    void add_to_private_card(const Hand & h) {private_card.insert(private_card.end(), h.begin(), h.end());}
    void add_to_community_card(const Hand & h) {community_card.insert(community_card.end(), h.begin(), h.end());}
    void add_to_action_history(Action a) {action_history.push_back(a);}
    void add_to_action_this_round(Action a) {action_this_round.push_back(a);}
    void remove_from_deck(const Hand & h) 
    {
        for (unsigned c : h) {
            auto it = std::find(remaining_deck.begin(), remaining_deck.end(), c);
            assert(it != remaining_deck.end());
            remaining_deck.erase(it);
        } 
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
} Build_node_args;


class TreeNode
{
public:
    std::vector<TreeNode*> children;
    bool is_terminal = false;
    bool is_chance = false;
    virtual void print_node(int, std::ostream&) {};
    virtual ~TreeNode() {};
};


class ChanceNode : public TreeNode
{
    friend class GameTree;
    friend class CFR_Trainer;
private:

public:
    float chance_prob;
    ChanceNode() {is_chance = true;}
    void build_chance_node(TreeNode* parent, const Build_node_args & args);
    void print_node(int depth, std::ostream& s) override;
    virtual ~ChanceNode() {}; 
};


class ActionNode : public TreeNode
{
    friend class GameTree;
    friend class CFR_Trainer;
private:
    friend class GameTree;

public:
    // private_card; community_card; action_history
    Infoset infoset;

    ActionNode() {}
    void build_action_node(TreeNode* parent, const Build_node_args & args);
    void print_node(int depth, std::ostream& s) override;
    virtual ~ActionNode() {};
};


class TerminalNode : public TreeNode
{
    friend class GameTree;
    friend class CFR_Trainer;
private:

public:
    // format will be <Up0, Up1>
    std::pair<float,float> utility;

    TerminalNode() {is_terminal = true;}
    void build_terminal_node(TreeNode* parent, bool is_showdown, const omp::HandEvaluator & m_eval, const Build_node_args & args);
    void print_node(int depth, std::ostream& s) override;
    virtual ~TerminalNode() {};
};
