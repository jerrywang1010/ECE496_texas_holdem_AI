#pragma once

#include <iostream>
#include <vector>
#include <stack>
#include <set>
#include <unordered_map>
#include <string>
#include <memory>
#include <stdexcept>
#include <C:\boost\include\boost-1_77\boost\format.hpp>

#define DEBUG 1

#define RESET "\033[0m"
#define BLACK "\033[30m"     /* Black */
#define RED "\033[31m"       /* Red */
#define GREEN "\033[32m"     /* Green */
#define YELLOW "\033[33m"    /* Yellow */
#define CYAN "\033[0;36m"    /* Cyan */
#define MAGENTA "\033[0;35m" /* Magenta */

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
        u8"└─────────┘"};

// const static std::unordered_map<unsigned, std::string> suits ( { {0, "Spade"}, {1, "Heart"}, {2, "club"}, {3, "Diamond"} } );
const static std::unordered_map<unsigned, std::string> suits({{0, u8"♠"}, {1, u8"♥"}, {2, u8"♣"}, {3, u8"♦"}});
const static std::unordered_map<unsigned, std::string> ranks({{1, "A "}, {2, "2 "}, {3, "3 "}, {4, "4 "}, {5, "5 "}, {6, "6 "}, {7, "7 "}, {8, "8 "}, {9, "9 "}, {10, "10"}, {11, "J "}, {12, "Q "}, {13, "K "}});
// const static std::unordered_map<std::string, unsigned> suits_idx ( { {"spade", 0}, {"heart", 1}, {"club", 2}, {"diamond", 3} } );

/*
enum class Action : std::int16_t
{
    FOLD    = 0,
    CHECK   = 1,
    BET     = 2,
    INVALID = 3
};
*/

enum class Action : std::int16_t
{
    INVALID = 0,
    FOLD = 1,
    CHECK = 2,
    BET = 3
};

// template <typename T>
// inline Action GET_LAST_ACTION(T actions) {return static_cast<Action> (actions & 0x3);}
#define ACTION_MASK 0x3
#define CARD_MASK 0x3F

#define GET_LAST_ACTION(actions) (static_cast<Action>(actions & ACTION_MASK))
#define DEL_LAST_ACTION(actions) (actions >>= 2)
#define ADD_TO_ACTIONS(actions, a) ((actions <<= 2) |= static_cast<int16_t>(a))

#define GET_LAST_CARD(hand) (static_cast<Card>(hand & CARD_MASK))
#define DEL_LAST_CARD(hand) (hand >>= 6)
#define ADD_TO_HAND(hand, c) ((hand <<= 6) |= c)

#define debug_print(fmt, ...) \
            do { if (DEBUG) fprintf(stdout, fmt, __VA_ARGS__); } while (0)

enum class Round_result : std::int16_t
{
    WIN = 0,
    LOSS = 1,
    TIE = 2,
    INVALID = 3
};

enum class Round : std::int16_t
{
    PRE_FLOP = 0,
    FLOP = 1,
    TURN = 2,
    RIVER = 3,
    SHOWDOWN = 4
};

enum class Node_type : std::int16_t
{
    Terminal = 0,
    Chance = 1,
    Action = 2,
    Showdown = 3,
    Invalid = 4
};
// for list of all possible combinations of private 

const static std::unordered_map<Action, std::string> action_to_str({{Action::BET, "Bet"}, {Action::FOLD, "Fold"}, {Action::CHECK, "Check"}, {Action::INVALID, "Invalid"}});

