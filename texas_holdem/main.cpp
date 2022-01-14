#include <iostream>
#include <string>
#include <Windows.h>
#include <cstdio>
// #include "game_rule.h"
#include "game.h"
#include "gametree.h"

int main()
{
    // Set console code page to UTF-8 so console known how to interpret string data
    //SetConsoleOutputCP(CP_UTF8);

    //Game game;
    //game.run(10);

    Hand deck = {1,2,3,4,5,6,7,8,9,10,11,12,13};
    GameTree tree;
    tree.build_tree(deck);

    

    // Enable buffering to prevent VS from chopping up UTF-8 byte sequences
    // setvbuf(stdout, nullptr, _IOFBF, 1000);
}

