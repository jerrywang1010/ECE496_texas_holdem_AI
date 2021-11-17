#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>

typedef unsigned Card;
typedef std::vector<Card> Hand;

const static std::unordered_map<unsigned, std::string> suits ( { {0, "spade"}, {1, "heart"}, {2, "club"}, {3, "diamond"} } );

typedef struct Card_visualization
{
    unsigned rank;
    std::string suit;
} Card_visualization;

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

