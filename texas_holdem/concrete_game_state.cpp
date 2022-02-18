#include "concrete_game_state.h"
#include <iostream>

/****************** Pre flop round ******************/
// Ask the user for an Action
// check if it's legal
// transition to next state, depend on input
// increase the pot size
// mark which player folds
void Pre_flop::process(Game* game)
{   
    game->shuffle_and_deal();
    Action a;

    // P0 commit 1 dollar, P1 commit 2 dollar
    game->m_players[0].m_balance -= 1.0f;
    game->m_players[1].m_balance -= 2.0f;

    game->m_pot = 3.0f;

    for (unsigned i = 0; i < game->m_num_player; i ++)
    {
        Player& current_player = game->m_players[i];
        // assign cards[0, 1] to player 0's hand
        // assign cards[2, 3] to player 1's hand
        Hand player_hand = {game->m_cards[2 * i], game->m_cards[2 * i + 1]};
        current_player.private_cards.assign(player_hand.begin(), player_hand.end());
        if (DEBUG)
        {
            current_player.display_private_cards();
        }

        // don't allow check
        a = current_player.get_action(false);
        // add the action to all player's action this round
        game->add_to_action_this_round(a);

        // if one player decides to fold, immediately go to terminal state
        if (a == Action::FOLD)
        {
            game->m_loss_player_idx = i;
            game->transition(Terminal::get_instance());
            return;
        }
        else
        {
            current_player.m_balance -= 1.0f;
            game->m_pot += 1.0f;
        }
    }
    // if all player bets, go to the next round
    game->transition(Flop::get_instance());
}

void Pre_flop::enter(Game* game)
{
    debug_print("%s", "***************************************************************\n");
    debug_print(GREEN "Entering Pre Flop Round:\tpot=%f\n" RESET, game->m_pot);
    debug_print(YELLOW "%s's balance=%f\n" RESET, game->m_players[0].player_name.c_str(), game->m_players[0].m_balance);
    debug_print(YELLOW "%s's balance=%f\n" RESET, game->m_players[1].player_name.c_str(), game->m_players[1].m_balance);
}

void Pre_flop::exit(Game* game)
{
    debug_print(CYAN "Exiting Pre Flop Round\tpot=%f\n\n" RESET, game->m_pot);
    // update history action
    game->update_his_action();
    // clear this round action
    game->clear_action_this_round();
}

Game_state& Pre_flop::get_instance()
{
    static Pre_flop singleton; 
    return singleton;
}


/****************** Flop round ********************/
void Flop::process(Game* game)
{
    Action a;
    unsigned active_player_idx = 0;
    while (!game->round_finished())
    {
        Player& current_player = game->m_players[active_player_idx];
        if (DEBUG)
        {
            current_player.display_private_cards();
        }
        
        // keep promping input from user if input is check and check is not allowed
        a = current_player.get_action(check_allowed);
        game->add_to_action_this_round(a);

        if (a == Action::FOLD)
        {
            game->m_loss_player_idx = active_player_idx;
            game->transition(Terminal::get_instance());
            return;
        }
        else if (a == Action::BET)
        {
            current_player.m_balance -= 1.0f;
            check_allowed = false;
            game->m_pot += 1.0f;
            // if player 1 bets, and player 0 check, we need to ensure the next player is player 0 if the round does not terminate
            if (active_player_idx == 1)
            {
                active_player_idx ^= 1;
                continue;
            }
        }
        active_player_idx ++;
    }
    // go to the next round
    game->transition(Turn::get_instance());
}

void Flop::enter(Game* game)
{
    // reset the flag because it's a new round
    check_allowed = true;
    debug_print("%s", "***************************************************************\n");
    debug_print(GREEN "Entering Flop Round:\tpot=%f\n" RESET, game->m_pot);
    debug_print(YELLOW "%s's balance=%f\n" RESET, game->m_players[0].player_name.c_str(), game->m_players[0].m_balance);
    debug_print(YELLOW "%s's balance=%f\n" RESET, game->m_players[1].player_name.c_str(), game->m_players[1].m_balance);

    // add 3 more community card to player's hand, ie cards[4, 5, 6]
    game->add_to_community_cards( {game->m_cards[4], game->m_cards[5], game->m_cards[6]} );
    if (DEBUG) game->display_community_cards();
}

