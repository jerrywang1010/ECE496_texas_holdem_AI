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
    this->m_hand.insert(m_hand.end(), h.begin(), h.end());
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
 * The function gets an action from a player, currently prompt the user to input an action
 *
 * @param  void
 * @return Action 
 */
Action Player::get_action(bool allow_check) const
{
    char _a;
    Action action = Action::INVALID;
    do
    {
        std::cout << "Player: " << player_name << "\n\tPlease enter F (Fold), C (Check) or B (Bet)\n";
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


int Player::get_balance() const
{
    return m_balance;
}


// add round action history to game action history
void Player::update_his_action()
{
    m_his_action.insert(m_his_action.end(), m_round_action.begin(), m_round_action.end());
}

void Player::update_round_action(Action a)
{
    // std::cout << "update round action:" << static_cast<uint16_t>(a) << std::endl;
    m_round_action.push_back(a);
}


Action Player::get_last_action_in_round() const
{
    if (m_round_action.size() == 0)
    {
        // fprintf(stderr, "Error! Getting last action on a empty round history\n");
        return Action::INVALID;
    }
    return m_round_action.back();
}


void print_card(Card card)
{
    unsigned rank = card / 4 + 2;
    if (rank > 13) 
    {
        rank -= 13;
    }
    std::string suit = suits.at(card % 4);
    
    std::cout << "rank=" << rank << " ,suit=" << suit << std::endl;
}


void Player::display_hand()
{
    // first sort the hand
    std::sort(m_hand.begin(), m_hand.end());
    std::cout << this->player_name << " hand:\n";
    for (Card card : m_hand)
    {
        print_card(card);
    }
}