//
// Created by Nan Ni on 2021-11-08.
//

#ifndef KUHN_POKER_KUHN_POKER_CFR_TRAINER_H
#define KUHN_POKER_KUHN_POKER_CFR_TRAINER_H
#include <unordered_map>
#include <string>
#include <vector>
#include <map>
#include "information_set.h"
#include "kuhn_poker.h"
using namespace std;
class kuhn_poker_cfr_trainer {
public:
    map<string, information_set> hash_node_infoset;
    information_set get_infoset(string history);
    void set_infoset(string history,information_set set);
    float cfr(kuhn_poker poker, string history, vector<char> cards, int active, vector<float> prob);
    float train(int num_iter);
};


#endif //KUHN_POKER_KUHN_POKER_CFR_TRAINER_H
