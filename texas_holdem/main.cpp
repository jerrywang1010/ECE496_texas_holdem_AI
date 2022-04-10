#include <iostream>
#include <string>
#include <Windows.h>
#include <cstdio>
#include <fstream>
#include <ios>
// #include "game_rule.h"
#include "game.h"
#include "gametree.h"
#include "Psapi.h"
#include "CFR_Trainer.h"
#include <ctime>
#include <C:\boost\include\boost-1_77\boost\serialization\boost_unordered_map.hpp>
#include <C:\boost\include\boost-1_77\boost\range\irange.hpp>
#include <C:\boost\include\boost-1_77\boost\range\algorithm_ext\push_back.hpp>


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


void save_map(const InfosetMap& m, const std::string& file="map.bin")
{
    std::ofstream filestream(file, std::ios::binary);
    boost::archive::binary_oarchive archive(filestream, boost::archive::no_codecvt);
    archive << m;
}


void load_map(InfosetMap* m_ptr, const std::string& file="map.bin")
{
    std::ifstream filestream(file, std::ios::binary);
    boost::archive::binary_iarchive archive(filestream, boost::archive::no_codecvt);
    archive >> *m_ptr;
}


int main()
{
    // Set console code page to UTF-8 so console known how to interpret string data
    SetConsoleOutputCP(CP_UTF8);

    /*
    InfosetMap m;
    Infoset i1, i2;
    // fold
    i1.action_history = 1;
    // check
    i2.action_history = 2;
    // 1, 1, 1
    i1.community_card = 4161;
    // 2, 2, 2
    i2.community_card = 8322;
    // 3, 3
    i1.private_card = 195;
    // 4, 4
    i2.private_card = 260;

    i1.round_idx = 1;
    i2.round_idx = 1;

    std::vector<float> v1 = {0.5, 0.5};
    std::vector<float> v2 = {0.3, 0.3, 0.3};

    Infoset_value iv1, iv2;
    iv1.sigma = v1;
    iv1.cumulative_sigma = v1;
    iv1.cumulative_cfr_regret = v1;

    iv2.sigma = v2;
    iv2.cumulative_sigma = v2;
    iv2.cumulative_cfr_regret = v2;
    m.insert(std::make_pair(i1, iv1));
    m.insert(std::make_pair(i2, iv2));

    save_map(m);
    m.clear();
    std::cout << "map is cleared, size=" << m.size() << "\n";
    load_map(&m);
    std::cout << "map is loaded, size=" << m.size() << "\n";
    m.clear();
    for (const auto & kv : m)
    {
        std::string key = decode_infoset(kv.first);
        std::cout << "key=" << key << ", sigma=";
        UTILS::print_vec<float>(kv.second.sigma, std::cout);
        std::cout << "cumulative_sigma=";
        UTILS::print_vec<float>(kv.second.cumulative_sigma, std::cout);
        std::cout << "cumulative_cfr_regret=";
        UTILS::print_vec<float>(kv.second.cumulative_cfr_regret, std::cout);
    }
    */

    // Ace to King
    Hand training_deck = {1,5,9,13,17,21,25,29,33,37,41,45,49};

    // Ace to 9
    // Hand training_deck = {1,5,9,13,17,21,25,29,33};
    // Hand training_deck = {10,2,3,40,5,6,27,38,9};
    // Hand training_deck = {1,2,3,4,5,6,7,8,9,10,11,12,13};
    // UTILS::display_hand(training_deck);

    GameTree tree;
    CFR_Trainer trainer;


    // write strategy to json file
    {
        Hand playing_deck;
        boost::push_back(playing_deck, boost::irange(0, 52));
        trainer.infoset_map.clear();
        std::cout << "infoset map is cleared\n";
        load_map(&trainer.infoset_map, "13_cards_p0_200_iter_strat.bin");
        Game game(playing_deck);
        game.m_players = {Player(&trainer, playing_deck, "CFR_bot", 10000)};
        game.output_strategy_as_json(10, "test.json");
    }

    /*
    {
        Hand playing_deck;
        // playing_deck = training_deck;
        boost::push_back(playing_deck, boost::irange(0, 52));
        trainer.infoset_map.clear();
        std::cout << "infoset map is cleared\n";
        load_map(&trainer.infoset_map, "13_cards_p0_200_iter_strat.bin");

        Game game(playing_deck);
        fixed_strat fs = {{0.5f, 0.5f}, {1/3.0, 1/3.0, 1/3.0}};
        // fixed_strat fs = {{0.9f, 0.1f}, {0.8f, 0.1f, 0.1f}};
        game.m_players = {Player(&trainer, playing_deck, "CFR_bot", 10000), Player(fs, "Random_bot", 10000)};
        // game.m_players = {Player(&trainer, playing_deck, "CFR_bot_1", 10000), Player(&trainer, playing_deck, "CFR_bot_2", 10000)};
        int games_num = 10000;
        game.run(games_num);
        std::cout << "Player 0 winning rate: " << game.winning_records[0] / (double)games_num << std::endl;
        std::cout << "Player 1 winning rate: " << game.winning_records[1] / (double)games_num << std::endl;

        Game new_game(playing_deck);
        new_game.m_players = {Player(fs, "Random_bot", 10000), Player(&trainer, playing_deck, "CFR_bot", 10000)};
        new_game.run(games_num);
        std::cout << "Player 0 winning rate: " << new_game.winning_records[0] / (double)games_num << std::endl;
        std::cout << "Player 1 winning rate: " << new_game.winning_records[1] / (double)games_num << std::endl;
    }
    */

    /*
    tree.init_all_combo_first_round(training_deck, false);
    int game_tree_first_level_partition = 10;
    size_t total_combo_size = tree.all_combo_first_round.size();
    size_t interval = total_combo_size / game_tree_first_level_partition;
    
    for (size_t i = 0; i * interval < total_combo_size; i ++)
    {
        // build subtree
        std::clock_t t_start = std::clock();
        tree.build_tree(training_deck, i * interval, interval);
        int start = i * interval;
        int end = std::min((i + 1) * interval, total_combo_size);
        std::cout << "start_idx=" << start << ", end_idx=" << end << "\n";
        std::clock_t t_end = std::clock();
        std::cout << "Build " << i << "th/" << game_tree_first_level_partition <<  " tree took CPU time: " << (t_end - t_start) / CLOCKS_PER_SEC << " s\n";
        t_start = std::clock();

        // train on subtree
        // first replace with the new subtree
        trainer.tree = &tree;
        trainer.trainer_init();
        trainer.train(200);
        trainer.compute_nash_eq();
        t_end = std::clock();
        std::cout << "CFR Trainer took CPU time on " << i << "th/" << game_tree_first_level_partition << " tree: " << (t_end - t_start) / CLOCKS_PER_SEC << " s\n";

        // system dependent code for memory profiling
        DWORDLONG virtual_mem_size_before_free, virtual_mem_size_after_free;
        //get the handle to this process
        auto myHandle = GetCurrentProcess();
        //to fill in the process' memory usage details
        PROCESS_MEMORY_COUNTERS pmc;
        //return the usage (bytes), if I may
        if (GetProcessMemoryInfo(myHandle, &pmc, sizeof(pmc)))
            virtual_mem_size_before_free = pmc.WorkingSetSize;
        else
            virtual_mem_size_before_free = 0;
        // free subtree
        tree.free_tree_nodes();
        if (GetProcessMemoryInfo(myHandle, &pmc, sizeof(pmc)))
            virtual_mem_size_after_free = pmc.WorkingSetSize;
        else
            virtual_mem_size_after_free = 0;

        std::cout << "freed memory size=" << virtual_mem_size_before_free - virtual_mem_size_after_free << std::endl;
    }

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
    std::cout << "total virtual memory used by storing strategy=" << virtual_mem_size << std::endl;
    {
        save_map(trainer.infoset_map, "13_cards_p0_200_iter_strat.bin");
        // Hand playing_deck;
        // boost::push_back(playing_deck, boost::irange(0, 52));
        Hand playing_deck = training_deck;
        Game game(playing_deck);

        // fixed_strat<0> = strat[bet, fold] (when check is not allowed)
        // fixed_strat<1> = strat[bet, fold, check] (when check is allowed)
        // fixed_strat fs = {{0.9f, 0.1f}, {0.8f, 0.1f, 0.1f}};
        // game.m_players = {Player(&trainer, deck, "CFR_bot", 10000), Player(fs, "Aggressive_bot", 10000)};

        fixed_strat fs = {{0.5f, 0.5f}, {1/3.0, 1/3.0, 1/3.0}};
        // fixed_strat fs = {{0.9f, 0.1f}, {0.8f, 0.1f, 0.1f}};
        game.m_players = {Player(&trainer, playing_deck, "CFR_bot", 10000), Player(fs, "Random_bot", 10000)};
        // game.m_players = {Player(&trainer, playing_deck, "CFR_bot", 10), Player("Me", 10)};
        // game.m_players = {Player(fs, "Random_bot", 10000), Player(&trainer, playing_deck, "CFR_bot", 10000)};
        int games_num = 100000;
        game.run(games_num);
        std::cout << "Player 0 winning rate: " << game.winning_records[0] / (double)games_num << std::endl;
        std::cout << "Player 1 winning rate: " << game.winning_records[1] / (double)games_num << std::endl;
    }
    */

    /*
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
    */

    // std::ofstream output;
    // output.open("test.txt");
    // tree.print_tree(output);
    // // tree.print_tree();
    // output.close();

    // Enable buffering to prevent VS from chopping up UTF-8 byte sequences
    // setvbuf(stdout, nullptr, _IOFBF, 1000);
}

