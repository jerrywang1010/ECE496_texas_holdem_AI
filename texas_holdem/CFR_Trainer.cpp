#include "CFR_Trainer.h"
#include <numeric>
#include <chrono>
#include <random>

void CFR_Trainer::odds_calculator(const omp::HandEvaluator &m_eval, const Hand &private_cards, const Hand &community_cards, const Hand &deck, int num_iter, bool allow_check, float &b, float &c, float &f)
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
    int cnt = 0;
    int bet = 0;
    int fold = 0;
    int check = 0;
    int win = 0;
    for (int i = 0; i < num_iter; i ++)
    {
        srand (time(NULL));
        float random_num = (rand() % 100 + 1) /(float)100;
        if (random_num <= 1/3)
        {
            bet ++;
            cnt ++;
            continue;
        }
        
        //Hand hands;
        //hands.push_back(private_cards[0]);
        //hands.push_back(private_cards[1]);
        Hand myhands = {private_cards[0], private_cards[1]};
        Hand ophands;
        for (size_t j = 0; j < community_cards.size(); j ++)
        {
            //hands.push_back(community_cards[j]);
            myhands.push_back(community_cards[j]);
            ophands.push_back(community_cards[j]);
        }
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::shuffle(pools.begin(), pools.end(), std::default_random_engine(seed));
        // win/lose for current status
        int cur_situation = 0; // -1 lose, 0 draw, 1 win
        int final_situation = 0;
        if (community_cards.size() && community_cards.size() < 5)
        {
            ophands.push_back(pools[0]);//op private1
            ophands.push_back(pools[1]);//op private2
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
                cur_situation = 1;
            }
            else if (myscore < opscore)
            {
                cur_situation = -1;
            }
        }
        else
        {
            ophands.push_back(pools[0]);//op private1
            ophands.push_back(pools[1]);//op private2
        }
        int hands_size = myhands.size();
        // get the rest of community cards
        for (int k = 0; k < 7 - hands_size; k ++)
        {
            //if (myhands.size() < 7)
            //{
            myhands.push_back(pools[k + 2]);
            //}
            ophands.push_back(pools[k + 2]);
        }
        
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
            final_situation = 1;
        }
        if (myscore < opscore)
        {
            final_situation = -1;
        }
        if (cur_situation == 1)
        {
            if (final_situation == 1)
            {
                
                bet ++;
            }
            else
            {
                check ++;
            }
        }
        else if (cur_situation == -1)
        {
            if (final_situation == 1)
            {
                
                check ++;
            }
            else
            {
                fold ++;
            }
        }
        else if (community_cards.size() < 5)
        {
            if (final_situation == 1)
            {
                
                check ++;
            }
            else
            {
                fold ++;
            }
        }
        else if (community_cards.size() == 5)
        {
             if (final_situation == 1)
            {
                
                bet ++;
            }
            else if (final_situation == 0)
            {
                check ++;
            }
            else
            {
                fold ++;
            }
        }
        cnt ++;
    }
    debug_print("Odds_calculator sampling results: bet=%d, check=%d, fold=%d, total_sampling_count=%d\n", bet, check, fold, cnt);
    debug_print("win : %f %d %d %d\n", win/(float)cnt, bet, check, fold);
    b = bet / (float)cnt;
    f = fold / (float)cnt;
    c  = check / (float)cnt;
    if (allow_check == false)
    {
        b = (bet + check) / (float)cnt;
    }

}

