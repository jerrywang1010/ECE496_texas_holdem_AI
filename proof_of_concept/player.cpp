#include "player.h"


/**
 * the function prompts the user to play a move, if move is invalid, will terminate the program
 *
 * @param  None
 * @return a valid move for this player for this term
 */
moves Player::play_a_round() const
{
    char _m;
    std::cout << "player: " << player_name << "\n\tPlease enter R (rock), P (paper) or S (scissor)\n";
    try
    {
        std::cin >> _m;
        switch (_m)
        {
        case 'R':
        case 'r':
            return moves::ROCK;
        
        case 'P':
        case 'p':
            return moves::PAPER;
        
        case 'S':
        case 's':
            return moves::SCISSOR;

        default:
            throw std::runtime_error ("user entered invalid move, terminating the game");
            exit (-1);
            break;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return moves::INVALID;
}


/**
 * Ask the trainer for each player to get next move based on current strategy
 * only make sense when player is a bot
 *
 * @param  None
 * @return void
 */
moves Player::trainer_play_a_round () const
{
    return this->trainer->get_move();
}


/**
 * After each round, ask the trainer to update strategy and regret
 * only make sense when player is a bot
 *
 * @param  2_moves
 * @return void
 */
void Player::train_iter (moves my_move, moves opp_move) const
{
    this->trainer->train_iter(my_move, opp_move);
}


/**
 * this function updates the historical (previous) moves vector for a player
 *
 * @param  None
 * @return void
 */
void Player::update_move (moves move)
{
    this->his_moves.push_back(move);
}


/**
 * this function updates the historical (previous) win/loss/tie vector for a player
 *
 * @param  None
 * @return void
 */
void Player::update_result (round_result result)
{
    this->his_result.push_back(result);
}


/**
 * this function returns the last move performed by a player
 *
 * @param  None
 * @return moves
 */
moves Player::get_last_move () const
{
    return this->his_moves.back();
}


/**
 * this function returns the last round result of a player resulting from playing get_last_move()
 *
 * @param  None
 * @return round_result
 */
round_result Player::get_last_round_result () const
{
    return this->his_result.back();
}


/**
 * this function returns if the player is a bot or not.
 *
 * @param  None
 * @return void
 */
bool Player::is_real_player () const
{
    return this->trainer == nullptr;
}


/**
 * this function prints final strategy for each non-bot player
 *
 * @param  None
 * @return void
 */
void Player::display_strategy () const
{
    if (!is_real_player())
    {
        this->trainer->display_strategy(this->trainer->get_avg_strategy());
    }
}
