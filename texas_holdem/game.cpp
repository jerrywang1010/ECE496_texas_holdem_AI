#include "game.h"
#include "concrete_game_state.h"
#include <random>
#include <algorithm>
#include <time.h>

const static std::unordered_map<std::string, unsigned> suits_idx ( { {"spade", 0}, {"heart", 1}, {"club", 2}, {"diamond", 3} } );

const static std::unordered_map<unsigned, std::string> hand_ranking ( {
            {1, "High card"}, {2, "Pair"}, {3, "Two pair"}, 
            {4, "Three of a kind"}, {5, "Straight"}, {6, "Flush"}, 
            {7, "Full house"}, {8, "Four of a kind"}, {9, "Straight flush"} } );


Game::Game()
{
    m_state = &Pre_flop::get_instance();
    m_players = {Player("Player 0", 10), Player("Player 1", 10)};
    m_loss_player_idx = -1;
    m_pot = 0;
    m_num_player = 2;
    m_round_count = 0;
    m_eval = omp::HandEvaluator();
    srand(time (nullptr));
}

void Game::transition(Game_state& new_state)
{
    m_state->exit(this);
    m_state = &new_state;
    m_state->enter(this);
}

void Game::process_state()
{
    m_state->process(this);
}

void Game::run(unsigned iteration)
{
    m_iteration = iteration;
    while (m_round_count < iteration)
    {
        process_state();
    }
}

// a round is finished if all player had a valid action
bool Game::round_finished() const
{
    // either all player check
    if (m_players[0].get_last_action_in_round() == Action::CHECK)
    {
        return m_players[1].get_last_action_in_round() == Action::CHECK;
    }
    // or no one checks
    else if (m_players[0].get_last_action_in_round() == Action::FOLD || m_players[0].get_last_action_in_round() == Action::BET)
    {
        return m_players[1].get_last_action_in_round() == Action::FOLD || m_players[1].get_last_action_in_round() == Action::BET;
    }
    return false;
}


void Game::clear_round_action()
{
    for (auto& player : m_players)
    {
        player.clear_round_action();
    }
}


void Game::clear_his_action()
{
    for (auto& player : m_players)
    {
        player.clear_his_action();
    }
}


void Game::update_his_action()
{
    for (auto& player : m_players)
    {
        player.update_his_action();
    }
}

void Game::clear_player_hand()
{
    for (auto& player : m_players)
    {
        player.clear_hand();
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
    std::cout << "Hand ranking=" << hand_ranking.at(score/4096) << std::endl;
    return score;
}


void Game::print_round_action() const
{
    for (const auto& player : m_players)
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


/**
 * The function return an uint representation of a card, so that it could be used to evaluate a hand
 * CardIdx is an integer between 0 and 51, so that CARD = 4 * RANK + SUIT, where
 * rank ranges from 0 (deuce) to 12 (ace) and suit is from 0 (spade), 1 (heart), 2 (club), 3 (diamond).
 *
 * @param  hand
 * @return 16bit 
 */
inline unsigned translate_card_to_idx(Card_visualization card)
{   
    return 4 * ((card.rank + 11) % 13) + suits_idx.at(card.suit);
}



// pick 9 cards from 0-51
// card[0, 1] is assigned to player 0, card[2, 3] is assgined to player 1
// card[4, 8] are community cards
void Game::shuffle_and_deal()
{
    // std::random_device dev;
    // std::mt19937 rng(dev());
    // std::uniform_int_distribution<std::mt19937::result_type> dist(0, 51);
    while (m_cards.size() < 9)
    {
        // unsigned rand = dist(rng);
        unsigned random = rand() % 51;
        if (std::find(m_cards.begin(), m_cards.end(), random) == m_cards.end())
        {
            m_cards.push_back(random);
        }
    }
}