#include "treenode.h"


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
    // if (this->infoset_str.back() == 'F') s << "printing FOLD TERMINAL node:\n";
    // else s << "printing SHOWDOWN TERMINAL node:\n";
    // s << "Active Player: " << this->active_player_idx << "\n";

    //utility
    s << "Utility: [" << this->utility.first << ", " << this->utility.second << "]" << "\n";
    if (this->utility.first > 0) s << "Player 0 wins" << "\n";
    else s << "Player 1 wins" << "\n";
    // s << "\ninfoset_str=[";
    // s << this->infoset_str;
    // s << "]\n";
    s << "\n";
}


// parent must be an action node
// given build node args, calculate utility
void TerminalNode::build_terminal_node(TreeNode* parent, bool is_showdown, const omp::HandEvaluator & m_eval, const Build_node_args & args)
{

    int active_player_idx = args.active_player_idx;
    // this->active_player_idx = active_player_idx;

    assert(args.private_card.size() == 4);
    // this->infoset_str = construct_infoset_key(args.private_card, args.community_card, args.action_history);

    // if reached showdown round, call hand evaluator to determine which player won
    if (is_showdown)
    {
        // committed passed in is after action a
        std::pair<uint8_t, uint8_t> committed = args.committed;

        Hand active_player_cards = {args.private_card[2 * active_player_idx], args.private_card[2 * active_player_idx + 1] };
        active_player_cards.insert(active_player_cards.end(), args.community_card.begin(), args.community_card.end());

        Hand opp_cards = {args.private_card[2 * (active_player_idx ^ 1)], args.private_card[2 * (active_player_idx ^ 1) + 1] };
        opp_cards.insert(opp_cards.end(), args.community_card.begin(), args.community_card.end());

        uint16_t score_active = evaluate_hand(active_player_cards, m_eval);
        uint16_t score_opp = evaluate_hand(opp_cards, m_eval);

        this->utility = calculate_utility(score_active, score_opp, active_player_idx, committed);
    }

    // active player folded that leads to terminal node
    else
    {
        int loser = active_player_idx;
        int winner = loser ^ 1;
        int loser_committed = loser == 0 ? args.committed.first : args.committed.second;
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
    // size of chidlren
    s << "Number of Children:" << this->children.size() << "\n";
    // chance prob
    s << "Chance Probability:" << this->chance_prob << "\n" << "\n";
    // s << "\ninfoset_str=[";
    // s << this->infoset_str;
    // s << "]\n";
    s << "\n";
}


// parent must be an action node
void ChanceNode::build_chance_node(TreeNode* parent, const Build_node_args & args)
{   
    (void) parent;
    (void) args;
    // this->infoset_str = construct_infoset_key(args.private_card, args.community_card, args.action_history);
    return;
}


void ActionNode::print_node(int depth, std::ostream& s)
{
    s << "=================== depth = " << depth << " ===================\n";
    s << "printing ACTION node:\n";
    //active player index
    s << "Active Player: " << (unsigned) this->infoset.active_player_idx << "\n";
    // size of chidlren
    s << "Number of Children:" << this->children.size() << "\n";
    
    //round index
    print_round_name(this->infoset.round_idx, s);

    if (this->infoset.action_history != 0)
    {
        s << "Action_history: ";
        UTILS::print_action_history(this->infoset.action_history, s);
    }

    if (this->infoset.private_card != 0)
    {
        // printf("private_card = %x\n", infoset.private_card);
        // 0xCF2 is the largest possible private card = {51, 50} in binary
        assert(this->infoset.private_card >= 0 && this->infoset.private_card <= 0xCF2);
        UTILS::print_hand<uint16_t>(this->infoset.private_card, s, 2);
    }
    if (this->infoset.community_card != 0)
    {
        // 0x33CB1C2F is the largest possible community card = {51, 50, 49, 48, 47} in binary
        assert(this->infoset.community_card >= 0 && this->infoset.community_card <= 0x33CB1C2F);
        s << "Community_card: ";
        // flop
        if (this->infoset.round_idx == 1) UTILS::print_hand<uint32_t>(this->infoset.community_card, s, 3);
        // turn
        else if (this->infoset.round_idx == 2) UTILS::print_hand<uint32_t>(this->infoset.community_card, s, 4);
        // river
        else if (this->infoset.round_idx == 3) UTILS::print_hand<uint32_t>(this->infoset.community_card, s, 5);
        else fprintf(stderr, "round_idx=%d in print_infoset\n", this->infoset.round_idx);
    }


    std::string infoset_key = decode_infoset(this->infoset);
    s << "\ninfoset_key=[";
    s << infoset_key;
    s << "]\n";

    /*
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
    */
    s << "\n";
}


// build a new action node that results from playing action a at parent node
// given build node args, construct a infoset key string
void ActionNode::build_action_node(TreeNode* parent, const Build_node_args & args)
{
    // pre flop round, P0 takes first 2 private card, P1 takes last 2 private card
    assert(args.private_card.size() == 4);
    Hand private_card = { args.private_card[2 * args.active_player_idx], args.private_card[2 * args.active_player_idx + 1] };

    // flop round, each player takes 3 community card
    if (args.round_idx == 1)
    {
        assert(args.community_card.size() == 3);
    }
    // turn, river round
    else if (args.round_idx == 2 || args.round_idx == 3)
    {
        assert(args.community_card.size() == 4 || args.community_card.size() == 5);
    }
    else if (args.round_idx != 0)
    {
        fprintf(stderr, "args.round_idx too large\n");
    }
    // this->infoset_key = construct_infoset_key(private_card, args.community_card, args.action_history);

    this->infoset = encode_infoset(private_card, args.community_card, args.action_history);
    this->infoset.active_player_idx = args.active_player_idx;
    this->infoset.round_idx = args.round_idx;
}
