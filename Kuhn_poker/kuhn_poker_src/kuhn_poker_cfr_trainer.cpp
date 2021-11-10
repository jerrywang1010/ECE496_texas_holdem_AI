//
// Created by Nan Ni on 2021-11-08.
//

#include "kuhn_poker_cfr_trainer.h"
#include "kuhn_poker.h"
#include <unordered_map>
#include <string>
#include <vector>
#include "information_set.h"
#include <stdio.h>
#include <cstdio>
#include <iostream>
#include <algorithm>
#include <random>


using namespace std;


information_set kuhn_poker_cfr_trainer::get_infoset(string history){
    if (hash_node_infoset.find(history) == hash_node_infoset.end())
    {
        hash_node_infoset[history] = information_set();
    }
    return hash_node_infoset[history];
}

void kuhn_poker_cfr_trainer::set_infoset(string history,information_set set){
    hash_node_infoset[history] = set;
}

float kuhn_poker_cfr_trainer::cfr(kuhn_poker poker, string history, vector<char> cards, int active, vector<float> prob) {
    if (poker.is_terminal_node(history)) {
        return poker.get_utility(history, cards);
    }


    information_set infoset = get_infoset(cards[active] + history);
    char my_card = cards[active];
    vector<float> strategy = infoset.get_strategy(prob[active]);
    //cout << "node: (" << my_card << ") " << active << history << endl;
    //cout << "strategy[" << strategy[0] << "," << strategy[1] << "]"<< endl;
    int opponent = (active + 1) % 2;
    vector<float> counterfactual = {0, 0};

    vector<char> actions = {'B', 'C'};
    for (int i = 0; i < actions.size(); i++)
    {
        float action_prob = strategy[i];
        vector<float> new_prob = prob;
        new_prob[active] *= action_prob;
        counterfactual[i] = -cfr(poker, history + actions[i], cards, opponent, new_prob);
    }
    float node_val = counterfactual[0] * strategy[0] + counterfactual[1] * strategy[1];
    //cout << "cumulative_regret update: ";
    for (int i = 0; i < actions.size(); i ++)
    {

        infoset.cumulative_regrets[i] += prob[opponent] * (counterfactual[i] - node_val);
        //cout << infoset.cumulative_regrets[i] << " ";
    }
    //cout << endl;
    set_infoset(cards[active]+ history,infoset);
    return node_val;

}
float kuhn_poker_cfr_trainer::train(int num_iter){
    vector<char> kuhn = {'J', 'Q', 'K'};
    vector<char> cards = {'J', 'K'};
    random_device rd;
    auto rng = default_random_engine {rd()};

    kuhn_poker poker;
    //int d = 100000;
    float n;
    while (num_iter) {
        //cout << "iteration:" << 10-d << "=========================" <<endl;

        shuffle(std::begin(kuhn), std::end(kuhn), rng);
        cards = vector<char>(kuhn.begin(),kuhn.end()-1);
        string history = "";
        vector<float> prob = {1, 1};
        n += cfr(poker, history, cards, 0, prob);
        num_iter --;
    }
    return n;
}
