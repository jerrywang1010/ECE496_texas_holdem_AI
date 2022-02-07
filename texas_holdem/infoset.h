#pragma once
#include <stdint.h>
#include "utils.h"
#include <C:\boost\include\boost-1_77\boost\functional\hash.hpp>

typedef struct Infoset
{
    // use 32 bit to represent action history, 2 bits per action, lower bits are more recent
    // default to be all Action::INVALID 
    uint32_t action_history = 0;

    // use 32 bit to represent upto 5 community cards, each card is 6 bits ranging from 0x0 to 0x33
    uint32_t community_card = 0;

    // use 16 bit to represent 2 private cards, each card is 6 bits ranging from 0x0 to 0x33
    uint16_t private_card = 0;

    uint8_t round_idx = 0;
    uint8_t active_player_idx = 0;

    bool operator== (Infoset const & other) const
    {
        return ((action_history == other.action_history) &&
                (community_card == other.community_card) &&
                (private_card   == other.private_card));
    }
    
} Infoset;


typedef struct Infoset_value
{
    /*
    // use 32 bit to represent action history, 2 bits per action, lower bits are more recent
    // default to be all Action::INVALID 
    uint32_t actionh_istory = 0;

    // use 32 bit to represent upto 5 community cards, each card is 6 bits ranging from 0x0 to 0x33
    uint32_t community_card = 0;

    // use 16 bit to represent 2 private cards, each card is 6 bits ranging from 0x0 to 0x33
    uint16_t private_card = 0;

    // committed can not exceed 255
    std::pair<uint8_t, uint8_t> committed = {0, 0};

    // round_idx ranging from 0 to 3
    uint8_t round_idx = 0;
    */

    // i=0 : BET
    // i=1 : FOLD
    // i=2 : CHECK
    std::vector<float> sigma;
    std::vector<float> cumulative_sigma;
    std::vector<float> cumulative_cfr_regret;

    /*
    void add_to_action_history(Action a)
    {
        assert(a != Action::INVALID);
        ADD_TO_ACTIONS(action_history, a);
    }

    void add_to_private_card(Hand h) 
    {
        for (Card c : h) {ADD_TO_HAND(private_card, c);}
    }

    void add_to_community_card(Hand h) 
    {
        for (Card c : h) {ADD_TO_HAND(community_card, c);}
    }

    void commit(int player_idx, int bet) 
    {
        if(player_idx == 0)
        {
            committed.first += bet;
        }
        else
        {
            committed.second += bet;
        }
    }
    void print_infoset(std::ostream& s)
    {
        s << "========INFORMATION SET========" << "\n";
        if (action_history != 0)
        {
            s << "Action_history: ";
            UTILS::print_action_history(action_history, s);
        }

        if (private_card != 0)
        {
            // 0xCF2 is the largest possible private card = {51, 50} in binary
            assert(private_card >= 0 && private_card <= 0xCF2);
            s << "Private_card: ";
            UTILS::print_hand<uint16_t>(private_card, s, 2);
        }
        if (community_card != 0)
        {
            // 0x33CB1C2F is the largest possible community card = {51, 50, 49, 48, 47} in binary
            assert(community_card >= 0 && community_card <= 0x33CB1C2F);
            s << "Community_card: ";
            // flop
            if (round_idx == 1) UTILS::print_hand<uint32_t>(community_card, s, 3);
            // turn
            else if (round_idx == 2) UTILS::print_hand<uint32_t>(community_card, s, 4);
            // river
            else if (round_idx == 3) UTILS::print_hand<uint32_t>(community_card, s, 5);
            else fprintf(stderr, "round_idx=%d in print_infoset\n", round_idx);
        }

        s << "Committed: [" << static_cast<int>(committed.first) << ", " << static_cast<int>(committed.second) << "]" << "\n";

        s << "===============================" << "\n";
    }
    */
} Infoset_value;


inline char action_to_char(const Action a)
{
    if (a == Action::BET) return 'B';
    else if (a == Action::FOLD) return 'F';
    else if (a == Action::CHECK) return 'C';
    else assert(false && "Action is not valid\n");
    return 'I';
}

std::size_t hash_value(Infoset const& i);

std::string decode_infoset(Infoset infoset);

Infoset encode_infoset(const Hand & private_card, const Hand & community_card, const std::vector<Action> & action_history);