void Flop::exit(Game* game)
{
    debug_print(CYAN "Exiting Flop Round\tpot=%f\n\n" RESET, game->m_pot);
    game->update_his_action();
    game->clear_action_this_round();
}

Game_state& Flop::get_instance()
{
    static Flop singleton;
    return singleton;
}


/****************** Turn round ********************/
void Turn::process(Game* game)
{
    Action a;
    unsigned active_player_idx = 0;
    while (!game->round_finished())
    {
        Player& current_player = game->m_players[active_player_idx];

        if (DEBUG)
        {
            current_player.display_private_cards();
        }
        // keep promping input from user if input is check and check is not allowed
        a = current_player.get_action(check_allowed);
        game->add_to_action_this_round(a);
        
        if (a == Action::FOLD)
        {
            game->m_loss_player_idx = active_player_idx;
            game->transition(Terminal::get_instance());
            return;
        }
        else if (a == Action::BET)
        {
            current_player.m_balance -= 1.0f;
            check_allowed = false;
            game->m_pot += 1.0f;
            // if player 1 bets, and player 0 check, we need to ensure the next player is player 0 if the round does not terminate
            if (active_player_idx == 1)
            {
                active_player_idx ^= 1;
                continue;
            }
        }
        active_player_idx ++;
    }
    // go to the next round
    game->transition(River::get_instance());
}

void Turn::enter(Game* game)
{
    // reset the flag because it's a new round
    check_allowed = true;
    debug_print("%s", "***************************************************************\n");
    debug_print(GREEN "Entering Turn Round:\tpot=%f\n\n" RESET, game->m_pot);
    debug_print(YELLOW "%s's balance=%f\n" RESET, game->m_players[0].player_name.c_str(), game->m_players[0].m_balance);
    debug_print(YELLOW "%s's balance=%f\n" RESET, game->m_players[1].player_name.c_str(), game->m_players[1].m_balance);

    // add 1 more community card to player's hand, ie cards[7]
    game->add_to_community_cards( {game->m_cards[7]} );
    if (DEBUG) game->display_community_cards();
}

void Turn::exit(Game* game)
{
    debug_print(CYAN "Exiting Turn Round\tpot=%f\n" RESET, game->m_pot);
    game->update_his_action();
    game->clear_action_this_round();
}

Game_state& Turn::get_instance()
{
    static Turn singleton;
    return singleton;
}


/****************** River round *********************/
void River::process(Game* game)
{
    Action a;
    unsigned active_player_idx = 0;
    while (!game->round_finished())
    {
        Player& current_player = game->m_players[active_player_idx];
        if (DEBUG)
        {
            current_player.display_private_cards();
        }
        // keep promping input from user if input is check and check is not allowed
        a = current_player.get_action(check_allowed);
        game->add_to_action_this_round(a);
        
        if (a == Action::FOLD)
        {
            game->m_loss_player_idx = active_player_idx;
            game->transition(Terminal::get_instance());
            return;
        }
        else if (a == Action::BET)
        {
            current_player.m_balance -= 1.0f;
            check_allowed = false;
            game->m_pot += 1.0f;
            // if player 1 bets, and player 0 check, we need to ensure the next player is player 0 if the round does not terminate
            if (active_player_idx == 1)
            {
                active_player_idx ^= 1;
                continue;
            }
        }
        active_player_idx ++;
    }
    // go to the next round
    game->transition(Showdown::get_instance());
}

void River::enter(Game* game)
{
    // reset the flag because it's a new round
    check_allowed = true;
    debug_print("%s", "***************************************************************\n");
    debug_print(GREEN "Entering River Round:\tpot=%f\n\n" RESET, game->m_pot);
    debug_print(YELLOW "%s's balance=%f\n" RESET, game->m_players[0].player_name.c_str(), game->m_players[0].m_balance);
    debug_print(YELLOW "%s's balance=%f\n" RESET, game->m_players[1].player_name.c_str(), game->m_players[1].m_balance);

    // show last community card, add to player's hand
    game->add_to_community_cards( {game->m_cards[8]} );
    if (DEBUG) game->display_community_cards();
}

void River::exit(Game* game)
{
    debug_print(CYAN "Exiting River Round\tpot=%f\n", game->m_pot);
    game->update_his_action();
    game->clear_action_this_round();
}

Game_state& River::get_instance()
{
    static River singleton;
    return singleton;
}


