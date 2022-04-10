#pragma once
#include "gametree.h"
#include <C:\boost\include\boost-1_77\boost\unordered_map.hpp>


typedef boost::unordered_map<Infoset, Infoset_value> InfosetMap;

class CFR_Trainer
{
private:
    float cfr_utility_recursive(TreeNode* node, float reach_0, float reach_1);
    inline void update_sigma_recursive(TreeNode* node);
    inline void update_sigma(ActionNode* node);
    void trainer_init_recursive(TreeNode* node);
    void compute_nash_eq_recursive(TreeNode* node);

public:
    GameTree* tree;
    InfosetMap infoset_map;
    CFR_Trainer() : tree(nullptr) {};
    void trainer_init();
    void train(int iteration = 1);
    void compute_nash_eq();
    void odds_calculator(const omp::HandEvaluator& m_eval, const Hand& private_cards, const Hand& community_cards, const Hand& deck, int num_iter, bool allow_check, float& b, float& c, float& f);
};