namespace UTILS
{
    static const std::set<std::pair<int, int>> any_position{
        std::make_pair(1, 1),
        std::make_pair(13, 13),
        std::make_pair(12, 12),
        std::make_pair(11, 11),
        std::make_pair(10, 10),
        std::make_pair(9, 9),
        std::make_pair(8, 8),
        std::make_pair(7, 7),
        std::make_pair(1, 13),
        std::make_pair(1, 12),
        std::make_pair(1, 11),
        std::make_pair(1, 10),
        std::make_pair(13, 1),
        std::make_pair(12, 1),
        std::make_pair(11, 1),
        std::make_pair(10, 1),
        std::make_pair(13, 12),
        std::make_pair(13, 11),
        std::make_pair(13, 10),
        std::make_pair(12, 11),
        std::make_pair(12, 10),
        std::make_pair(11, 10),
        std::make_pair(11, 9),
        std::make_pair(10, 9),
        std::make_pair(12, 13),
        std::make_pair(11, 13),
        std::make_pair(10, 13),
        std::make_pair(11, 12),
        std::make_pair(10, 12),
        std::make_pair(10, 11),
        std::make_pair(9, 11),
        std::make_pair(9, 10)};

    static const std::set<std::pair<int, int>> mid_late_posotion{
        std::make_pair(9, 8),
        std::make_pair(10, 8),
        std::make_pair(12, 9),
        std::make_pair(11, 8),
        std::make_pair(13, 9),
        std::make_pair(12, 8),
        std::make_pair(1, 9),
        std::make_pair(1, 8),
        std::make_pair(1, 7),
        std::make_pair(1, 6),
        std::make_pair(6, 6),
        std::make_pair(5, 5)};

    static const std::set<std::pair<int, int>> late_posotion{
        std::make_pair(8, 7),
        std::make_pair(7, 6),
        std::make_pair(6, 5),
        std::make_pair(5, 4),
        std::make_pair(9, 7),
        std::make_pair(8, 6),
        std::make_pair(7, 5),
        std::make_pair(10, 7),
        std::make_pair(9, 6),
        std::make_pair(11, 7),
        std::make_pair(13, 8),
        std::make_pair(13, 7),
        std::make_pair(13, 6),
        std::make_pair(13, 5),
        std::make_pair(13, 4),
        std::make_pair(13, 3),
        std::make_pair(13, 2),
        std::make_pair(1, 5),
        std::make_pair(1, 4),
        std::make_pair(1, 3),
        std::make_pair(1, 2),
        std::make_pair(4, 4),
        std::make_pair(3, 3),
        std::make_pair(2, 2)};

    static const std::set<std::pair<int, int>> unplayable_hand{
        std::make_pair(4, 3),
        std::make_pair(3, 4),
        std::make_pair(3, 2),
        std::make_pair(2, 3),
        std::make_pair(6, 4),
        std::make_pair(4, 6),
        std::make_pair(5, 3),
        std::make_pair(3, 5),
        std::make_pair(4, 2),
        std::make_pair(2, 4),
        std::make_pair(8, 5),
        std::make_pair(5, 8),
        std::make_pair(7, 4),
        std::make_pair(4, 7),
        std::make_pair(6, 3),
        std::make_pair(3, 6),
        std::make_pair(5, 2),
        std::make_pair(2, 5),
        std::make_pair(10, 6),
        std::make_pair(6, 10),
        std::make_pair(9, 5),
        std::make_pair(5, 9),
        std::make_pair(8, 4),
        std::make_pair(4, 8),
        std::make_pair(7, 3),
        std::make_pair(3, 7),
        std::make_pair(6, 2),
        std::make_pair(2, 6),
        std::make_pair(12, 7),
        std::make_pair(7, 12),
        std::make_pair(11, 6),
        std::make_pair(6, 11),
        std::make_pair(10, 5),
        std::make_pair(5, 10),
        std::make_pair(9, 4),
        std::make_pair(4, 9),
        std::make_pair(8, 3),
        std::make_pair(3, 8),
        std::make_pair(7, 2),
        std::make_pair(2, 7),
        std::make_pair(12, 6),
        std::make_pair(6, 12),
        std::make_pair(12, 5),
        std::make_pair(5, 12),
        std::make_pair(12, 4),
        std::make_pair(4, 12),
        std::make_pair(12, 3),
        std::make_pair(3, 12),
        std::make_pair(12, 2),
        std::make_pair(2, 12),
        std::make_pair(11, 5),
        std::make_pair(5, 11),
        std::make_pair(11, 4),
        std::make_pair(4, 11),
        std::make_pair(11, 3),
        std::make_pair(3, 11),
        std::make_pair(11, 2),
        std::make_pair(2, 11),
        std::make_pair(10, 4),
        std::make_pair(4, 10),
        std::make_pair(10, 3),
        std::make_pair(3, 10),
        std::make_pair(10, 2),
        std::make_pair(2, 10),
        std::make_pair(9, 3),
        std::make_pair(3, 9),
        std::make_pair(9, 2),
        std::make_pair(2, 9),
        std::make_pair(8, 2),
        std::make_pair(2, 8)};

    
    template <typename T, typename A>
    static inline void print_vec(const std::vector<T, A> &v, std::ostream &s)
    {
        for (auto e : v)
        {
            s << e << ", ";
        }
        s << "\n";
    }

