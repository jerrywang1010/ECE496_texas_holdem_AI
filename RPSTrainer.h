#ifndef RPSTRAINER_GUARD
#define RPSTRAINER_GUARD

#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <iterator>
#include "utils.h"

const static int NUM_ACTIONS = 3;

class RPSTrainer
{

private:
    std::vector<int> regret_sum;
    std::vector<double> strategy;
    std::vector<double> strategy_sum;
    bool use_fixed_strategy;

public:
    /**
     * Constructor for RPSTrainer
     *
     * @param None
     * @return None
     * 
     */
    RPSTrainer() 
        : regret_sum(NUM_ACTIONS, 0), 
          strategy(NUM_ACTIONS, 0), 
          strategy_sum(NUM_ACTIONS, 0),
		  use_fixed_strategy(false)
    {
        srand (static_cast <unsigned> (time(0)));
    }

    /**
     * Constructor for RPSTrainer
     *
     * @param use_fixed_strategy
     * @return None
     * 
     */
    RPSTrainer(std::vector<double> strategy, bool use_fixed_strategy=true)
        : regret_sum(NUM_ACTIONS, 0), 
          strategy(strategy), 
          strategy_sum(NUM_ACTIONS, 0),
          use_fixed_strategy(use_fixed_strategy)
    {
        srand (static_cast <unsigned> (time(0)));
    }

    std::vector<double> get_strategy ();

    moves get_move ();

    void update_regret (moves my_move, moves opp_move);

    std::vector<double> get_avg_strategy () const;

    void train_iter (moves my_move, moves opp_move);

    /**
     * Display the final strategy
     *
     * @param final_strategy to be printed
     * @return void
     * 
     */
    template <typename T>
    void display_strategy (const std::vector<T> & final_strategy) const
    {
        std::cout << "\nprobability of playing [ROCK, PAPER, SCISSOR] = [";
        std::copy(final_strategy.begin(), --final_strategy.end(), std::ostream_iterator<T>(std::cout, ", "));
        std::cout << final_strategy.back() << "]\n" << "\n";
    }
};

#endif