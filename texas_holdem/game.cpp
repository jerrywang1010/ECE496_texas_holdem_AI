#include "game.h"
#include "concrete_game_state.h"
#include <random>
#include <algorithm>
#include <time.h>
#include <chrono>
#include <fstream>
#include <C:\boost\include\boost-1_77\boost\range\irange.hpp>
#include <C:\boost\include\boost-1_77\boost\range\algorithm_ext\push_back.hpp>


Game::Game(Hand deck)
{
    m_playing_deck = std::move(deck);
    m_state = &Pre_flop::get_instance();
    // m_players = {Player("CFR_bot", 10), Player("Me", 10)};
    m_loss_player_idx = -1;
    m_pot = 0;
    m_num_player = 2;
    m_round_count = 0;
    m_eval = omp::HandEvaluator();
    srand(time(nullptr));
}

/**
 * Transitions from the current state to the next state of the game state machine,
 * updates current state and calls state transition functions
 *
 * @param  new_state
 * @return void
 */
void Game::transition(Game_state &new_state)
{
    m_state->exit(this);
    m_state = &new_state;
    m_state->enter(this);
}

/**
 * Envoke the override process function of a game state object
 *
 * @param  none
 * @return void
 */
void Game::process_state()
{
    m_state->process(this);
}

/**
 * Run the game for a fixed number of iteration
 *
 * @param  max_iteration
 * @return void
 */
void Game::run(unsigned iteration)
{
    m_iteration = iteration;
    while (m_round_count < iteration)
    {
        process_state();
    }
}

/**
 * checks if the current round is finished
 * a round is finished if all player had a valid action
 *
 * @param  none
 * @return bool
 */
bool Game::round_finished() const
{
    std::vector<std::vector<Action> > round_ending_action_history 
    {
        {Action::FOLD},
        {Action::BET, Action::BET},
        {Action::BET, Action::FOLD},
        {Action::CHECK, Action::CHECK},
        {Action::CHECK, Action::FOLD},
        {Action::CHECK, Action::BET, Action::BET},
        {Action::CHECK, Action::BET, Action::FOLD}
        
    };
    assert(m_players[0].action_this_round == m_players[1].action_this_round);
    for (const auto & pattern : round_ending_action_history)
    {
        if (m_players[0].action_this_round == pattern) return true;
    }
    return false;
}

/**
 * clear the round actions of all players
 *
 * @param  none
 * @return void
 */
void Game::clear_action_this_round()
{
    for (auto &player : m_players)
    {
        player.action_this_round.clear();
    }
}


void Game::add_to_action_this_round(Action a)
{
    for (auto & player : m_players)
    {
        player.action_this_round.push_back(a);
    }
}


/**
 * clear the history actions of all players
 *
 * @param  none
 * @return void
 */
void Game::clear_his_action()
{
    for (auto &player : m_players)
    {
        player.action_history.clear();
    }
}

/**
 * record history actions of all players
 *
 * @param  none
 * @return void
 */
void Game::update_his_action()
{
    for (auto &player : m_players)
    {
        player.action_history.insert(player.action_history.end(), 
            player.action_this_round.begin(), player.action_this_round.end());
    }
}

/**
 * clear the usable cards of all players
 *
 * @param  none
 * @return void
 */
void Game::clear_player_cards()
{
    for (auto &player : m_players)
    {
        player.private_cards.clear();
        player.community_cards.clear();
    }
}

void Game::display_community_cards()
{
    std::sort(m_community_cards.begin(), m_community_cards.end());
    std::cout << "------------------------------------------------\n";
    std::cout << "Community cards:\n";

    UTILS::display_hand(m_community_cards);
    std::cout << "\n";
}

/**
 * add a hand to the community cards for the current game
 *
 * @param  Hand
 * @return void
 */
void Game::add_to_community_cards(Hand h)
{
    this->m_community_cards.insert(m_community_cards.end(), h.begin(), h.end());
    for (auto &player : m_players)
    {
        player.community_cards.insert(player.community_cards.end(), h.begin(), h.end());
    }
}


/**
 * The function calculates the relative ranking of a hand (vector of cards size=[5,7])
 * Return a 16 bit unsigned interger representing the ranking.
 * The returned value / 4096 gives the relative category of the hand. Ex 1 = Highcard, 2 = pair ... except for royal flush
 *
 * @param  hand
 * @return 16bit
 */
uint16_t Game::get_hand_ranking(Hand hand) const
{
    omp::Hand h = omp::Hand::empty();

    // first translate my representation of card to a int
    for (Card card : hand)
    {
        h += omp::Hand(card);
    }
    uint16_t score = m_eval.evaluate(h);
    debug_print(MAGENTA "Hand ranking=%s\n" RESET, UTILS::hand_ranking.at(score / 4096).c_str());
    return score;
}

