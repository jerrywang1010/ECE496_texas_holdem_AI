//
// Created by Nan Ni on 2021-11-08.
//
#include <iostream>
#include "information_set.h"
#include <vector>
using namespace std;
float sum_vector(vector<float> s)
{
    float sum = 0;
    for(int i = 0; i < s.size(); i ++)
    {
        sum += s[i];
    }
    return sum;
}
vector<float> information_set::normalize(vector<float> strategy)
{
    float sum = sum_vector(strategy);
    vector<float> res;
    if (sum > 0)
    {
        for (int i = 0; i < strategy.size(); i ++)
        {
            res.push_back(strategy[i] / sum);
        }
    }
    else
    {
        for (int i = 0; i < strategy.size(); i ++)
        {
            res.push_back(1.0 / strategy.size());
        }
    }
    return res;
}
// get for each iteration
vector<float> information_set::get_strategy(float probability)
{
    vector<float> tmp;
    //cout << "cumulative regret: [";
    for (int i = 0; i < cumulative_regrets.size(); i ++)
    {

        float x = 0;
        if (cumulative_regrets[i] > 0)
        {
            x = cumulative_regrets[i];

        }
        //cout << cumulative_regrets[i] << " ";

        tmp.push_back(x);
    }
    //cout << "]" << endl;
    vector<float> res = normalize(tmp);
    for (int i = 0; i < res.size(); i ++)
    {
        //cout << res[i] << endl;
        strategy_sum[i] += (probability * res[i]);
    }
    return res;
    return res;

}
// get for all iterations
vector<float> information_set::get_average_strategy(){
    vector<float> res = normalize(strategy_sum);
    return res;
}