#include "CFR_Trainer.h"
#include <numeric>
#include <chrono>
#include <random>

float CFR_Trainer::odds_calculator(const Hand& private_cards, const Hand& community_cards, const Hand& deck, int num_iter)
{
    // input: private cards, community cards, legal actions, deck size, num_iter
    Hand pools;
    for (size_t i = 0; i < deck.size(); i ++)
    {
        if (std::find(private_cards.begin(), private_cards.end(), deck[i]) != private_cards.end()
        || std::find(community_cards.begin(), community_cards.end(), deck[i]) != community_cards.end())
        {
            continue;
        }
        pools.push_back(deck[i]);
    }


    int win = 0;
    int cnt = 1;
    int tie = 0;
    omp::HandEvaluator m_eval = omp::HandEvaluator();
    for (int i = 0; i < num_iter; i ++)
    {
        Hand hands;
        hands.push_back(private_cards[0]);
        hands.push_back(private_cards[1]);
        for (size_t j = 0; j < community_cards.size(); j ++)
        {
            hands.push_back(community_cards[j]);
        }
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::shuffle(pools.begin(), pools.end(), std::default_random_engine(seed));
        int hands_size = hands.size();
        for (int k = 0; k < 9 - hands_size; k ++)
        {
            hands.push_back(pools[k]);
        }
        Hand myhands = {hands[0], hands[1], hands[2], hands[3], hands[4], hands[5], hands[6]};
        Hand ophands = {hands[7], hands[8], hands[2], hands[3], hands[4], hands[5], hands[6]};
        //std::cout << hands[0] << " " << hands[1] << " " << hands[2] << " " << hands[3] << " " << hands[4] << " " << hands[5] << " " << hands[6] << std::endl;
        //std::cout << hands[7] << " " << hands[8] << " " << hands[2] << " " << hands[3] << " " << hands[4] << " " << hands[5] << " " << hands[6] << std::endl;
        omp::Hand h = omp::Hand::empty();
        for (Card card: myhands)
        {
            h += omp::Hand(card);
        }
        uint16_t myscore = m_eval.evaluate(h);
        omp::Hand oph = omp::Hand::empty();
        for (Card card: ophands)
        {
            oph += omp::Hand(card);
        }
        uint16_t opscore = m_eval.evaluate(oph);
        if (myscore > opscore)
        {
            win ++;
        }
        if (myscore == opscore)
        {
            tie ++;
        }
        cnt ++;
    }
    debug_print("Odds_calculator sampling results: win=%d, tie=%d, loss=%d, total_sampling_count=%d\n", win, tie, cnt-win-tie, cnt);
    float win_rate = win / (float)cnt;
    debug_print("winning rate=%f\n", win_rate);
    return win_rate;
}



