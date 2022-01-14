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
    SetConsoleOutputCP(CP_UTF8);

    //Game game;
    //game.run(10);

    Hand deck = {1,5,9,13,17,21,25,29,33};
    GameTree tree;
    tree.build_tree(deck);
    tree.print_tree();

    // Card card = 1;
    // UTILS::display_card(card);
    // UTILS::print_card(card);

    

    // Enable buffering to prevent VS from chopping up UTF-8 byte sequences
    // setvbuf(stdout, nullptr, _IOFBF, 1000);
}

