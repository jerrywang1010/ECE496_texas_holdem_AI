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
    unsigned active_player_idx = 0;

    for (unsigned i = 0; i < game->m_num_player; i ++)
    {
        Player& current_player = game->m_players[i];
        // assign cards[0, 1] to player 0's hand
        // assign cards[2, 3] to player 1's hand
        Hand player_hand = {game->m_cards[2 * i], game->m_cards[2 * i + 1]};
        current_player.add_to_hand(player_hand);
        current_player.add_to_usable_cards(player_hand);
        current_player.display_hand();

        // don't allow check
        a = current_player.get_action(false);
        current_player.update_round_action(a);
        // if one player decides to fold, immediately go to terminal state
        if (a == Action::FOLD)
        {
            game->m_loss_player_idx = i;
            game->transition(Terminal::get_instance());
            return;
        }
        else
        {
            current_player.update_balanace(-1);
            game->m_pot += 1;
        }
    }
    // if all player bets, go to the next round
    game->transition(Flop::get_instance());
}

void Pre_flop::enter(Game* game)
{
    std::cout << "***************************************************************" << std::endl;
    std::cout << GREEN << "Entering Pre Flop Round:\t" << "pot=" << game->m_pot << RESET << std::endl;
    std::cout << YELLOW << game->m_players[0].player_name << "'s balance=" << game->m_players[0].get_balance() << RESET << std::endl;
    std::cout << YELLOW << game->m_players[1].player_name << "'s balance=" << game->m_players[1].get_balance() << RESET << std::endl;
}

