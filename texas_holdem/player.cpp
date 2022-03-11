#include "player.h"
#include "assert.h"
#include <algorithm>
#include <chrono>
#include <random>

/**
 * The function updates the balance of a player, if he wins or losses money
 *
 * @param  int
 * @return void 
 */
// void Player::update_balanace(int money)
// {
//     this->m_balance += money;
// }

/**
 * The function adds card to a player's private_cards, representing avaliable cards that he could use
 *
 * @param  Hand
 * @return void 
 */
// void Player::add_to_private_cards(Hand h)
// {
//     assert(h.size() == 2);
//     this->private_cards.insert(this->private_cards.end(), h.begin(), h.end());
// }


/**
 * The function adds a hand to the players community cards
 *
 * @param  hand
 * @return void 
 */
// void Player::add_to_community_cards(Hand h)
// {
//     assert(h.size() <= 3);
//     this->community_cards.insert(community_cards.end(), h.begin(), h.end());
// }


/**
 * The functions clears the player's private cards, usually happends at the end of a game.
 *
 * @param  hand
 * @return void 
 */
// void Player::clear_private_cards()
// {
//     this->private_cards.clear();
// }


/**
 * The functions clears the player's community cards, usually happends at the end of a game.
 *
 * @param  hand
 * @return void 
 */
// void Player::clear_community_cards()
// {
//     this->community_cards.clear();
// }


/**
 * The function sets balance of a player
 * This function should only be called at the beginning of a game
 * Balance change in each round should be done with update_balance()
 *
 * @param  int
 * @return void 
 */
// void Player::set_balance(int balance)
// {
//     this->m_balance = balance;
// }


