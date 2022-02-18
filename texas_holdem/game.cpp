#include "game.h"
#include "concrete_game_state.h"
#include <random>
#include <algorithm>
#include <time.h>
#include <chrono>

const static std::unordered_map<unsigned, std::string> hand_ranking({{1, "High card"}, {2, "Pair"}, {3, "Two pair"}, {4, "Three of a kind"}, {5, "Straight"}, {6, "Flush"}, {7, "Full house"}, {8, "Four of a kind"}, {9, "Straight flush"}});

Game::Game()
{
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
    debug_print(MAGENTA "Hand ranking=%s\n" RESET, hand_ranking.at(score / 4096).c_str());
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
    // Hand deck = {1,5,9,13,17,21,25,29,33,37};
    Hand deck = {1,5,9,13,17,21,25,29,33,37,41,45,49};
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::shuffle(std::begin(deck), std::end(deck), generator);
    m_cards = deck;
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