/**
 * pick 9 cards from 0-51
 * card[0, 1] is assigned to player 0, card[2, 3] is assgined to player 1
 * card[4, 8] are community cards
 *
 * @param  none
 * @return void
 */
void Game::shuffle_and_deal()
{
    // std::random_device dev;
    // std::mt19937 rng(dev());
    // std::uniform_int_distribution<std::mt19937::result_type> dist(0, 51);
    // while (m_cards.size() < 9)
    // {
    //     // unsigned rand = dist(rng);
    //     unsigned random = rand() % 51;
    //     if (std::find(m_cards.begin(), m_cards.end(), random) == m_cards.end())
    //     {
    //         m_cards.push_back(random);
    //     }
    // }
    // Hand deck = {1,5,9,13,17,21,25,29,33};
    // Hand deck = {1,5,9,13,17,21,25,29,33,37,41,45,49};

    // play on 52 cards deck
    Hand deck = this->m_playing_deck;
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::shuffle(std::begin(deck), std::end(deck), generator);
    m_cards = deck;
    // std::cout << "deck:\n";
    // deck = Hand(deck.begin(), deck.begin() + 9);
    // UTILS::display_hand(deck);
}

// assume only 2 players
void Game::display_round_result() const
{
    if (m_loss_player_idx == 0)
    {
        debug_print("%s\n",
        R"(
            .______    __          ___   ____    ____  _______ .______          __     ____    __    ____  __  .__   __.      _______.
            |   _  \  |  |        /   \  \   \  /   / |   ____||   _  \        /_ |    \   \  /  \  /   / |  | |  \ |  |     /       |
            |  |_)  | |  |       /  ^  \  \   \/   /  |  |__   |  |_)  |        | |     \   \/    \/   /  |  | |   \|  |    |   (----`
            |   ___/  |  |      /  /_\  \  \_    _/   |   __|  |      /         | |      \            /   |  | |  . `  |     \   \
            |  |      |  `----./  _____  \   |  |     |  |____ |  |\  \----.    | |       \    /\    /    |  | |  |\   | .----)   |   
            | _|      |_______/__/     \__\  |__|     |_______|| _| `._____|    |_|        \__/  \__/     |__| |__| \__| |_______/    
        )");
    }
    else
    {
        debug_print("%s\n",
        R"(
            .______    __          ___   ____    ____  _______ .______           ___      ____    __    ____  __  .__   __.      _______.
            |   _  \  |  |        /   \  \   \  /   / |   ____||   _  \         / _ \     \   \  /  \  /   / |  | |  \ |  |     /       |
            |  |_)  | |  |       /  ^  \  \   \/   /  |  |__   |  |_)  |       | | | |     \   \/    \/   /  |  | |   \|  |    |   (----`
            |   ___/  |  |      /  /_\  \  \_    _/   |   __|  |      /        | | | |      \            /   |  | |  . `  |     \   \
            |  |      |  `----./  _____  \   |  |     |  |____ |  |\  \----.   | |_| |       \    /\    /    |  | |  |\   | .----)   |   
            | _|      |_______/__/     \__\  |__|     |_______|| _| `._____|    \___/         \__/  \__/     |__| |__| \__| |_______/    
        )");
    }
}


int get_active_player_based_on_action_history(std::string action_history, bool& allow_check)
{
    // round terminating condition
    // BB, CC, CBC
    if (action_history.length() == 0)
    {
        allow_check = false;
        return 0;
    } 
    if (action_history.length() == 1) 
    {
        allow_check = false;
        return 1;
    }

    while (action_history.length() >= 3)
    {
        if (action_history.substr(0, 2) == "BB")
        {
            action_history = action_history.substr(2, std::string::npos);
            continue;
        }
        else if (action_history.substr(0, 2) == "CC")
        {
            action_history = action_history.substr(2, std::string::npos);
            continue;
        }
        else if (action_history.substr(0, 3) == "CBB")
        {
            action_history = action_history.substr(3, std::string::npos);
            continue;
        }
    }

    // possible action_history = {}, "B", "C", "BB", "CC", "CB"
    assert(action_history.length() <= 2);
    if (action_history.length() == 0 || action_history.length() == 2)
    {
        if (action_history.find("B") != std::string::npos)
            allow_check = false;
        else
            allow_check = true;
        return 0;
    } 
    if (action_history.length() == 1) 
    {
        if (action_history.find("B") != std::string::npos)
            allow_check = false;
        else
            allow_check = true;
        return 1;
    }
    assert(false && "what is going on?\n");
    return 0;
}

