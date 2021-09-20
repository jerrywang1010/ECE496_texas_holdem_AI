#ifndef RPSTRAINER_GUARD
#define RPSTRAINER_GUARD

#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <iterator>
#include "game.h"
#include "utils.h"


const static std::vector<double> opp_strategy {0.4, 0.3, 0.3};
const static int NUM_ACTIONS = 3;
// row is other action, colomn is my action
const static std::vector<std::vector<int> > utility_table = { {0, 1, -1},
                                                              {-1, 0, 1},
                                                              {1, -1, 0} }; 

class RPSTrainer
{

private:
    std::vector<int> regret_sum;
    std::vector<double> strategy;
    std::vector<double> strategy_sum;

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
          strategy_sum(NUM_ACTIONS, 0)
    {
        srand (static_cast <unsigned> (time(0)));
    }

    std::vector<double> get_strategy ();

    moves get_move(const std::vector<double> & strategy) const;

    void update_regret (moves my_move, moves opp_move);

    std::vector<double> get_avg_strategy ();

    std::vector<double> train (int iteration);

    // /**
    //  * Display the final strategy
    //  *
    //  * @param final_strategy to be printed
    //  * @return void
    //  * 
    //  */
    template <typename T>
    void display_strategy (const std::vector<T> & final_strategy) const
    {
        std::cout << "\nprobability of playing [ROCK, PAPER, SCISSOR] = [";
        std::copy(final_strategy.begin(), --final_strategy.end(), std::ostream_iterator<T>(std::cout, ", "));
        std::cout << final_strategy.back() << "]\n" << "\n";
    }
};

#endif