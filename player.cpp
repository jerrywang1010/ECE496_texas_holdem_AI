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