void Pre_flop::exit(Game* game)
{
    std::cout << CYAN << "Exiting Pre Flop Round\t" << "pot=" << game->m_pot << RESET << std::endl << std::endl;
    // update history action
    game->update_his_action();
    // clear this round action
    game->clear_round_action();
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
        current_player.display_hand();
        
        // keep promping input from user if input is check and check is not allowed
        a = current_player.get_action(check_allowed);
        current_player.update_round_action(a);

        if (a == Action::FOLD)
        {
            game->m_loss_player_idx = active_player_idx;
            game->transition(Terminal::get_instance());
            return;
        }
        else if (a == Action::BET)
        {
            current_player.update_balanace(-1);
            check_allowed = false;
            game->m_pot += 1;
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
    std::cout << "***************************************************************" << std::endl;
    std::cout << GREEN << "Entering Flop Round:\t" << "pot=" << game->m_pot << RESET << std::endl;
    std::cout << YELLOW << game->m_players[0].player_name << "'s balance=" << game->m_players[0].get_balance() << RESET << std::endl;
    std::cout << YELLOW << game->m_players[1].player_name << "'s balance=" << game->m_players[1].get_balance() << RESET << std::endl;

    // add 3 more community card to player's hand, ie cards[4, 5, 6]
    for (auto& player : game->m_players)
    {
        player.add_to_usable_cards( {game->m_cards[4], game->m_cards[5], game->m_cards[6]} );
    }
    game->add_to_community_cards( {game->m_cards[4], game->m_cards[5], game->m_cards[6]} );
    game->display_community_cards();
}

void Flop::exit(Game* game)
{
    std::cout << CYAN << "Exiting Flop Round\t" << "pot=" << game->m_pot << RESET << std::endl << std::endl;
    game->update_his_action();
    game->clear_round_action();
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

        current_player.display_hand();
        // keep promping input from user if input is check and check is not allowed
        a = current_player.get_action(check_allowed);
        current_player.update_round_action(a);
        
        if (a == Action::FOLD)
        {
            game->m_loss_player_idx = active_player_idx;
            game->transition(Terminal::get_instance());
            return;
        }
        else if (a == Action::BET)
        {
            current_player.update_balanace(-1);
            check_allowed = false;
            game->m_pot += 1;
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
    std::cout << "***************************************************************" << std::endl;
    std::cout << GREEN << "Entering Turn Round:\t" << "pot=" << game->m_pot << RESET << std::endl << std::endl;
    std::cout << YELLOW << game->m_players[0].player_name << "'s balance=" << game->m_players[0].get_balance() << RESET << std::endl;
    std::cout << YELLOW << game->m_players[1].player_name << "'s balance=" << game->m_players[1].get_balance() << RESET << std::endl;

    // add 1 more community card to player's hand, ie cards[7]
    for (auto& player : game->m_players)
    {
        player.add_to_usable_cards( {game->m_cards[7]} );
    }
    game->add_to_community_cards( {game->m_cards[7]} );
    game->display_community_cards();
}

void Turn::exit(Game* game)
{
    std::cout << CYAN << "Exiting Turn Round\t" << "pot=" << game->m_pot << RESET << std::endl;
    game->update_his_action();
    game->clear_round_action();
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
        current_player.display_hand();
        // keep promping input from user if input is check and check is not allowed
        a = current_player.get_action(check_allowed);
        current_player.update_round_action(a);
        
        if (a == Action::FOLD)
        {
            game->m_loss_player_idx = active_player_idx;
            game->transition(Terminal::get_instance());
            return;
        }
        else if (a == Action::BET)
        {
            current_player.update_balanace(-1);
            check_allowed = false;
            game->m_pot += 1;
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
    std::cout << "***************************************************************" << std::endl;
    std::cout << GREEN << "Entering River Round:\t" << "pot=" << game->m_pot << RESET << std::endl << std::endl;
    std::cout << YELLOW << game->m_players[0].player_name << "'s balance=" << game->m_players[0].get_balance() << RESET << std::endl;
    std::cout << YELLOW << game->m_players[1].player_name << "'s balance=" << game->m_players[1].get_balance() << RESET << std::endl;

    // show last community card, add to player's hand
    for (auto& player : game->m_players)
    {
        player.add_to_usable_cards( {game->m_cards[8]} );
    }
    game->add_to_community_cards( {game->m_cards[8]} );
    game->display_community_cards();
}

void River::exit(Game* game)
{
    std::cout << CYAN << "Exiting River Round\t" << "pot=" << game->m_pot << RESET << std::endl;
    game->update_his_action();
    game->clear_round_action();
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

    uint16_t player_0_score = game->get_hand_ranking(game->m_players[0].get_usable_cards());
    uint16_t player_1_score = game->get_hand_ranking(game->m_players[1].get_usable_cards());

    if (player_0_score > player_1_score)
    {
        std::cout << RED << game->m_players[0].player_name << " won the game, winning $" << game->m_pot << RESET << std::endl;
        game->m_loss_player_idx = 1;
    }
    else if (player_0_score < player_1_score)
    {
        std::cout << RED << game->m_players[1].player_name << " won the game, winning $" << game->m_pot << RESET << std::endl;
        game->m_loss_player_idx = 0;
    }
    else
    {
        std::cout << RED << "Game is tie" << RESET << std::endl;
    }
    game->transition(Terminal::get_instance());
}

void Showdown::enter(Game* game)
{
    std::cout << "***************************************************************" << std::endl;
    std::cout << GREEN << "Entering Showdown Round:\t" << "pot=" << game->m_pot << RESET << std::endl;
    std::cout << YELLOW << game->m_players[0].player_name << "'s balance=" << game->m_players[0].get_balance() << RESET << std::endl;
    std::cout << YELLOW << game->m_players[1].player_name << "'s balance=" << game->m_players[1].get_balance() << RESET << std::endl;
}

void Showdown::exit(Game* game)
{
    std::cout << CYAN << "Exiting Showdown Round\t" << "pot=" << game->m_pot << RESET << std::endl << std::endl;
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
    unsigned winning_palyer_idx = game->m_loss_player_idx ^ 1;

    // std::cout << game->m_players[winning_palyer_idx].player_name << " won the game, winning $" << game->m_pot << std::endl;

    game->display_round_result();
    
    game->m_players[winning_palyer_idx].update_balanace(game->m_pot);

    game->m_pot = 0;

    game->m_loss_player_idx = -1;

    game->m_round_count ++;

    game->clear_his_action();

    game->clear_player_hand();

    game->clear_player_usable_cards();

    game->clear_community_cards();

    game->m_cards.clear();

    // continue if we haven't reached the round limit
    if (game->m_round_count < game->m_iteration)
    {
        game->transition(Pre_flop::get_instance());
    }
}

void Terminal::enter(Game* game)
{
    std::cout << "***************************************************************" << std::endl;
    std::cout << GREEN << "Entering Terminal Round:\t" << "pot=" << game->m_pot << RESET << std::endl;
    std::cout << YELLOW << game->m_players[0].player_name << "'s balance=" << game->m_players[0].get_balance() << RESET << std::endl;
    std::cout << YELLOW << game->m_players[1].player_name << "'s balance=" << game->m_players[1].get_balance() << RESET << std::endl;
}

void Terminal::exit(Game* game)
{
    std::cout << CYAN << "Exiting Terminal Round\t" << "pot=" << game->m_pot << RESET << std::endl << std::endl;
}

Game_state& Terminal::get_instance()
{
    static Terminal singleton;
    return singleton;
}
