#include "game.h"

/**
 * the constructor for Game object
 *
 * @param  number of players
 * @return void
 */
Game::Game (int num_players) 
{
    this->num_players = num_players;
    for (int i = 0; i < num_players; i ++)
    {
        Player player ("Player " + std::to_string(i + 1));
        this->players.push_back(player);
    }
}


/**
 * the main loop to play the game between players, until user aborts
 *
 * @param  None
 * @return void
 */
void Game::play ()
{
    Player & player_1 = players[0];
    Player & player_2 = players[1];

    while (true)
    {
        moves move_1 = player_1.play_a_round();
        moves move_2 = player_2.play_a_round();

        // result is in the perspective of player_1
        round_result result_1 = find_result(move_1, move_2);

        // this is ugly, change it!
        round_result result_2 = result_1 == round_result::TIE ? 
                                            round_result::TIE : result_1 == round_result::WIN ? 
                                                                            round_result::LOSE : round_result::WIN;

        // upate the gamestate
        // ie update the player historical results + historical moves
        player_1.update_move(move_1);
        player_1.update_result(result_1);

        player_2.update_move(move_2);
        player_2.update_result(result_2);

        // display results
        this->display_game_board();
    }
}


/**
 * Determine which move wins
 *
 * @param move_1,2 are assumed to be valid
 * @return round_result enum, indicating the results of move_1 VS move_2, in the prespective of move_1 
 * 
 * TODO: move this somewhere else? instead of declaring it static
 * use utility table to determine who wins
 */
round_result Game::find_result(moves move_1, moves move_2) const
{
    if (move_1 == move_2)
        return round_result::TIE;

    int16_t move_1_index = static_cast<int16_t>(move_1);
    int16_t move_2_index = static_cast<int16_t>(move_2);

    if (move_1_index - move_2_index == 1 || move_1_index - move_2_index == -2)
    {
        return round_result::WIN;
    }
    return round_result::LOSE;
}


/**
 * Display the latest moves of each player + show who wins and how many points they get
 * in ASCII
 *
 * @param None
 * @return void
 */
void Game::display_game_board () const
{
    for (const auto & player : players)
    {
        char game_board[200];
        snprintf(game_board, sizeof(game_board), "%s played:", player.player_name.c_str());
        moves last_move = player.get_last_move();
        round_result last_result = player.get_last_round_result();

        printf ("%s", game_board);
        memset (game_board, 0, sizeof(game_board));
        
        // display last move
        switch (last_move)
        {
            case moves::ROCK:
                snprintf(game_board, sizeof(game_board), "%s", art::ROCK_SIGN.c_str());
                break;
            
            case moves::PAPER:
                snprintf(game_board, sizeof(game_board), "%s", art::PAPRT_SIGN.c_str());
                break;

            case moves::SCISSOR:
                snprintf(game_board, sizeof(game_board), "%s", art::SCISSOR_SIGN.c_str());
                break;

            default:
                throw std::runtime_error ("trying to print invalid move, terminating the game");
                exit (-1);
                break;
        }
        printf ("%s", game_board);
        memset (game_board, 0, sizeof(game_board));

        // dispaly last round result
        switch (last_result)
        {
            case round_result::WIN:
            snprintf (game_board, sizeof(game_board), "\n%s", (GREEN "WIN\n" RESET));
            break;

            case round_result::TIE:
            snprintf (game_board, sizeof(game_board), "\n%s", (YELLOW "TIE\n" RESET));
            break;

            case round_result::LOSE:
            snprintf (game_board, sizeof(game_board), "\n%s", (RED "LOSE\n" RESET));
            break;
        }
        printf("%s\n", game_board);
        memset (game_board, 0, sizeof(game_board));
    }
}
