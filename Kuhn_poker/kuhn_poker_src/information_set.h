//
// Created by Nan Ni on 2021-11-08.
//

#ifndef KUHN_POKER_INFORMATION_SET_H
#define KUHN_POKER_INFORMATION_SET_H
#include <vector>
using namespace std;
// store in nodes in texas holdem
class information_set {
public:
    vector<float> cumulative_regrets = {0, 0};
    vector<float> strategy_sum = {0, 0};
    int num_actions = 2;

    vector<float> normalize(vector<float> strategy);
    // get for each iteration
    vector<float> get_strategy(float probability);
    // get for all iterations
    vector<float> get_average_strategy();
};


#endif //KUHN_POKER_INFORMATION_SET_H
