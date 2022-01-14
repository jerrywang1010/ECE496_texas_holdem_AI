#pragma once
#include "treenode.h"

class GameTree
{
private:
    TreeNode* root;
    omp::HandEvaluator m_eval;

public:
    GameTree() : root(nullptr), m_eval (omp::HandEvaluator()) {}
    void build_tree(Hand deck);
    void recursive_build_tree(TreeNode* parent, Board_state args);
    void print_tree();
    Node_type get_next_node_type(TreeNode* parent, Action a);
    std::vector<Action> get_legal_actions(TreeNode* parent);
};
