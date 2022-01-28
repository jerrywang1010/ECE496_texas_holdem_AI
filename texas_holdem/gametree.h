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
    std::unordered_map<int, std::vector<TreeNode*> > tree_with_depth;
    GameTree() : root(nullptr), m_eval (omp::HandEvaluator()) {}
    void build_tree(Hand deck);
    void recursive_build_tree(TreeNode* parent, Board_state args);
    void print_tree(std::ostream& s = std::cout);
    Node_type get_next_node_type(TreeNode* parent, Action a);
    std::vector<Action> get_legal_actions(TreeNode* parent);
};
