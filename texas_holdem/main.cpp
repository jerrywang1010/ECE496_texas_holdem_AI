#include <iostream>
#include <string>
#include <Windows.h>
#include <cstdio>
#include <fstream>
// #include "game_rule.h"
#include "game.h"
#include "gametree.h"
#include "Psapi.h"
#include "CFR_Trainer.h"
#include <ctime>

/*
memeory usage:
before optimization: 
    9 cards deck: 514449408 bytes
    10 cards deck: 4386779136 bytes

use action history encoding: (save ~30%)
    9 cards deck: 361824256 bytes
    10 cards deck: 3054977024 bytes

use 8 bits committed:
    9 cards deck: 361803776 bytes
    10 cards deck: 3054944256 bytes

use 16 byte infoset:
    9 cards deck: 206110720 bytes
    10 cards deck: 1718247424 bytes

    9 cards deck: 322002944 bytes

after game tree modification:
    9 cards deck:  339939328 bytes
    10 cards deck: 2867847168 bytes
                

after game tree modification & infoset encoding:
    9 cards deck: 170315776 bytes
    10 cards deck: 1423507456 bytes
    13 cards deck (100 combination): 2147975168 bytes
    13 cards deck (100 combination) with infoset map 3035860992 bytes
*/


int main()
{
    // Set console code page to UTF-8 so console known how to interpret string data
    SetConsoleOutputCP(CP_UTF8);

    /*
    boost::unordered_map<Infoset, int> imap;
    
    Infoset i1, i2;
    i1.action_history = 2;
    i2.action_history = 2;
    
    i1.community_card = 11;
    i2.community_card = 11;
    
    i1.private_card = 0;
    i2.private_card = 0;
    
    bool inserted = false;
    std::tie(std::ignore, inserted) = imap.insert({i1, 1});
    std::cout << inserted << std::endl;

    inserted = false;
    std::tie(std::ignore, inserted) = imap.insert({i2, 1});
    std::cout << inserted << std::endl;

    inserted = false;
    std::tie(std::ignore, inserted) = imap.insert({i1, 1});
    std::cout << inserted << std::endl;
    */

    // Game game;
    // game.run(10);
    // Hand deck = {1,5,9,13,17,21,25,29,37};
    // Hand deck = {10,2,3,40,5,6,27,38,9};
    // Hand deck = {1,2,3,4,5,6,7,8,9,10,11,12,13};
    std::clock_t t_start = std::clock();
    Hand deck = {51,50,1,5,16,21,26,29,30};
    UTILS::display_hand(deck);

    // Hand deck = {1,5,6,10,14,16,21,26,29};
    GameTree tree;
    tree.build_tree(deck);
    std::clock_t t_end = std::clock();

    std::cout << "Build tree took CPU time: " << (t_end - t_start) / CLOCKS_PER_SEC << " s\n";

    t_start = std::clock();
    CFR_Trainer trainer(tree);
    trainer.train(50);
    trainer.compute_nash_eq();
    t_end = std::clock();
    std::cout << "CFR Trainer took CPU time: " << (t_end - t_start) / CLOCKS_PER_SEC << " s\n";


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


    // Infoset i1 = {0x3, 0, 69};
    // Infoset i1 = {1023, 4282013, 69};
    // Infoset i1 = {0, 0, 3314};
    // river round player 1, private hand = 6,9 010000 011110
    uint32_t community_card = 0;
    uint32_t action_history = 0;
    uint16_t private_card = 0;
    // 6 and 9
    ADD_TO_HAND(private_card, 16);
    ADD_TO_HAND(private_card, 30);
    Infoset i1 = {action_history, community_card, private_card};
    std::cout << "Preflop round:\nsigma=";
    UTILS::print_vec<float>(trainer.infoset_map.at(i1).sigma, std::cout);
    std::cout << "cumulative_sigma=";
    UTILS::print_vec<float>(trainer.infoset_map.at(i1).cumulative_sigma, std::cout);
    std::cout << "cumulative_cfr_regret=";
    UTILS::print_vec<float>(trainer.infoset_map.at(i1).cumulative_cfr_regret, std::cout);


    ADD_TO_HAND(community_card, 1);
    ADD_TO_HAND(community_card, 5);
    ADD_TO_HAND(community_card, 29);
    ADD_TO_ACTIONS(action_history, Action::BET);
    ADD_TO_ACTIONS(action_history, Action::BET);
    i1 = {action_history, community_card, private_card};
    std::cout << "Flop round:\nsigma=";
    UTILS::print_vec<float>(trainer.infoset_map.at(i1).sigma, std::cout);
    std::cout << "cumulative_sigma=";
    UTILS::print_vec<float>(trainer.infoset_map.at(i1).cumulative_sigma, std::cout);
    std::cout << "cumulative_cfr_regret=";
    UTILS::print_vec<float>(trainer.infoset_map.at(i1).cumulative_cfr_regret, std::cout);


    ADD_TO_HAND(community_card, 21);
    ADD_TO_ACTIONS(action_history, Action::BET);
    ADD_TO_ACTIONS(action_history, Action::BET);
    i1 = {action_history, community_card, private_card};
    std::cout << "Turn round:\nsigma=";
    UTILS::print_vec<float>(trainer.infoset_map.at(i1).sigma, std::cout);
    std::cout << "cumulative_sigma=";
    UTILS::print_vec<float>(trainer.infoset_map.at(i1).cumulative_sigma, std::cout);
    std::cout << "cumulative_cfr_regret=";
    UTILS::print_vec<float>(trainer.infoset_map.at(i1).cumulative_cfr_regret, std::cout);


    ADD_TO_HAND(community_card, 26);
    ADD_TO_ACTIONS(action_history, Action::BET);
    ADD_TO_ACTIONS(action_history, Action::BET);
    i1 = {action_history, community_card, private_card};
    std::cout << "River round:\nsigma=";
    UTILS::print_vec<float>(trainer.infoset_map.at(i1).sigma, std::cout);
    std::cout << "cumulative_sigma=";
    UTILS::print_vec<float>(trainer.infoset_map.at(i1).cumulative_sigma, std::cout);
    std::cout << "cumulative_cfr_regret=";
    UTILS::print_vec<float>(trainer.infoset_map.at(i1).cumulative_cfr_regret, std::cout);
    // std::ofstream output;
    // output.open("test.txt");
    // tree.print_tree(output);
    // // tree.print_tree();
    // output.close();

    // Card card = 1;
    // UTILS::display_card(card);
    // UTILS::print_card(card);

    // Enable buffering to prevent VS from chopping up UTF-8 byte sequences
    // setvbuf(stdout, nullptr, _IOFBF, 1000);
}

