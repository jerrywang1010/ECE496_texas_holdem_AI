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


std::pair<int, int> calculate_utility(uint16_t score_active, uint16_t score_opp, int active_player_idx, int money_exchanged)
{
    if (score_active > score_opp)
    {
        return active_player_idx == 0 ? std::make_pair(money_exchanged, -1 * money_exchanged) : std::make_pair(-1 * money_exchanged, money_exchanged);
    }
    else if (score_active < score_opp)
    {
        return active_player_idx == 0 ? std::make_pair(-1 * money_exchanged, money_exchanged) : std::make_pair(money_exchanged, -1 * money_exchanged);
    }
    else
    {
        return std::make_pair(0, 0);
    }
}

void TerminalNode::print_node() override
{
    std::cout << "=========================\n";
    printf("printing terminal node:\n");
    this->infoset.print_infoset();
    //utility

    //active player index

    //round index


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

        assert(this->infoset.committed.first == this->infoset.committed.second);
        this->utility = calculate_utility(score_active, score_opp, this->active_player_idx, this->infoset.committed.first);
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


// parent must be an action node
void ChanceNode::build_chance_node(TreeNode* parent, Action a, const Board_state & args)
{
    assert(a == Action::BET || a == Action::CHECK);
    this->infoset.add_to_action_history(a);
    this->infoset.committed = parent->infoset.committed;
    this->round_idx = parent->round_idx + 1;
}


// build a new action node that results from playing action a at parent node
void ActionNode::build_action_node(TreeNode* parent, Action a, const Board_state & args)
{
    this->infoset.action_history = parent->infoset.action_history;
    this->infoset.committed = parent->infoset.committed;

    if (parent->is_chance)
    {
        this->active_player_idx = 0;
    }
    else
    {
        assert(a != Action::INVALID);
        this->infoset.add_to_action_history(a);
        this->action_this_round.push_back(a);
        int player_idx = dynamic_cast<ActionNode*> (parent)->active_player_idx;
        if (a == Action::BET) this->infoset.commit(player_idx, 1);
        this->active_player_idx = player_idx ^ 1;
    }

    // pre flop round, P0 takes first 2 private card, P1 takes last 2 private card
    assert(args.private_card.size() == 4);
    this->infoset.add_to_private_card( { args.private_card[2 * this->active_player_idx], args.private_card[2 * this->active_player_idx + 1] } );
    // flop round, each player takes 3 community card
    if (parent->round_idx == 1)
    {
        assert(args.community_card.size() == 3);
        this->infoset.add_to_community_card(args.community_card);
    }
    // turn, river round
    else if (parent->round_idx == 2 || parent->round_idx == 3)
    {
        assert(args.community_card.size() == 4 || args.community_card.size() == 5);
        this->infoset.add_to_community_card(args.community_card);
    }
    else if (parent->round_idx != 0)
    {
        fprintf(stderr, "parent->round_idx too large\n");
    }
}

/*
TerminalNode* TerminalNode::build_terminal_node(TreeNode* parent, bool is_showdown, Action a, const auto & m_eval)
{
    TerminalNode* node = new TerminalNode();
    node->infoset = parent->infoset;
    node->infoset.add_to_action_history(a);
    // if reached showdown round, call hand evaluator to determine which player won
    if (is_showdown)
    {
        if (a == Action::BET) node->infoset.commit(std::static_cast<ActionNode*> (parent)->active_player_idx, 1);
        // walk up the tree 2 steps to find hands of player 0 & 1
        Hand usable_cards = parent->infoset.private_card;
        usable_cards.insert(usable_cards.end(), parent->infoset.community_card.begin(), parent->infoset.community_card.end());
        uint16_t score_active = evaluate_hand(usable_cards, m_eval);

        ActionNode* grandparent = parent->parent;
        usable_cards = grandparent->infoset.private_card;
        usable_cards.insert(usable_cards.end(), grandparent->infoset.community_card.begin(), grandparent->infoset.community_card.end());
        
        // determine which player win and put it in utility
        uint16_t score_opp = evaluate_hand(usable_cards, m_eval);
        assert(node->infoset.committed.first == node->infoset.committed.second);
        node->utility = calculate_utility(score_active, score_opp, node->active_player_idx, node->infoset.committed.first);
    }

    // active player folded that leads to terminal node
    else
    {
        assert(a == Action::FOLD);
        int loser = std::static_cast<ActionNode*> (parent)->active_player_idx;
        int winner = loser ^ 1;
        int loser_committed = std::get<loser>(parent->infoset.committed);
        if (winner == 0)
        {
            node->utility = std::make_pair(loser_committed, -1 * loser_committed);
        }
        else
        {
            node->utility = std::make_pair(-1 * loser_committed, loser_committed);
        }
    }
    return node;
}


ActionNode* ActionNode::build_action_node(TreeNode* parent, Action a, Board_state args)
{
    ActionNode* node = new ActionNode();
    Infoset infoset;
    infoset.action_history = parent->infoset.action_history;
    infoset.committed = parent->infoset.committed;

    // pre flop round, private card is passed in, <c1, c2, c3, c4>
    // c1, c2 are for P0, c3, c4 are for P1
    if (parent->round_idx == 0)
    {
        // P0 temporarily holds all 4 cards
        if(parent->is_chance)
        {
            assert(chance_event.size() == 4);
            assert(a == Action::INVALID);
            node->active_player_idx = 0;
            Hand private_card = chance_event;
            // add all 4 cards to private hand
            node->infoset.add_to_private_card(private_card);
            // remove all 4 cards from deck
            node->infoset.remove_from_deck(private_card);
        }
        // P1 gets 2 private cards from P0, and deletes it's from P0
        else
        {
            assert(a != Action::INVALID);
            if (a == Action::BET) node->infoset.commit(0, 1);
            node->active_player_idx = 1;

            Hand four_card_hand = node->infoset.private_card;
            // erase the first 2 element because they belong to parent
            four_card_hand.erase(std::next(four_card_hand.begin()), std::next(four_card_hand.begin(), 2));
            node->infoset.private_card = four_card_hand;

            // erase last 2 element in parents private card, done only once
            four_card_hand = parent->infoset.private_card;
            assert(four_card_hand.size() == 4);
            four_card_hand.erase(std::next(four_card_hand.begin(), 2), four_card_hand.end());
            parent->infoset.private_card = four_card_hand;
            
            node->infoset.add_to_action_history(a);
            node->action_this_round.push_back(a);
        }
    }

    else
    {
        if(parent->is_chance)
        {
            node->active_player_idx = 0;
            // flop round
            else if (parent->round_idx == 1)
            {
                assert(chance_event.size() == 3);
                node->infoset.add_to_community_card(chance_event);
                node->infoset.remove_from_deck(chance_event);
            }
            // turn round, river round
            else if (parent->round_idx == 2 || parent->round_idx == 3)
            {
                assert(chance_event.size == 1);
                node->infoset.add_to_community_card(chance_event);
                node->infoset.remove_from_deck(chance_event);
            }
            else
            {
                fprintf(stderr, "Error when building tree, round idx too large for a chance node\n");
            }
        }

        // a can only be check or bet since fold leads to a terminal node
        else
        {
            assert(a != Action::INVALID);
            assert(chance_event.size() == 0)
            node->infoset.add_to_action_history(a);
            node->action_this_round.push_back(a);
            int player_idx =  std::static_cast<ActionNode*> (parent)->active_player_idx;
            if (a == Action::BET) node->infoset.commit(player_idx, 1);
            node->active_player_idx = player_idx ^ 1;
        }
    }
    return node;
}
*/