template<typename T, typename A>
void write_vector_to_file(const std::vector<T, A>& v, std::ofstream& out)
{
    if (v.size() == 0)
    {
        out << "\"null\"";
        return;
    }
    out << "[";
    for (size_t i = 0; i < v.size() - 1; i ++)
    {
        out << v[i] << ", ";
    }
    out << v.back() << "]";
}


void Game::output_strategy_as_json(int num_games, std::string file_name)
{
    // number of decision points in a game of Texas Hold'em
    assert(all_possible_action_history.size() == 4);
    assert(this->m_players.size() == 1);

    std::ofstream json_file;
    json_file.open(file_name, std::ios::out);
    json_file << "{\n\t\"strategies\": [\n";
    for (int iter = 0; iter < num_games; iter ++)
    {
        
        shuffle_and_deal();
        Hand deck = Hand(this->m_cards.begin(), this->m_cards.begin() + 9);
        Hand P0_private_hand = Hand(this->m_cards.begin(), this->m_cards.begin() + 2);
        std::sort(P0_private_hand.begin(), P0_private_hand.end());
        Hand P1_private_hand = Hand(this->m_cards.begin() + 2, this->m_cards.begin() + 4);
        std::sort(P1_private_hand.begin(), P1_private_hand.end());

        std::cout << "P0_private_hand=";
        UTILS::display_hand(P0_private_hand);
        std::cout << "P1_private_hand=";
        UTILS::display_hand(P1_private_hand);

        Hand private_hand;
        Hand community_cards;
        for (int round_idx = 0; round_idx < 4; round_idx ++)
        {
            std::vector<std::string> action_history_in_round = all_possible_action_history[round_idx];
            // flop
            if (round_idx == 1)
            {
                community_cards = Hand(this->m_cards.begin() + 4, this->m_cards.begin() + 7);
                std::sort(community_cards.begin(), community_cards.end());
            }
            // turn
            else if (round_idx == 2)
            {
                community_cards = Hand(this->m_cards.begin() + 4, this->m_cards.begin() + 7);
                std::sort(community_cards.begin(), community_cards.end());
                community_cards.push_back(*(this->m_cards.begin() + 8));
            }
            // river
            else if (round_idx == 3)
            {
                community_cards = Hand(this->m_cards.begin() + 4, this->m_cards.begin() + 7);
                std::sort(community_cards.begin(), community_cards.end());
                community_cards.insert(community_cards.end(), this->m_cards.begin() + 8, this->m_cards.begin() + 10);
            }
            
            std::cout << "community_cards=";
            UTILS::display_hand(community_cards);

            for (size_t i = 0; i < action_history_in_round.size(); i ++)
            {
                std::string action_history = action_history_in_round[i];
                bool allow_check = false;
                int active_player_idx = get_active_player_based_on_action_history(action_history, allow_check);
                private_hand = active_player_idx == 0 ?  P0_private_hand : P1_private_hand;
                std::vector<float> strat = this->m_players[0].get_strategy(private_hand, community_cards, action_history, allow_check);

                json_file << "{\n";
                json_file << "\t\"deck\": ";
                write_vector_to_file<Card>(deck, json_file);
                json_file << ",\n";
                json_file << "\t\"private_hand\": ";
                write_vector_to_file<Card>(private_hand, json_file);
                json_file << ",\n";
                json_file << "\t\"community_cards\": ";
                write_vector_to_file<Card>(community_cards, json_file);
                json_file << ",\n";
                json_file << "\t\"action_history\": " << (action_history.empty() ? "\"null\"" : "\"" + action_history + "\"") << ",\n";
                json_file << "\t\"active_player_idx\": " << active_player_idx << ",\n";
                json_file << "\t\"strategy\": ";
                write_vector_to_file<float>(strat, json_file);
                json_file << "\n";
                json_file << "},\n";
            }
        }
    }
    json_file << "]\n}";
    json_file.close();
}


/*
void Game::print_round_action() const
{
    for (const auto &player : m_players)
    {
        std::cout << player.player_name << " last action in round is: ";
        Action last_round_action = player.get_last_action_in_round();
        std::cout << static_cast<uint16_t>(last_round_action) << std::endl;
        switch (last_round_action)
        {
        case Action::BET:
            std::cout << "BET\n";
            break;

        case Action::FOLD:
            std::cout << "FOLD\n";
            break;

        case Action::CHECK:
            std::cout << "CHECK\n";
            break;

        case Action::INVALID:
            std::cout << "INVALID\n";
            break;

        default:
            break;
        }
    }
}
*/
