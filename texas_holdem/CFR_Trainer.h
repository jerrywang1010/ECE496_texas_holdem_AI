#pragma once
#include "gametree.h"
#include <C:\boost\include\boost-1_77\boost\unordered_map.hpp>


typedef boost::unordered_map<Infoset, Infoset_value> InfosetMap;

class CFR_Trainer
{
private:
    GameTree& tree;
    float cfr_utility_recursive(TreeNode* node, float reach_0, float reach_1);
    inline void update_sigma_recursive(TreeNode* node);
    inline void update_sigma(ActionNode* node);
    void trainer_init_recursive(TreeNode* node);
    void trainer_init();
    void compute_nash_eq_recursive(TreeNode* node);

public:
    InfosetMap infoset_map;
    CFR_Trainer(GameTree& tree) : tree(tree) { trainer_init(); }
    void train(int iteration = 1);
    void compute_nash_eq();
    float odds_calculator(const Hand& private_cards, const Hand& community_cards, const Hand& deck, int num_iter);
};
