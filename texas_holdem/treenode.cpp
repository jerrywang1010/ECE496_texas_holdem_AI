#include "treenode.h"
#include <string.h>

uint16_t evaluate_hand(const Hand & hand, const omp::HandEvaluator & m_eval)
{
    omp::Hand h = omp::Hand::empty();
    for (Card card : hand)
    {
        h += omp::Hand(card);
    }
    return m_eval.evaluate(h);
}


std::pair<float, float> calculate_utility(uint16_t score_active, uint16_t score_opp, int active_player_idx, std::pair<int, int> committed)
{
    if (score_active > score_opp)
    {
        return active_player_idx == 0 ? std::make_pair(committed.second, -1 * committed.second) : std::make_pair(-1 * committed.first, committed.first);
    }
    else if (score_active < score_opp)
    {
        return active_player_idx == 0 ? std::make_pair(-1 * committed.first, committed.first) : std::make_pair(committed.second, -1 * committed.second);
    }
    else
    {
        float payoff = (committed.first + committed.second) / 2.0f;
        return std::make_pair(payoff - committed.first, payoff - committed.second);
    }
}

void print_round_name(int idx, std::ostream& s)
{
    if (idx == 0)
    {
        s << "Round: Pre Flop" << "\n";
    } 
    else if (idx == 1)
    {
        s << "Round Flop" << "\n";
    } 
    else if (idx == 2)
    {
        s << "Round: Turn" << "\n";
    } 
    else if (idx == 3) 
    {
        s << "Round: River" << "\n";
    }
    else
    {
        s << "Round: Showdown/Terminal" << "\n";
    }
}

void TerminalNode::print_node(int depth, std::ostream& s)
{
    s << "=================== depth = " << depth << " ===================\n";
    if (GET_LAST_ACTION(this->infoset.action_history) == Action::FOLD) s << "printing FOLD TERMINAL node:\n";
    else s << "printing SHOWDOWN TERMINAL node:\n";
    s << "Active Player: " << this->active_player_idx << "\n";
    //round index
    print_round_name(this->infoset.round_idx, s);
    //num_of_children
    s << "Number of Children: " << this->children.size() << "\n";
    //utility
    s << "Utility: [" << this->utility.first << ", " << this->utility.second << "]" << "\n";
    if (this->utility.first > 0) s << "Player 0 wins" << "\n";
    else s << "Player 1 wins" << "\n";
    s << "\n";
    this->infoset.print_infoset(s);
    s << "\n";
}

// parent must be an action node
void TerminalNode::build_terminal_node(TreeNode* parent, bool is_showdown, Action a, const omp::HandEvaluator & m_eval, const Board_state & args)
{
    this->infoset.action_history = parent->infoset.action_history;
    this->infoset.committed = parent->infoset.committed;
    assert(a != Action::INVALID);
    this->infoset.add_to_action_history(a);
    int active_player_idx = dynamic_cast<ActionNode*> (parent)->active_player_idx;
    this->active_player_idx = active_player_idx;
    this->infoset.round_idx = parent->infoset.round_idx;

    // if reached showdown round, call hand evaluator to determine which player won
    if (is_showdown)
    {
        if (a == Action::BET) this->infoset.commit(active_player_idx, 1);
        Hand active_player_cards = {args.private_card[2 * active_player_idx], args.private_card[2 * active_player_idx + 1] };
        active_player_cards.insert(active_player_cards.end(), args.community_card.begin(), args.community_card.end());

        Hand opp_cards = {args.private_card[2 * (active_player_idx ^ 1)], args.private_card[2 * (active_player_idx ^ 1) + 1] };
        opp_cards.insert(opp_cards.end(), args.community_card.begin(), args.community_card.end());

        uint16_t score_active = evaluate_hand(active_player_cards, m_eval);
        uint16_t score_opp = evaluate_hand(opp_cards, m_eval);
        
        // std::cout << "active idx=" << active_player_idx << std::endl;
        // std::cout << "active player cards: ";
        // UTILS::print_vec<Card>(active_player_cards, std::cout);
        // std::cout << "oppo cards: ";
        // UTILS::print_vec<Card>(opp_cards, std::cout);

        // std::cout << "score active=" << score_active << ", score_opp=" << score_opp << "\n";

        this->utility = calculate_utility(score_active, score_opp, this->active_player_idx, this->infoset.committed);
    }

    // active player folded that leads to terminal node
    else
    {
        assert(a == Action::FOLD);
        int loser = active_player_idx;
        int winner = loser ^ 1;
        int loser_committed = loser == 0 ? parent->infoset.committed.first : parent->infoset.committed.second;
        if (winner == 0)
        {
            this->utility = std::make_pair(loser_committed, -1 * loser_committed);
        }
        else
        {
            this->utility = std::make_pair(-1 * loser_committed, loser_committed);
        }
    }
}

