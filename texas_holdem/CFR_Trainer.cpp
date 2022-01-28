#include "CFR_Trainer.h"
#include <numeric>


// bot is player 0 by convention
float CFR_Trainer::cfr_utility_recursive (TreeNode* node, float reach_0, float reach_1)
{
    if (node->is_terminal)
    {
        TerminalNode* terminal_node = dynamic_cast<TerminalNode*>(node);
        return std::get<0>(terminal_node->utility);
    }

    else if (node->is_chance)
    {
        float utility_sum = 0;
        ChanceNode* chance_node = dynamic_cast<ChanceNode*>(node);
        for (auto child : chance_node->children)
        {
            utility_sum += chance_node->chance_prob * cfr_utility_recursive(child, reach_0, reach_1);
        }
        return utility_sum;
    }

    // action node
    float utility = 0;
    ActionNode* action_node = dynamic_cast<ActionNode*>(node);
    std::vector<float> children_state_utility(action_node->children.size());
    // i=0 : BET
    // i=1 : FOLD
    // i=2 : CHECK
    for (int i = 0; i < action_node->children.size(); i ++)
    {
        if (action_node->active_player_idx == 0) reach_0 *= action_node->sigma[i];
        else reach_1 *= action_node->sigma[i];

        float child_state_utility = cfr_utility_recursive(action_node->children[i], reach_0, reach_1);

        children_state_utility[i] = child_state_utility;

        utility += action_node->sigma[i] * child_state_utility;
    }
    float cfr_reach, reach;
    std::tie(cfr_reach, reach) = action_node->active_player_idx == 0 ? std::make_tuple(reach_1, reach_0) : std::make_tuple(reach_0, reach_1);
    
    for (int i = 0; i < action_node->children.size(); i ++)
    {
        float action_cfr_regret = action_node->active_player_idx == 0 ? 
                                  cfr_reach * (children_state_utility[i] - utility): 
                                  -1 * cfr_reach * (children_state_utility[i] - utility);
        action_node->cumulative_cfr_regret[i] += action_cfr_regret;
        action_node->cumulative_sigma[i] += reach * action_node->sigma[i];
        // std::cout << "reach=" << reach << " sigma[" << i << "]=" << action_node->sigma[i] << " cumulative_sigma[" << i << "]=" << action_node->cumulative_sigma[i] << "\n";
    }
    return utility;
}


inline void CFR_Trainer::update_sigma_recursive(TreeNode* node)
{
    if (node->is_terminal) return;
    else if (node->is_chance)
    {
        for (auto child : node->children)
        {
            update_sigma_recursive(child);
        }
    }
    else
    {
        // action node
        update_sigma(dynamic_cast<ActionNode*>(node));
        for (auto child : node->children)
        {
            update_sigma_recursive(child);
        }
    }
}


inline void CFR_Trainer::update_sigma(ActionNode* node)
{
    float regret_sum = std::accumulate(node->cumulative_cfr_regret.begin(), node->cumulative_cfr_regret.end(), 0, 
                        [](int first, int second)
                        {
                            return first + (second > 0 ? second : 0);
                        });
    int num_actions = node->children.size();

    for (int i = 0; i < num_actions; i ++)
    {
        node->sigma[i] = regret_sum > 0 ?
                         std::max(node->cumulative_cfr_regret[i], 1E-10f) / regret_sum :
                         1.0 / num_actions;
        // if (node->sigma[i] <= 0)
        // {
        //     std::cout << "regret_sum=" << regret_sum << ", cumulative_cfr_regret[i]=" << node->cumulative_cfr_regret[i] << "\n";
        //     assert(node->sigma[i] > 0);
        // }
    }
}


void CFR_Trainer::trainer_init_recursive(TreeNode* node)
{
    if (node->is_terminal) return;
    if (node->is_chance)
    {
        for (auto child : node->children)
        {
            trainer_init_recursive(child);
        }
    }
    // action node
    else
    {
        ActionNode* action_node = dynamic_cast<ActionNode*>(node);
        int num_actions = action_node->children.size();

        action_node->sigma.resize(num_actions, 1.0f / num_actions);
        action_node->cumulative_cfr_regret.resize(num_actions, 0);
        action_node->cumulative_sigma.resize(num_actions, 0);

        for (auto child : action_node->children)
        {
            trainer_init_recursive(child);
        }
    }
}


// initialize specific data fileds in action nodes
// including resize vectors, set initial values
void CFR_Trainer::trainer_init()
{   
    assert(this->tree.root != nullptr);
    return trainer_init_recursive(this->tree.root);
}


void CFR_Trainer::train(int iteration)
{
    for (int i = 0; i < iteration; i ++)
    {
        cfr_utility_recursive(this->tree.root, 1, 1);

        update_sigma_recursive(this->tree.root);
    }
}


// put the final nash eq strategy inside sigma, since that's no longer needed
void CFR_Trainer::compute_nash_eq_recursive(TreeNode* node)
{
    if (node->is_terminal) return;
    if (node->is_chance)
    {
        for (auto child : node->children)
        {
            compute_nash_eq_recursive(child);
        }
    }
    // action node
    else
    {
        ActionNode* action_node = dynamic_cast<ActionNode*>(node);
        float sigma_sum = 0;
        std::for_each(action_node->cumulative_sigma.begin(), action_node->cumulative_sigma.end(), [&] (float n) {
            sigma_sum += n;
        });
        for (int i = 0; i < action_node->children.size(); i ++)
        {
            action_node->sigma[i] = action_node->cumulative_sigma[i] / sigma_sum;
        }
        for (auto child : action_node->children)
        {
            compute_nash_eq_recursive(child);
        }
    }
}


void CFR_Trainer::compute_nash_eq()
{
    return compute_nash_eq_recursive(this->tree.root);
}



