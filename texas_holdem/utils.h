#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>
#include <stdexcept>
#include <C:\boost\include\boost-1_77\boost\format.hpp>

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define CYAN    "\033[0;36m"    /* Cyan */
#define MAGENTA "\033[0;35m"    /* Magenta */


typedef unsigned Card;
typedef std::vector<Card> Hand;

const std::string s[] = 
{
    u8"┌─────────┐",
    // rank here
    u8"│         │",
    u8"│         │",
    // suit here
    u8"│         │",
    u8"│         │",
    // rank here
    u8"└─────────┘"
};

const static std::unordered_map<unsigned, std::string> suits ( { {0, u8"♠"}, {1, u8"♥"}, {2, u8"♣"}, {3, u8"♦"} } );
const static std::unordered_map<unsigned, std::string> ranks ( { {1, "A "}, {2, "2 "}, {3, "3 "}, {4, "4 "}, {5, "5 "}, {6, "6 "}, {7, "7 "}, 
                                                                 {8, "8 "}, {9, "9 "}, {10, "10"}, {11, "J "}, {12, "Q "}, {13, "K "} } );
// const static std::unordered_map<std::string, unsigned> suits_idx ( { {"spade", 0}, {"heart", 1}, {"club", 2}, {"diamond", 3} } );

/*
typedef struct Card_visualization
{
    unsigned rank;
    std::string suit;
} Card_visualization;
*/

enum class Action : std::int16_t
{
    FOLD    = 0,
    CHECK   = 1,
    BET     = 2,
    INVALID = 3
};

enum class Round_result : std::int16_t
{
    WIN     = 0,
    LOSS    = 1,
    TIE     = 2,
    INVALID = 3
};

enum class Round : std::int16_t
{
    PRE_FLOP = 0,
    FLOP     = 1,
    TURN     = 2,
    RIVER    = 3,
    SHOWDOWN = 4
};

enum class Node_type : std::int16_t
{
    Terminal = 0,
    Chance   = 1,
    Action   = 2,
    Showdown = 3,
    Invalid  = 4
};

namespace UTILS
{
/**
 * The functions accepts a card or a hand of cards and display them in a more readable form
 *
 * @param  card
 * @return void 
 */
static void print_card(Card card)
{
    unsigned rank = card / 4 + 2;
    if (rank > 13) 
    {
        rank -= 13;
    }
    std::string suit = suits.at(card % 4);
    
    std::cout << "rank=" << rank << " ,suit=" << suit << std::endl;
}

template<class T>
static void print_vectors(const std::vector<T> & my_vector)
{
    for(int i = 0; i < my_vector.size(); i++)
    {
        std::cout << my_vector[i] << " ";
    }
    std::cout << std::endl;
}





/**
 * The function return an uint representation of a card, so that it could be used to evaluate a hand
 * CardIdx is an integer between 0 and 51, so that CARD = 4 * RANK + SUIT, where
 * rank ranges from 0 (deuce) to 12 (ace) and suit is from 0 (spade), 1 (heart), 2 (club), 3 (diamond).
 *
 * @param  hand
 * @return 16bit 
 */
// inline unsigned translate_card_to_idx(Card_visualization card)
// {   
//     return 4 * ((card.rank + 11) % 13) + suits_idx.at(card.suit);
// }


/**
 * The functions accepts a card or a hand of cards and display them in a more readable form
 *
 * @param  hand
 * @return none 
 */
static void display_card(Card card)
{   
    unsigned rank = card / 4 + 2;
    if (rank > 13) 
    {
        rank -= 13;
    }
    std::string r = ranks.at(rank);
    std::string suit = suits.at(card % 4);

    std::cout << s[0] << "\n";
    std::cout << boost::format("│%1%       │\n") % r;
    std::cout << s[1] << "\n" << s[2] << "\n";
    std::cout << boost::format("│    %1%    │\n") % suit;
    std::cout << s[3] << "\n" << s[4] << "\n";
    std::cout << boost::format("│       %1%│\n") % r;
    std::cout << s[5] << "\n";
}


static void display_hand(Hand hand)
{
    unsigned size = hand.size();
    for (int i = 0; i < size; i ++) std::cout << s[0] << " ";
    std::cout << "\n"; 
    for (const auto &card : hand)
    {
        unsigned rank = card / 4 + 2;
        if (rank > 13)
        {
            rank -= 13;
        }
        std::string r = ranks.at(rank);
        std::cout << boost::format("│%1%       │ ") % r;
    }
    std::cout << "\n";
    for (int i = 0; i < size; i ++) std::cout << s[1] << " ";
    std::cout << "\n";
    for (int i = 0; i < size; i ++) std::cout << s[2] << " ";
    std::cout << "\n";

    for (const auto &card : hand)
    {
        std::string suit = suits.at(card % 4);
        std::cout << boost::format("│    %1%    │ ") % suit;
    }
    std::cout << "\n";
    for (int i = 0; i < size; i ++) std::cout << s[3] << " ";
    std::cout << "\n";
    for (int i = 0; i < size; i ++) std::cout << s[4] << " ";
    std::cout << "\n";

    for (const auto &card : hand)
    {
        unsigned rank = card / 4 + 2;
        if (rank > 13)
        {
            rank -= 13;
        }
        std::string r = ranks.at(rank);
        std::cout << boost::format("│       %1%│ ") % r;
    }
    std::cout << "\n";
    for (int i = 0; i < size; i ++) std::cout << s[5] << " ";
    std::cout << "\n";
}
}
