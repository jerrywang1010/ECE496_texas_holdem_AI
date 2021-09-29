#include "game.h"
#include <assert.h>

/**
 * the constructor for self play game mode
 *
 * @param  number_ofplayers
 * @param  GAME_MODE
 * @param  iteration
 * @return void
 */
Game::Game (int num_players, GAME_MODE mode, int num_iteration)
{
    assert (mode == GAME_MODE::SELF_PLAY);
    this->num_players = num_players;
    this->mode = mode;
    this->num_iteration = num_iteration;
    for (int i = 0; i < num_players; i ++)
    {
        RPSTrainer* trainer = new RPSTrainer();
        Player player ("Player" + std::to_string(i + 1), trainer);
        this->players.push_back(player);
    }
}


/**
 * the constructor for 1 vs 1 game mode
 *
 * assume player 1 is the user controled player, player 2 is the trainer bot
 * only support 2 players
 * 
 * @param  GAME_MODE
 * @return void
 * 
 */
Game::Game (GAME_MODE mode, int num_iteration)
{
    assert (mode == GAME_MODE::ONE_VS_ONE);
    this->num_players = 2;
    this->mode = mode;
    this->num_iteration = num_iteration;

    std::string user_name;
    std::cout << "Plase enter a user name: " << "\n";
    std::cin >> user_name; 

    Player player1 (user_name, nullptr);
    RPSTrainer* trainer = new RPSTrainer();
    Player trainer_bot ("Trainer bot", trainer);
    auto new_players = {player1, trainer_bot};
    this->players.insert(this->players.begin(), new_players.begin(), new_players.end());
}


/**
 * the constructor for set opponent's strategy game mode
 *
 * opponent's strategy is hard coded
 * trainer bot playes againt the set strategy to reach an opptimal strategy
 * assume player 1 is the trainer bot, player 2 is the set strategy
 * 
 * @param  GAME_MODE
 * @param  oppenent_strategy
 * @param  iteration
 * @return void
 * 
 */
Game::Game (GAME_MODE mode, std::vector<double> opp_strategy, int num_iteration)
{
    assert (opp_strategy.size() == 3 && mode == GAME_MODE::FIXED_OPP_STRAT);
    this->opp_strategy = opp_strategy;
    this->num_players = 2;
    this->mode = mode;
    this->num_iteration = num_iteration;

    RPSTrainer* trainer = new RPSTrainer();
    Player trainer_bot ("Trainer bot", trainer);

    RPSTrainer* set_strat_trainer = new RPSTrainer(opp_strategy);
    Player opponent ("Opponent", set_strat_trainer);
    auto new_players = {trainer_bot, opponent};
    this->players.insert(this->players.begin(), new_players.begin(), new_players.end());
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

    for (int i = 0; i < num_iteration; i ++)
    {
        moves move_1 = moves::INVALID;
        moves move_2 = moves::INVALID;

        // I feel like game loop need a lot of change
        // TODO: make this prettier

        // get input from either real user or bot
        if (player_1.is_real_player())
        {
            move_1 = player_1.play_a_round();
        }
        else
        {
            move_1 = player_1.trainer_play_a_round();
        }

        if (player_2.is_real_player())
        {
            move_2 = player_2.play_a_round();
        }
        else
        {
            move_2 = player_2.trainer_play_a_round();
        }

        // result is in the perspective of player_1
        round_result result_1 = find_result(move_1, move_2);
        round_result result_2 = find_result(move_2, move_1);

        // if any player is a bot, update their regret
        if (!player_1.is_real_player())
        {
            player_1.train_iter(move_1, move_2);
        }
        if (!player_2.is_real_player())
        {
            player_2.train_iter(move_2, move_1);
        }


        // upate the gamestate
        // ie update the player historical results + historical moves
        player_1.update_move(move_1);
        player_1.update_result(result_1);

        player_2.update_move(move_2);
        player_2.update_result(result_2);

        // display results
        if (this->mode != GAME_MODE::SELF_PLAY && this->mode != GAME_MODE::FIXED_OPP_STRAT)
        {
            this->display_game_board();
        }
    }
}


/**
 * Determine which move wins
 *
 * @param move_1,2 are assumed to be valid
 * @return round_result enum, indicating the results of move_1 VS move_2, in the prespective of move_1 
 * 
 */
round_result Game::find_result(moves move_1, moves move_2) const
{   
    int utility = utility_table[static_cast<int16_t> (move_2)][static_cast<int16_t> (move_1)];
    return utility == 1 ? 
           round_result::WIN : utility == -1 ? 
                               round_result::LOSE : round_result::TIE;
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


/**
 * Display the final strategy of each player, only applies to bot player
 *
 * @param None
 * @return void
 */
void Game::display_player_strategies () const
{
    for (const auto & player : this->players)
    {
        player.display_strategy();
    }
}
