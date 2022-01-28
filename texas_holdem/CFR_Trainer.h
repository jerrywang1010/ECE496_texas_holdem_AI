#pragma once
#include "gametree.h"

class CFR_Trainer
{
private:
    GameTree& tree;
    float cfr_utility_recursive(TreeNode* node, float reach_0, float reach_1);
    inline void update_sigma_recursive(TreeNode* node);
    inline void update_sigma(ActionNode* node);
    void trainer_init_recursive(TreeNode* node);
    void trainer_init();

public:
    CFR_Trainer(GameTree& tree) : tree(tree) { trainer_init(); }
    void train(int iteration = 1);
};