// bot is player 0 by convention
float CFR_Trainer::cfr_utility_recursive(TreeNode *node, float reach_0, float reach_1)
{
    if (node->is_terminal)
    {
        TerminalNode *terminal_node = dynamic_cast<TerminalNode *>(node);
        return std::get<0>(terminal_node->utility);
    }

    else if (node->is_chance)
    {
        float utility_sum = 0;
        ChanceNode *chance_node = dynamic_cast<ChanceNode *>(node);
        for (auto child : chance_node->children)
        {
            utility_sum += chance_node->chance_prob * cfr_utility_recursive(child, reach_0, reach_1);
        }
        return utility_sum;
    }

    // action node
    float utility = 0;
    ActionNode *action_node = dynamic_cast<ActionNode *>(node);
    std::vector<float> children_state_utility(action_node->children.size());
    // i=0 : BET
    // i=1 : FOLD
    // i=2 : CHECK

    assert(this->infoset_map.find(action_node->infoset) != this->infoset_map.end());
    Infoset_value &v = this->infoset_map.at(action_node->infoset);
    for (size_t i = 0; i < action_node->children.size(); i++)
    {
        double child_reach_0 = 0, child_reach_1 = 0;
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

    for (size_t i = 0; i < action_node->children.size(); i++)
    {
        float action_cfr_regret = action_node->infoset.active_player_idx == 0 ? cfr_reach * (children_state_utility[i] - utility) : -1 * cfr_reach * (children_state_utility[i] - utility);

        v.cumulative_cfr_regret[i] += action_cfr_regret;
        v.cumulative_sigma[i] += reach * v.sigma[i];
    }
    return utility;
}

inline void CFR_Trainer::update_sigma_recursive(TreeNode *node)
{
    if (node->is_terminal)
        return;
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
        update_sigma(dynamic_cast<ActionNode *>(node));
        for (auto child : node->children)
        {
            update_sigma_recursive(child);
        }
    }
}

inline void CFR_Trainer::update_sigma(ActionNode *node)
{
    Infoset_value &v = infoset_map.at(node->infoset);
    float regret_sum = std::accumulate(v.cumulative_cfr_regret.begin(), v.cumulative_cfr_regret.end(), 0.0f,
                                       [](float first, float second)
                                       {
                                           return first + (second > 0 ? second : 0);
                                       });
    int num_actions = node->children.size();

    for (int i = 0; i < num_actions; i++)
    {
        v.sigma[i] = regret_sum > 0 ? std::max(v.cumulative_cfr_regret[i], 0.0f) / regret_sum : 1.0 / num_actions;

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

void CFR_Trainer::trainer_init_recursive(TreeNode *node)
{
    if (node->is_terminal)
        return;
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
        ActionNode *action_node = dynamic_cast<ActionNode *>(node);
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
    assert(this->tree->root != nullptr);
    return trainer_init_recursive(this->tree->root);
}

void CFR_Trainer::train(int iteration)
{
    int interval = iteration >= 10 ? iteration / 10 : 1;
    for (int i = 0; i < iteration; i++)
    {
        if (i % interval == 0)
        {
            std::cout << "iteration: " << i << std::endl;
            float progress = (float)i / iteration;
            int barWidth = 70;

            std::cout << "[";
            int pos = barWidth * progress;
            for (int i = 0; i < barWidth; ++i)
            {
                if (i < pos)
                    std::cout << "=";
                else if (i == pos)
                    std::cout << ">";
                else
                    std::cout << " ";
            }
            std::cout << "] " << int(progress * 100.0) << " %\r";
            std::cout.flush();
            std::cout << std::endl;
        }
        cfr_utility_recursive(this->tree->root, 1, 1);
        update_sigma_recursive(this->tree->root);
    }
}

// put the final nash eq strategy inside sigma, since that's no longer needed
void CFR_Trainer::compute_nash_eq_recursive(TreeNode *node)
{
    if (node->is_terminal)
        return;
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
        ActionNode *action_node = dynamic_cast<ActionNode *>(node);
        Infoset_value &v = infoset_map.at(action_node->infoset);
        float sigma_sum = 0;
        std::for_each(v.cumulative_sigma.begin(), v.cumulative_sigma.end(), [&](float n)
                      { sigma_sum += n; });
        for (size_t i = 0; i < action_node->children.size(); i++)
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
    return compute_nash_eq_recursive(this->tree->root);
}
