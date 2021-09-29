#include "RPSTrainer.h"

/**
 * Return the strategy (probablity distribution) of next move base on previous regrets
 *
 * @param None
 * @return vector<double> as probablity distribution
 * 
 */
std::vector<double> RPSTrainer::get_strategy ()
{
    if (use_fixed_strategy) return strategy;

    double normalizing_sum = 0;
    for (int a = 0; a < NUM_ACTIONS; a ++)
    {
        strategy[a] = regret_sum[a] > 0 ? regret_sum[a] : 0;
        normalizing_sum += strategy[a];
    }

    for (int a = 0; a < NUM_ACTIONS; a ++)
    {
        if (normalizing_sum > 0)
        {
            strategy[a] /= normalizing_sum;

        }
        else
        {
            strategy[a] = 1.0 / NUM_ACTIONS;
        }
        strategy_sum[a] += strategy[a];
    }
    return strategy;
}


/**
 * Return the next move based on strategy i.e. probability distribution 
 *
 * @param strategy
 * @return moves
 * 
 */
moves RPSTrainer::get_move()
{
    std::vector<double> strategy = get_strategy();

    // get a random float between [0, 1)
    float r = static_cast<float> (rand()) / static_cast<float> (RAND_MAX);

    std::vector<double> cdf (NUM_ACTIONS, 0);
    double sum = 0;
    int i = 0;
    for (; i < NUM_ACTIONS; i ++)
    {
        sum += strategy[i];
        cdf[i] = sum;
        if (cdf[i] >= r) break;
    }

    switch (i)
    {
        case 0 : return moves::ROCK;
        case 1 : return moves::PAPER;
        case 2 : return moves::SCISSOR;
        
        default : return moves::INVALID;
    }
}


/**
 * Compute utility and regrets based on the last round results, also updates regret_sum
 *
 * @param my_move
 * @param opponent_move
 * @return void
 * 
 */
void RPSTrainer::update_regret (moves my_move, moves opp_move)
{
    int action_utility = utility_table[static_cast<int16_t> (opp_move)][static_cast<int16_t> (my_move)];
    // expected_utility is the utility I would get if I played i instead of self_move
    std::vector<int> expected_utility = utility_table[static_cast<int16_t> (opp_move)];
    for (int a = 0; a < NUM_ACTIONS; a ++)
    {   
        // regret is computed by what utility I would have got if I played a - what utility I actually get
        // regret_sum is a continuous sum of all regret for action a through each iteration
        regret_sum[a] += expected_utility[a] - action_utility;
    }
}


/**
 * Return the final strategy after the game is done, taking an avg of all previous strategies
 *
 * @param None
 * @return vector<double> as probablity distribution of next move
 * 
 */
std::vector<double> RPSTrainer::get_avg_strategy () const
{   
    if (this->use_fixed_strategy) return this->strategy;
    
    std::vector<double> avg_strategy (NUM_ACTIONS, 0);
    double normalizing_sum = 0;
    for (int a = 0; a < NUM_ACTIONS; a ++)
    {
        normalizing_sum += strategy_sum[a];
    }
    for (int a = 0; a < NUM_ACTIONS; a ++)
    {
        if (normalizing_sum > 0)
        {
            avg_strategy[a] = strategy_sum[a] / normalizing_sum;
        }
        else
        {
            avg_strategy[a] = 1.0 / NUM_ACTIONS;
        }
    }
    return avg_strategy;
}


/**
 * Train for one iteration, update regret
 *
 * @param my_move
 * @param opponents_move
 * @return void
 * 
 */
void RPSTrainer::train_iter (moves my_move, moves opp_move)
{   
    if (!use_fixed_strategy)
    {
        update_regret(my_move, opp_move);
    }
}


/**
 * Train for a number of iteration and return the final avg strategy
 *
 * @param num_iteration
 * @return vector<double> as probablity distribution of the final optimal strategy
 * 
 */
// std::vector<double> RPSTrainer::train (int iteration)
// {
//     for (int i = 0; i < iteration; i ++)
//     {
//         std::vector<double> my_strategy = get_strategy();
//         moves my_move = get_move();
        
//         // calculate utility and regret also update regret_sum
//         update_regret(my_move, opp_move);
//     }
//     return get_avg_strategy();
// }