/****************** Showdown round ********************/
void Showdown::process(Game* game)
{
    // evaluate the hand of each player and determine who won

    Hand player_0_usable_cards, player_1_usable_cards;
    player_0_usable_cards = game->m_players[0].private_cards;
    player_0_usable_cards.insert(player_0_usable_cards.end(), game->m_players[0].community_cards.begin(), game->m_players[0].community_cards.end());
    
    player_1_usable_cards = game->m_players[1].private_cards;
    player_1_usable_cards.insert(player_1_usable_cards.end(), game->m_players[1].community_cards.begin(), game->m_players[1].community_cards.end());

    uint16_t player_0_score = game->get_hand_ranking(player_0_usable_cards);
    uint16_t player_1_score = game->get_hand_ranking(player_1_usable_cards);

    if (player_0_score > player_1_score)
    {
        debug_print(RED "%s won the game, winning $%f\n" RESET, game->m_players[0].player_name.c_str(), game->m_pot);
        game->m_loss_player_idx = 1;
    }
    else if (player_0_score < player_1_score)
    {
        debug_print(RED "%s won the game, winning $%f\n" RESET, game->m_players[1].player_name.c_str(), game->m_pot);
        game->m_loss_player_idx = 0;
    }
    else
    {
        // keep loss_player_idx as -1 to indicate a tie round
        debug_print(RED "%s" RESET, "Game is tie\n");
    }
    game->transition(Terminal::get_instance());
}

void Showdown::enter(Game* game)
{
    debug_print("%s", "***************************************************************\n");
    debug_print(GREEN "Entering Showdown Round:\tpot=%f\n\n" RESET, game->m_pot);
    debug_print(YELLOW "%s's balance=%f\n" RESET, game->m_players[0].player_name.c_str(), game->m_players[0].m_balance);
    debug_print(YELLOW "%s's balance=%f\n" RESET, game->m_players[1].player_name.c_str(), game->m_players[1].m_balance);
}

void Showdown::exit(Game* game)
{
    debug_print(CYAN "Exiting Showdown Round\tpot=%f\n\n" RESET, game->m_pot);
}


Game_state& Showdown::get_instance()
{
    static Showdown singleton;
    return singleton;
}


/****************** Terminal state ********************/
void Terminal::process(Game* game)
{
    // clear pot
    // clear loss_player_index
    // clear hand
    // update player balance...
    // clear player action history

    // std::cout << game->m_players[winning_palyer_idx].player_name << " won the game, winning $" << game->m_pot << std::endl;

    game->display_round_result();

    game->m_pot = 0;

    game->m_loss_player_idx = -1;

    game->m_round_count ++;

    game->clear_his_action();

    game->clear_player_cards();

    game->m_community_cards.clear();

    game->m_cards.clear();

    // continue if we haven't reached the round limit
    if (game->m_round_count < game->m_iteration)
    {
        game->transition(Pre_flop::get_instance());
    }
}

void Terminal::enter(Game* game)
{
    if (game->m_loss_player_idx == -1)
    {
        game->m_players[0].m_balance += game->m_pot / 2;
        game->m_players[1].m_balance += game->m_pot / 2;
    }
    else
    {
        unsigned winning_palyer_idx = game->m_loss_player_idx ^ 1;
        game->m_players[winning_palyer_idx].m_balance += game->m_pot;
        game->winning_records[winning_palyer_idx] ++;
    }
    std::cout << "Round: " << game->m_round_count << ", " << game->m_players[0].player_name << "'s balance=" 
        << game->m_players[0].m_balance << ", " << game->m_players[1].player_name << "'s balance=" << game->m_players[1].m_balance << std::endl;

    debug_print("%s", "***************************************************************\n");
    debug_print(GREEN "Entering Terminal Round:\tpot=%f\n\n" RESET, game->m_pot);
    debug_print(YELLOW "%s's balance=%f\n" RESET, game->m_players[0].player_name.c_str(), game->m_players[0].m_balance);
    debug_print(YELLOW "%s's balance=%f\n" RESET, game->m_players[1].player_name.c_str(), game->m_players[1].m_balance);
}

void Terminal::exit(Game* game)
{
    debug_print(CYAN "Exiting Terminal Round\tpot=%f\n\n" RESET, game->m_pot);
}

Game_state& Terminal::get_instance()
{
    static Terminal singleton;
    return singleton;
}
