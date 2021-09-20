#ifndef UTILS_GUARD
#define UTILS_GUARD

#include <iostream>

// a enum class for all possible moves
enum class moves : std::int16_t
{
    ROCK = 0, 
    PAPER = 1, 
    SCISSOR = 2,
    INVALID = 3
};

// a enum class for all possible results after a round
enum class round_result : std::int16_t
{
    WIN,
    TIE,
    LOSE
};

#endif