void ChanceNode::print_node(int depth, std::ostream& s)
{
    s << "=================== depth = " << depth << " ===================\n";
    s << "printing CHANCE node:\n";
    // round idx
    print_round_name(this->infoset.round_idx, s);
    // size of chidlren
    s << "Number of Children:" << this->children.size() << "\n";
    // chance prob
    s << "Chance Probability:" << this->chance_prob << "\n" << "\n";
    this->infoset.print_infoset(s);
    s << "\n";
}


// parent must be an action node
void ChanceNode::build_chance_node(TreeNode* parent, Action a, const Board_state & args)
{
    assert(a == Action::BET || a == Action::CHECK);
    this->infoset.action_history = dynamic_cast<ActionNode*> (parent)->infoset.action_history;
    this->infoset.add_to_action_history(a);
    this->infoset.committed = parent->infoset.committed;
    if (a == Action::BET) this->infoset.commit(dynamic_cast<ActionNode*> (parent)->active_player_idx, 1);
    this->infoset.round_idx = parent->infoset.round_idx + 1;
}


void ActionNode::print_node(int depth, std::ostream& s)
{
    s << "=================== depth = " << depth << " ===================\n";
    s << "printing ACTION node:\n";
    //active player index
    s << "Active Player: " << this->active_player_idx << "\n";
    //round index
    print_round_name(this->infoset.round_idx, s);
    // size of chidlren
    s << "Number of Children:" << this->children.size() << "\n";
    if (this->action_this_round != 0)
    {
        s << "Action this round: ";
        UTILS::print_action_history(this->action_this_round, s);
        s << "\n";
    }
    s << "\n";
    this->infoset.print_infoset(s);

    s << "----------CFR utility and strategy----------" << "\n";
    if (sigma.size())
    {
        s << "Sigma of [Bet, Fold, (Check)] = ";
        UTILS::print_vec<float>(this->sigma, s);
        s << "Cumulative sigma of [Bet, Fold, (Check)] = ";
        UTILS::print_vec<float>(this->cumulative_sigma, s);
        s << "Cumulative CFR regret of [Bet, Fold, (Check)] = ";
        UTILS::print_vec<float>(this->cumulative_cfr_regret, s);
    }

    s << "\n";
}


// build a new action node that results from playing action a at parent node
void ActionNode::build_action_node(TreeNode* parent, Action a, const Board_state & args)
{
    this->infoset.action_history = parent->infoset.action_history;
    this->infoset.committed = parent->infoset.committed;
    this->infoset.round_idx = parent->infoset.round_idx;

    if (parent->is_chance)
    {
        this->active_player_idx = 0;
    }
    else
    {
        assert(a != Action::INVALID);
        this->infoset.add_to_action_history(a);
        this->action_this_round = dynamic_cast<ActionNode*> (parent)->action_this_round;
        ADD_TO_ACTIONS(this->action_this_round, a);
        int player_idx = dynamic_cast<ActionNode*> (parent)->active_player_idx;
        if (a == Action::BET) this->infoset.commit(player_idx, 1);
        this->active_player_idx = player_idx ^ 1;
    }

    // pre flop round, P0 takes first 2 private card, P1 takes last 2 private card
    assert(args.private_card.size() == 4);
    this->infoset.add_to_private_card( { args.private_card[2 * this->active_player_idx], args.private_card[2 * this->active_player_idx + 1] } );
    // flop round, each player takes 3 community card
    if (parent->infoset.round_idx == 1)
    {
        assert(args.community_card.size() == 3);
        this->infoset.add_to_community_card(args.community_card);
    }
    // turn, river round
    else if (parent->infoset.round_idx == 2 || parent->infoset.round_idx == 3)
    {

        assert(args.community_card.size() == 4 || args.community_card.size() == 5);
        this->infoset.add_to_community_card(args.community_card);
    }
    else if (parent->infoset.round_idx != 0)
    {
        fprintf(stderr, "parent->infoset.round_idx too large\n");
    }
}
