#include <iostream>
// #include "game_rule.h"
#include "game.h"

int main()
{
    // Hand hand_1 = {Card(9, 'c'), Card(13, 'd'), Card(8, 'd'), Card(9, 'd'), Card(2, 'd')};
    // Hand hand_2 = {Card(9, 'h'), Card(13, 'd'), Card(8, 'd'), Card(9, 's'), Card(2, 'd')};

    // Game_rule game_rule;
    // std::cout << game_rule.get_card_ranking(hand_1) << "\n";
    // std::cout << game_rule.get_card_ranking(hand_2) << "\n";

    Game game;
    game.run(10);
}

