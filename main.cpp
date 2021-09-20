/*
    ///// scratch pad area for brain storming and TODOs /////

    game of rock paper scissors, assume 2 players

    # TO DO:
    1. move different components into different headers and classes (please add header guards)
    2. please follow fuction comment template

*/

#include <iostream>
#include "game.h"
#include "utils.h"
#include "art.h"
#include "RPSTrainer.h"


int main()
{
    // Game game (2);
    // game.play();
    RPSTrainer trainer;
    std::vector<double> final_strategy = trainer.train(10000);
    trainer.display_strategy(final_strategy);
    return 0;
}