// bot is player 0 by convention
float CFR_Trainer::cfr_utility_recursive(TreeNode* node, float reach_0, float reach_1)
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

    assert(this->infoset_map.find(action_node->infoset) != this->infoset_map.end());
    Infoset_value& v = this->infoset_map.at(action_node->infoset);
    for (size_t i = 0; i < action_node->children.size(); i ++)
    {
        double child_reach_0=0, child_reach_1=0;
        if (action_node->infoset.active_player_idx == 0) 
        {
            
            child_reach_0 = reach_0 * v.sigma[i];
            child_reach_1 = reach_1;
        }
        else 
        {
            child_reach_1 = reach_1 * v.sigma[i];
            child_reach_0 = reach_0;
        }
        float child_state_utility = cfr_utility_recursive(action_node->children[i], child_reach_0, child_reach_1);

        children_state_utility[i] = child_state_utility;

        utility += v.sigma[i] * child_state_utility;
    }
    float cfr_reach, reach;
    std::tie(cfr_reach, reach) = action_node->infoset.active_player_idx == 0 ? std::make_tuple(reach_1, reach_0) : std::make_tuple(reach_0, reach_1);
    

    for (size_t i = 0; i < action_node->children.size(); i ++)
    {
        float action_cfr_regret = action_node->infoset.active_player_idx == 0 ? 
                                  cfr_reach * (children_state_utility[i] - utility): 
                                  -1 * cfr_reach * (children_state_utility[i] - utility);

        v.cumulative_cfr_regret[i] += action_cfr_regret;
        v.cumulative_sigma[i] += reach * v.sigma[i];
        
        // if (action_node->active_player_idx == 1 && action_node->infoset.action_history == 1023 && node->infoset.round_idx == 2 )
        // {
        //     std::cout << "node @ depth=8, " << "reach=" << reach << " sigma[" << i << "]=" << action_node->sigma[i] 
        //                 << " cumulative_sigma[" << i << "]=" << action_node->cumulative_sigma[i]
        //                 << " cumulative_cfr_regret[" << i << "]=" << action_node->cumulative_cfr_regret[i]
        //                 << " children_state_utility[" << i << "]=" << children_state_utility[i] << " utility=" << utility << "\n";
        // }
        // if (action_node->active_player_idx == 0 && action_node->infoset.action_history == 4095 && action_node->infoset.round_idx == 3)
        // {
        //     std::cout << "node @ depth=10, " << "reach=" << reach << " sigma[" << i << "]=" << action_node->sigma[i] 
        //                 << " cumulative_sigma[" << i << "]=" << action_node->cumulative_sigma[i]
        //                 << " cumulative_cfr_regret[" << i << "]=" << action_node->cumulative_cfr_regret[i]
        //                 << " children_state_utility[" << i << "]=" << children_state_utility[i] << " utility=" << utility << "\n";
        // }
        // if (action_node->active_player_idx == 0 && action_node->infoset.action_history == 0 && action_node->infoset.round_idx == 0)
        // {
        //     std::cout << "node @ depth=1, " << "reach=" << reach << " sigma[" << i << "]=" << action_node->sigma[i] 
        //                 << " cumulative_sigma[" << i << "]=" << action_node->cumulative_sigma[i]
        //                 << " cumulative_cfr_regret[" << i << "]=" << action_node->cumulative_cfr_regret[i] << "\n";
        // }
        // if (action_node->active_player_idx == 1 && action_node->infoset.action_history == 0x3 && action_node->infoset.round_idx == 0)
        // {
        //     std::cout << "node @ depth=2, " << "reach=" << reach << " sigma[" << i << "]=" << action_node->sigma[i] 
        //                 << " cumulative_sigma[" << i << "]=" << action_node->cumulative_sigma[i] 
        //                 << " cumulative_cfr_regret[" << i << "]=" << action_node->cumulative_cfr_regret[i] << "\n";
        // }
        // if (action_node->active_player_idx == 0 && action_node->infoset.action_history == 15 && node->infoset.community_card == 66906)
        // {
        //     std::cout << "node @ depth=4, " << "reach=" << reach << " sigma[" << i << "]=" << action_node->sigma[i] 
        //                 << " cumulative_sigma[" << i << "]=" << action_node->cumulative_sigma[i]
        //                 << " cumulative_cfr_regret[" << i << "]=" << action_node->cumulative_cfr_regret[i] << "\n";
        // }
        // if (action_node->active_player_idx == 1 && action_node->infoset.action_history == 63 && node->infoset.community_card == 66906)
        // {
        //     std::cout << "node @ depth=5, " << "reach=" << reach << " sigma[" << i << "]=" << action_node->sigma[i] 
        //                 << " cumulative_sigma[" << i << "]=" << action_node->cumulative_sigma[i]
        //                 << " cumulative_cfr_regret[" << i << "]=" << action_node->cumulative_cfr_regret[i]
        //                 << " children_state_utility[" << i << "]=" << children_state_utility[i] << " utility=" << utility << "\n";
        // }
        // if (action_node->active_player_idx == 1 && action_node->infoset.action_history == 1023 && node->infoset.round_idx == 2 )
        // {
        //     std::cout << "node @ depth=8, " << "reach=" << reach << " sigma[" << i << "]=" << action_node->sigma[i] 
        //                 << " cumulative_sigma[" << i << "]=" << action_node->cumulative_sigma[i]
        //                 << " cumulative_cfr_regret[" << i << "]=" << action_node->cumulative_cfr_regret[i]
        //                 << " children_state_utility[" << i << "]=" << children_state_utility[i] << " utility=" << utility << "\n";
        // }
        // if (action_node->active_player_idx == 1 && action_node->infoset.action_history == 16383 && node->infoset.round_idx == 3)
        // {
        //     std::cout << "node @ depth=11, " << "reach=" << reach << " cfr_reach=" << cfr_reach << " sigma[" << i << "]=" << action_node->sigma[i] 
        //                 << " cumulative_sigma[" << i << "]=" << action_node->cumulative_sigma[i]
        //                 << " cumulative_cfr_regret[" << i << "]=" << action_node->cumulative_cfr_regret[i]
        //                 << " children_state_utility[" << i << "]=" << children_state_utility[i] << " utility=" << utility << "\n";
        // }
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
    Infoset_value& v = infoset_map.at(node->infoset);
    float regret_sum = std::accumulate(v.cumulative_cfr_regret.begin(), v.cumulative_cfr_regret.end(), 0.0f, 
                        [](float first, float second)
                        {
                            return first + (second > 0 ? second : 0);
                        });
    int num_actions = node->children.size();

    for (int i = 0; i < num_actions; i ++)
    {
        v.sigma[i] = regret_sum > 0 ?
                        std::max(v.cumulative_cfr_regret[i], 0.0f) / regret_sum :
                        1.0 / num_actions;

        /*
        tentative              
        if (node->sigma[i] == 0) node->sigma[i] = 0.01f;
        */
        // if (v.sigma[i] == 0) v.sigma[i] = 0.01f;
        // if (node->sigma[i] < 0 || node->sigma[i] > 1)
        // {
        //     UTILS::print_vec<float>(node->cumulative_cfr_regret, std::cout);
        //     std::cout << "sigma=" << node->sigma[i] << " regret_sum=" << regret_sum << " cumulative_cfr_regret[i]=" << node->cumulative_cfr_regret[i] << "\n";
        // }
        assert(v.sigma[i] >= 0 && v.sigma[i] <= 1);
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

        Infoset i = action_node->infoset;
        if (this->infoset_map.find(i) == this->infoset_map.end())
        {
            Infoset_value v;
            v.sigma.resize(num_actions, 1.0f / num_actions);
            v.cumulative_cfr_regret.resize(num_actions, 0);
            v.cumulative_sigma.resize(num_actions, 0);
            this->infoset_map.insert({i, v});
        }
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
    int interval = iteration >= 10 ? iteration / 10 : 1;
    for (int i = 0; i < iteration; i ++)
    {
        if (i % interval == 0)
        {
            std::cout << "iteration: " << i << std::endl;
            float progress = (float) i / iteration;
            int barWidth = 70;

            std::cout << "[";
            int pos = barWidth * progress;
            for (int i = 0; i < barWidth; ++i) {
                if (i < pos) std::cout << "=";
                else if (i == pos) std::cout << ">";
                else std::cout << " ";
            }
            std::cout << "] " << int(progress * 100.0) << " %\r";
            std::cout.flush();
            std::cout << std::endl;
        }
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
        Infoset_value& v = infoset_map.at(action_node->infoset);
        float sigma_sum = 0;
        std::for_each(v.cumulative_sigma.begin(), v.cumulative_sigma.end(), [&] (float n) {
            sigma_sum += n;
        });
        for (size_t i = 0; i < action_node->children.size(); i ++)
        {
            v.sigma[i] = v.cumulative_sigma[i] / sigma_sum;
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