inline Action get_human_player_action()
{
    char _a;
    Action action = Action::INVALID;
    try
    {
        std::cin >> _a;
        switch (_a)
        {
            case 'F':
            case 'f':
                action = Action::FOLD;
                break;

            case 'C':
            case 'c':
                action = Action::CHECK;
                break;

            case 'B':
            case 'b':
                return Action::BET;
                break;

            default:
                throw std::runtime_error ("user entered invalid action, terminating the game\n");
                exit (-1);
                break;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        exit(0);
    }
    return action;
}


inline Action get_action_based_on_strat(std::vector<float>& strat)
{
    // get a random float between [0, 1)
    float r = static_cast<float> (rand()) / static_cast<float> (RAND_MAX);

    int num_actions = strat.size();
    assert(num_actions == 2 || num_actions == 3);
    std::vector<float> cdf (num_actions, 0);
    float sum = 0;
    int i = 0;
    for (; i < num_actions; i ++)
    {
        sum += strat[i];
        cdf[i] = sum;
        if (cdf[i] >= r) break;
    }

    switch (i)
    {
        case 0 : return Action::BET;
        case 1 : return Action::FOLD;
        case 2 : return Action::CHECK;
        
        default : return Action::INVALID;
    }
}


/**
 * The function gets an action from a player, prompt the user to input an action and check if it is valid. Returns a legal action
 *
 * @param  check_allowed
 * @return Action 
 */
Action Player::get_action(bool allow_check) const
{
    Action action = Action::INVALID;
    do
    {   
        if (allow_check)
        {
            debug_print("Player: %s\n\tPlease enter F (Fold), C (Check) or B (Bet)\n", player_name.c_str());
        }
        else
        {
            debug_print("Player: %s\n\tPlease enter F (Fold) or B (Bet)\n", player_name.c_str());
        }

        if (!is_cfr_bot && !is_fixed_strat_bot)
        {
            action = get_human_player_action();
        }
        else if (is_cfr_bot)
        {
            // use current game state to construct a infoset key
            // but need to sort the first 3 community card and private card in ascending order to keep consistency
            auto c_cards = this->community_cards;
            if (!c_cards.empty())
            {
                assert(c_cards.size() >= 3);
                std::sort(c_cards.begin(), c_cards.begin() + 3);
            }
            auto p_cards = this->private_cards;
            assert(p_cards.size() == 2);
            std::sort(p_cards.begin(), p_cards.end());

            // because action_history only stores action at the end of the previous round, there might be new actions performed in this round
            auto up_to_date_action_history = this->action_history;
            up_to_date_action_history.insert(up_to_date_action_history.end(), this->action_this_round.begin(), this->action_this_round.end());

            // now finally we can convert it into a infomap key and lookup
            Infoset infoset = encode_infoset(p_cards, c_cards, up_to_date_action_history);
            assert(this->trainer != nullptr);

            if (trainer->infoset_map.find(infoset) != trainer->infoset_map.end())
            {
                std::cout << "Found infoset_key\n";
                debug_print("%s", "Found infoset_key\nstrat=");
                std::vector<float> strat = trainer->infoset_map.at(infoset).sigma;
                assert(strat.size() == 2 || strat.size() == 3);
                if (DEBUG) UTILS::print_vec<float>(strat, std::cout);
                action = get_action_based_on_strat(strat);
            }
            else
            {
                std::cout << "Infoset_key not found in map\n";
                debug_print("%s\n", "infoset_key not found in map");
                float b=0, c=0, f=0;
                trainer->odds_calculator(this->private_cards, this->community_cards, this->deck, 50000, allow_check, b, c, f);
                std::vector<float> strat;
                if (allow_check)
                {
                    strat = {b, f, c};
                }
                else
                {
                    strat = {b, f};
                }
                action = get_action_based_on_strat(strat);
            }
        }
        else if (is_fixed_strat_bot)
        {
            auto strat = allow_check ? fs.second : fs.first;

            // std::cout << "Using fixed strategy\n";
            // UTILS::print_vec<float>(strat, std::cout);
            action = get_action_based_on_strat(strat);
            // std::cout << "action=" << action_to_str.at(action) << std::endl;
        }
        else
        {
            fprintf(stderr, "unknown player type\n");
        }
    } while(!allow_check && action == Action::CHECK);

    // assert(action != Action::INVALID);
    debug_print(GREEN "%s\n" RESET, action_to_str.at(action).c_str());
    return action;
}


// Hand Player::get_private_cards() const
// {
//     return this->private_cards;
// }


// Hand Player::get_community_cards() const
// {
//     return this->community_cards;
// }


// Hand get_usable_cards() const
// {
//     auto usable_cards = this->private_cards;
//     usable_cards.insert(usable_cards.end(), this->community_cards.begin(), this->community_cards.end());
//     return usable_cards;
// }


// int Player::get_balance() const
// {
//     return m_balance;
// }

/**
 * The functions record all the history action of a player by copying the round action.
 *
 * @param  none
 * @return void 
 */
// void Player::update_his_action()
// {
//     m_his_action.insert(m_his_action.end(), m_round_action.begin(), m_round_action.end());
// }

/**
 * The functions record the action of a player in the current round
 * will be cleared when entering the next round
 *
 * @param  Action
 * @return void 
 */
// void Player::update_round_action(Action a)
// {
//     m_round_action.push_back(a);
// }

/**
 * The functions returns the last action of a player in the current round or INVALID if the player hasn't played this round
 *
 * @param  none
 * @return void 
 */
// Action Player::get_last_action_in_round() const
// {
//     if (m_round_action.size() == 0)
//     {
//         // fprintf(stderr, "Error! Getting last action on a empty round history\n");
//         return Action::INVALID;
//     }
//     return m_round_action.back();
// }


/**
 * The functions sorts the hand and display the cards in a more readable form
 *
 * @param  none
 * @return void 
 */
void Player::display_private_cards()
{
    // first sort the hand
    std::sort(this->private_cards.begin(), this->private_cards.end());
    std::cout << "------------------------------------------------\n";
    std::cout << this->player_name << " Private cards:\n";
    UTILS::display_hand(this->private_cards);
    std::cout << "\n";
}

// void Player::display_usable_cards()
// {
//     this->display_private_cards();
//     std::cout << "\n-------------------------------\n";
//     std::cout << this->player_name << " Community cards\n";
//     UTILS::display_hand(this->community_cards);
//     std::cout << "\n";
// }

