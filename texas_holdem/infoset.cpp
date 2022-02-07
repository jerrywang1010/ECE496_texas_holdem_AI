#include "infoset.h"

std::size_t hash_value(Infoset const& i)
{
    std::size_t seed = 0;
    boost::hash_combine(seed, i.action_history);
    boost::hash_combine(seed, i.community_card);
    boost::hash_combine(seed, i.private_card);
    
    return seed;
}


/*
// return string format of "private_card; community_card; action_history", each card is a 2 digit decimal number, each action is B or F or C
inline std::string construct_infoset_key(const Hand & private_card, const Hand & community_card, const std::vector<Action> & action_history)
{
    std::string infoset_key {};
    // assert(private_card.size() == 2);
    for (Card c : private_card)
    {
        if (c < 10)
        {
            infoset_key.push_back('0');
        }
        infoset_key.append(std::to_string(c));
    }
    infoset_key.push_back(';');
    
    assert(community_card.size() <= 5);
    for (Card c : community_card)
    {
        if (c < 10)
        {
            infoset_key.push_back('0');
        }
        infoset_key.append(std::to_string(c));
    }
    infoset_key.push_back(';');

    for (Action a : action_history)
    {
        infoset_key.push_back(action_to_char(a));
    }
    return infoset_key;
}
*/

// return string format of "private_card; community_card; action_history", each card is a 2 digit decimal number, each action is B or F or C
std::string decode_infoset(Infoset infoset)
{
    std::string infoset_key {};

    // 2 private cards
    for (int i = 0; i < 2; i ++)
    {
        Card c = GET_LAST_CARD(infoset.private_card);
        DEL_LAST_CARD(infoset.private_card);
        if (c < 10)
        {
            infoset_key.push_back('0');
        }
        infoset_key.append(std::to_string(c));
    }
    infoset_key.push_back(';');

    // variable length community cards
    int num_community_cards = 0;
    if (infoset.round_idx == 1) num_community_cards = 3;
    else if (infoset.round_idx == 2) num_community_cards = 4;
    else if (infoset.round_idx == 3) num_community_cards = 5;
    else if (infoset.round_idx != 0) fprintf(stderr, "infoset.round_idx is too large\n");

    for (int i = 0; i < num_community_cards; i ++)
    {
        Card c = GET_LAST_CARD(infoset.community_card);
        DEL_LAST_CARD(infoset.community_card);
        if (c < 10)
        {
            infoset_key.push_back('0');
        }
        infoset_key.append(std::to_string(c));
    }
    infoset_key.push_back(';');

    // action history
    std::stack<Action> ordered_actions;
    Action a = GET_LAST_ACTION(infoset.action_history);
    while (a != Action::INVALID)
    {
        DEL_LAST_ACTION(infoset.action_history);
        ordered_actions.push(a);
        a = GET_LAST_ACTION(infoset.action_history);
    }

    while (!ordered_actions.empty())
    {
        a = ordered_actions.top();
        ordered_actions.pop();
        infoset_key.push_back(action_to_char(a));
    }
    return infoset_key;
}


// construct a struct infoset given vector of arguments, encode into 32 bits and 16 bits uint
Infoset encode_infoset(const Hand & private_card, const Hand & community_card, const std::vector<Action> & action_history)
{
    Infoset infoset;
    for (Card c : private_card)
    {
        ADD_TO_HAND(infoset.private_card, c);
    }
    for (Card c : community_card)
    {
        ADD_TO_HAND(infoset.community_card, c);
    }
    for (Action a : action_history)
    {
        ADD_TO_ACTIONS(infoset.action_history, a);
    }
    return infoset;
}
