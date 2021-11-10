#ifndef UTILS_GUARD
#define UTILS_GUARD

#include <iostream>
#include <vector>

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

// a enum class for all possible game mode
enum class GAME_MODE : int16_t
{
    SELF_PLAY,
    ONE_VS_ONE,
    FIXED_OPP_STRAT
};

// row is other action, colomn is my action
const static std::vector<std::vector<int> > utility_table = { {0, 1, -1},
                                                              {-1, 0, 1},
                                                              {1, -1, 0} }; 

#endif
