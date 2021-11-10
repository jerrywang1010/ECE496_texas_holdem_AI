//
// Created by Nan Ni on 2021-11-08.
//

#include "kuhn_poker.h"
#include <string>
#include <vector>
using namespace std;
// compare the string of history to all possible moves to decide wether we reach the terminal node or not
// all possible moves include: BC, BB, CC, CBB, CBC
bool kuhn_poker::is_terminal_node(string history)
{
    if (find(all_possible_moves.begin(), all_possible_moves.end(), history) != all_possible_moves.end())
    {
        return true;
    }
    return false;

}
// calculate utility of terminal nodes
float kuhn_poker::get_utility(string history, vector<char> cards)
{
    // in Texas holdem, this is the fold case
    // in the case of BC and CBC（fold), only win +1
    if (history == "BC" || history == "CBC")
    {
        return 1;
    }
    // reach showdown : CC, BB, CBB
    int utility = 1; //for CC
    if (find(history.begin(), history.end(), 'B') != history.end())
        utility = 2;
    int active_player = history.length() % 2;
    int opponent = (active_player + 1) % 2;
    if (cards[active_player] == 'K' ||  cards[opponent] == 'J')
        return utility;
    else
        return -utility;
    // in texas holdem, player info can be stored in the node
    // for +/- call function to decide whether loss/win

}