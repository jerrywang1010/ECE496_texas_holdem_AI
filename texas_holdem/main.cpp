#include <iostream>
#include <string>
#include <Windows.h>
#include <cstdio>
#include <fstream>
// #include "game_rule.h"
#include "game.h"
#include "gametree.h"
#include "Psapi.h"

/*
memeory usage:
before optimization: 9 cards deck: 514449408 bytes
                     10 cards deck: 4386779136 bytes

*/


int main()
{
    // Set console code page to UTF-8 so console known how to interpret string data
    SetConsoleOutputCP(CP_UTF8);

    // Game game;
    // game.run(10);
    Hand deck = {1,5,9,13,17,21,25,29,37};
    GameTree tree;
    tree.build_tree(deck);

    std::ofstream output;
    output.open("test.txt");
    
    // system dependent code for memory profiling
    DWORDLONG virtual_mem_size;
    //get the handle to this process
    auto myHandle = GetCurrentProcess();
    //to fill in the process' memory usage details
    PROCESS_MEMORY_COUNTERS pmc;
    //return the usage (bytes), if I may
    if (GetProcessMemoryInfo(myHandle, &pmc, sizeof(pmc)))
        virtual_mem_size = pmc.WorkingSetSize;
    else
        virtual_mem_size = 0;

    // printf("total virtual memory used by constructing the tree=%lld\n", totalVirtualMem);
    std::cout << "total virtual memory used by constructing the tree=" << virtual_mem_size << std::endl;
    //

    tree.print_tree(output);
    output.close();
    // Card card = 1;
    // UTILS::display_card(card);
    // UTILS::print_card(card);

    // Enable buffering to prevent VS from chopping up UTF-8 byte sequences
    // setvbuf(stdout, nullptr, _IOFBF, 1000);
}

