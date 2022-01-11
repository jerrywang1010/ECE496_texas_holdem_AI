#include <iostream>
#include <string>
#include <Windows.h>
#include <cstdio>
// #include "game_rule.h"
#include "game.h"

int main()
{
    // Set console code page to UTF-8 so console known how to interpret string data
    SetConsoleOutputCP(CP_UTF8);

    Game game;
    game.run(10);

    // Enable buffering to prevent VS from chopping up UTF-8 byte sequences
    // setvbuf(stdout, nullptr, _IOFBF, 1000);
}

