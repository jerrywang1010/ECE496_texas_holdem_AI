#include "player.h"
#include "assert.h"
#include <algorithm>

/**
 * The function updates the balance of a player, if he wins or losses money
 *
 * @param  int
 * @return void 
 */
void Player::update_balanace(int money)
{
    this->m_balance += money;
}

/**
 * The function adds card to a player's hand, representing avaliable cards that he could use
 *
 * @param  Hand
 * @return void 
 */
void Player::add_to_hand(Hand h)
{
    assert(h.size() == 2);
    this->m_hand.insert(m_hand.end(), h.begin(), h.end());
}

/**
 * The function adds a hand to the players usable cards
 *
 * @param  hand
 * @return void 
 */
void Player::add_to_usable_cards(Hand h)
{
    assert(h.size() <= 3);
    this->m_usable_cards.insert(m_usable_cards.end(), h.begin(), h.end());
}


/**
 * The function clears the hand of a player
 *
 * @param void
 * @return void 
 */
void Player::clear_hand()
{
    this->m_hand.clear();
}

/**
 * The functions clears the player's usable cards, usually happends at the end of a game.
 *
 * @param  hand
 * @return void 
 */
void Player::clear_usable_cards()
{
    this->m_usable_cards.clear();
}


/**
 * The function sets balance of a player
 * This function should only be called at the beginning of a game
 * Balance change in each round should be done with update_balance()
 *
 * @param  int
 * @return void 
 */
void Player::set_balance(int balance)
{
    this->m_balance = balance;
}

/**
 * The function gets an action from a player, prompt the user to input an action and check if it is valid. Returns a legal action
 *
 * @param  check_allowed
 * @return Action 
 */
Action Player::get_action(bool allow_check) const
{
    char _a;
    Action action = Action::INVALID;
    do
    {   
        if (allow_check)
        {
            std::cout << "Player: " << player_name << "\n\tPlease enter F (Fold), C (Check) or B (Bet)\n";
        }
        else
        {
            std::cout << "Player: " << player_name << "\n\tPlease enter F (Fold) or B (Bet)\n";
        }
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
                    throw std::runtime_error ("user entered invalid action, terminating the game");
                    exit (-1);
                    break;
            }
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    } while(!allow_check && action == Action::CHECK);

    return action;
}


Hand Player::get_hand() const
{
    return m_hand;
}


Hand Player::get_usable_cards() const
{
    return m_usable_cards;
}


int Player::get_balance() const
{
    return m_balance;
}

/**
 * The functions record all the history action of a player by copying the round action.
 *
 * @param  none
 * @return void 
 */
void Player::update_his_action()
{
    m_his_action.insert(m_his_action.end(), m_round_action.begin(), m_round_action.end());
}

/**
 * The functions record the action of a player in the current round
 * will be cleared when entering the next round
 *
 * @param  Action
 * @return void 
 */
void Player::update_round_action(Action a)
{
    m_round_action.push_back(a);
}

/**
 * The functions returns the last action of a player in the current round or INVALID if the player hasn't played this round
 *
 * @param  none
 * @return void 
 */
Action Player::get_last_action_in_round() const
{
    if (m_round_action.size() == 0)
    {
        // fprintf(stderr, "Error! Getting last action on a empty round history\n");
        return Action::INVALID;
    }
    return m_round_action.back();
}

/**
 * The functions sorts the hand and display the cards in a more readable form
 *
 * @param  none
 * @return void 
 */
void Player::display_hand()
{
    // first sort the hand
    std::sort(m_hand.begin(), m_hand.end());
    std::cout << "------------------------------------------------\n";
    std::cout << this->player_name << " hand:\n";
    UTILS::display_hand(m_hand);
    std::cout << "\n";
}

void Player::display_usable_cards()
{
    std::sort(m_usable_cards.begin(), m_usable_cards.end());
    std::cout << "------------------------------------------------\n";
    std::cout << this->player_name << " usable cards\n";
    UTILS::display_hand(m_usable_cards);
    std::cout << "\n";
}