    /**
     * The functions accepts a card or a hand of cards and display them in a more readable form
     *
     * @param  card
     * @return void
     */
    __attribute__((unused))
    static inline void print_card(Card card, std::ostream &s)
    {
        unsigned rank = card / 4 + 2;
        if (rank > 13)
        {
            rank -= 13;
        }
        std::string suit = suits.at(card % 4);
        s << "rank=" << rank << " ,suit=" << boost::format("%1%") % suit;
        // s << suit << " " << rank;
    }


    // print cards in encoded hand h, h can be 32 bits unsigned or 16 bits unsigned
    template <typename T>
    static inline void print_hand(T h, std::ostream &s, int num)
    {
        // although it doesn't matter here the order of cards to be printed unlike action histry
        // I am just doing it for consistency purpose
        std::stack<Card> ordered_cards;
        while (num > 0)
        {
            Card c = GET_LAST_CARD(h);
            ordered_cards.push(c);
            DEL_LAST_CARD(h);
            num--;
        }
        while (!ordered_cards.empty())
        {
            Card c = ordered_cards.top();
            ordered_cards.pop();
            print_card(c, s);
            s << ", ";
        }
        s << "\n";
    }

    __attribute__((unused))
    static inline void print_action_history(uint32_t actions, std::ostream &s)
    {
        std::stack<Action> ordered_actions;
        Action a = GET_LAST_ACTION(actions);
        while (a != Action::INVALID)
        {
            DEL_LAST_ACTION(actions);
            ordered_actions.push(a);
            a = GET_LAST_ACTION(actions);
        }
        while (!ordered_actions.empty())
        {
            a = ordered_actions.top();
            ordered_actions.pop();
            s << action_to_str.at(a) << ", ";
        }
        s << "\n";
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
    __attribute__((unused))
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
        std::cout << s[1] << "\n"
                  << s[2] << "\n";
        std::cout << boost::format("│    %1%    │\n") % suit;
        std::cout << s[3] << "\n"
                  << s[4] << "\n";
        std::cout << boost::format("│       %1%│\n") % r;
        std::cout << s[5] << "\n";
    }

    __attribute__((unused))
    static void display_hand(Hand hand)
    {
        unsigned size = hand.size();
        for (unsigned i = 0; i < size; i++)
            std::cout << s[0] << " ";
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
        for (unsigned i = 0; i < size; i++)
            std::cout << s[1] << " ";
        std::cout << "\n";
        for (unsigned i = 0; i < size; i++)
            std::cout << s[2] << " ";
        std::cout << "\n";

        for (const auto &card : hand)
        {
            std::string suit = suits.at(card % 4);
            std::cout << boost::format("│    %1%    │ ") % suit;
        }
        std::cout << "\n";
        for (unsigned i = 0; i < size; i++)
            std::cout << s[3] << " ";
        std::cout << "\n";
        for (unsigned i = 0; i < size; i++)
            std::cout << s[4] << " ";
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
        for (unsigned i = 0; i < size; i++)
            std::cout << s[5] << " ";
        std::cout << "\n";
    }
}
