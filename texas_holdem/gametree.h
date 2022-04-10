#pragma once
#include <unordered_map>
#include "treenode.h"

class GameTree
{
friend class CFR_Trainer;

private:
    TreeNode* root;
    omp::HandEvaluator m_eval;

public:
    std::vector<Hand> all_combo_first_round;
    std::unordered_map<int, std::vector<TreeNode*> > tree_with_depth;
    GameTree() : root(nullptr), m_eval (omp::HandEvaluator()) {}
    void init_all_combo_first_round(Hand deck, bool early_pruning);
    void build_tree(Hand deck, size_t first_round_combination_start_idx, size_t interval);
    void recursive_build_tree(TreeNode* parent, Build_node_args args, size_t first_round_combination_start_idx, size_t interval);
    void print_tree(std::ostream& s = std::cout);
    inline Node_type get_next_node_type(TreeNode* parent, Action a, int round_idx, const std::vector<Action>& action_this_round);
    inline std::vector<Action> get_legal_actions(TreeNode* parent, int round_idx, const std::vector<Action> & action_this_round);
    void free_tree_nodes();
};
