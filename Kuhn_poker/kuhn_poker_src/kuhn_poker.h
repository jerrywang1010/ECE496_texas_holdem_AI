//
// Created by Nan Ni on 2021-11-08.
//

#ifndef KUHN_POKER_KUHN_POKER_H
#define KUHN_POKER_KUHN_POKER_H
#include <string>
#include <vector>
using namespace std;
class kuhn_poker {
public:
    vector<string> all_possible_moves = {"BC", "BB", "CC", "CBB", "CBC"};
    bool is_terminal_node(string history);
    float get_utility(string history, vector<char> cards);
};


#endif //KUHN_POKER_KUHN_POKER_H
