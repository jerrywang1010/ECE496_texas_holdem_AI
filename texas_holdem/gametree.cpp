#include <queue>
#include "gametree.h"
#include <set>


void GameTree::build_tree(Hand deck)
{
    //create a chance node
    root = new ChanceNode();
    Board_state args;
    args.remaining_deck = deck;

    recursive_build_tree(root, args);
    //return all combination of choosing 4 cards from deck
    //std::vector<Card> comb = combination(int num_card, Hand deck);
}


void combination_helper_with_limit(std::vector<Hand> & res, const Hand & deck, int data[], int start, int end, int index, int r, int limit)
{
    if (index == r)
    {
        Hand curr_comb;
        for (int j = 0; j < r; j ++)
        {
            curr_comb.push_back(data[j]);
        }
        res.push_back(curr_comb);
        return;
    }
    for (int i = start; (i <= end) && (end - i + 1 >= r - index); i ++)
    {
        data[index] = deck[i];
        if (res.size() == limit){
            return;
        }
        combination_helper_with_limit(res, deck, data, i + 1, end, index + 1, r, limit);
    }
}


std::vector<Hand> all_combination_with_limit(Hand deck, int round_idx, int limit)
{
    std::vector<Hand> combinations;
    // entering pre flop round, choose 2 cards from deck, then choose another 2 cards from remaining deck, return all combination
    if (round_idx == 0)
    {
        std::vector<Hand> partial_result;
        int data[2];
        combination_helper_with_limit(partial_result, deck, data, 0, deck.size() - 1, 0, 2, limit);
        std::set<int> s(deck.begin(), deck.end());
        for (int i = 0; i < partial_result.size(); i++)
        {
            s.erase(partial_result[i][0]);
            s.erase(partial_result[i][1]);
            int data_card[2];
            std::vector<Hand> partial_result_2;
            Hand v(s.begin(), s.end());
            combination_helper_with_limit(partial_result_2, v, data_card, 0, v.size()-1, 0, 2, limit);
            for (int j = 0; j < partial_result_2.size(); j++)
            {
                if (combinations.size() == limit) return combinations;
                Hand vec = {partial_result[i][0], partial_result[i][1], partial_result_2[j][0], partial_result_2[j][1]};
                combinations.push_back(vec);
            }
            s.insert(partial_result[i][0]);
            s.insert(partial_result[i][1]);
        }
    }
    // entering flop round, choose 3 community card
    else if (round_idx == 1)
    {
        int data[3];
        combination_helper_with_limit(combinations, deck, data, 0, deck.size() - 1, 0, 3, limit);
    }
    // entering turn/river round, choose 1 community card
    else if (round_idx == 2 || round_idx == 3)
    {
        int data[1];
        combination_helper_with_limit(combinations, deck, data, 0, deck.size() - 1, 0, 1, limit);
    }
    return combinations;
}


void combination_helper(std::vector<Hand> & res, const Hand & deck, int data[], int start, int end, int index, int r)
{
    if (index == r)
    {
        Hand curr_comb;
        for (int j = 0; j < r; j ++)
        {
            curr_comb.push_back(data[j]);
        }
        res.push_back(curr_comb);
        return;
    }
    for (int i = start; (i <= end) && (end - i + 1 >= r - index); i ++)
    {
        data[index] = deck[i];
        combination_helper(res, deck, data, i + 1, end, index + 1, r);
    }
}


std::vector<Hand> all_combination(Hand deck, int round_idx)
{
    std::vector<Hand> combinations;
    // entering pre flop round, choose 2 cards from deck, then choose another 2 cards from remaining deck, return all combination
    if (round_idx == 0)
    {
        std::vector<Hand> partial_result;
        int data[2];
        combination_helper(partial_result, deck, data, 0, deck.size() - 1, 0, 2);
        std::set<int> s(deck.begin(), deck.end());
        for (int i = 0; i < partial_result.size(); i++)
        {
            s.erase(partial_result[i][0]);
            s.erase(partial_result[i][1]);
            int data_card[2];
            std::vector<Hand> partial_result_2;
            Hand v(s.begin(), s.end());
            combination_helper(partial_result_2, v, data_card, 0, v.size()-1, 0, 2);
            for (int j = 0; j < partial_result_2.size(); j++)
            {
                Hand vec = {partial_result[i][0], partial_result[i][1], partial_result_2[j][0], partial_result_2[j][1]};
                combinations.push_back(vec);
            }
        s.insert(partial_result[i][0]);
        s.insert(partial_result[i][1]);
        }
    }
    // entering flop round, choose 3 community card
    else if (round_idx == 1)
    {
        int data[3];
        combination_helper(combinations, deck, data, 0, deck.size() - 1, 0, 3);
    }
    // entering turn/river round, choose 1 community card
    else if (round_idx == 2 || round_idx == 3)
    {
        int data[1];
        combination_helper(combinations, deck, data, 0, deck.size() - 1, 0, 1);
    }
    return combinations;
}


Node_type GameTree::get_next_node_type(TreeNode* parent, Action a)
{
    if (a == Action::FOLD)
    {
        return Node_type::Terminal;
    }
    if (parent->is_chance) 
    {
        // next node must be action
        return Node_type::Action;
    }
    if (dynamic_cast<ActionNode*> (parent)->action_this_round.size() == 0)
    {
        return Node_type::Action;
    }
    
    Action prev_action = dynamic_cast<ActionNode*> (parent)->action_this_round.back();
    
    // if prev act = B, then 'B(p)B(c)' or 'CB(p)B(c)' next must be next round 
    // if prev act = C, then 'C(p)C(c)' -> next round
    if (prev_action == Action::BET || (prev_action == Action::CHECK && a == Action::CHECK))
    {
        if (parent->round_idx == 3)
        {
            return Node_type::Showdown;
        }
        else
        {
            // round end
            return Node_type::Chance;
        }
    }
    // if prev act = C and 'C(p)B(c)'
    return Node_type::Action;
}


std::vector<Action> GameTree::get_legal_actions(TreeNode* parent)
{
    // no need to worry sth like CBBB, this will be checked in the get_next_node_type
    // bet and fold must be legal action no matter what previous action is
    std::vector<Action> res{Action::FOLD, Action::BET};
    // if it's preflop round then no check are allowed
    if (parent->round_idx > 0)
    {
        if (parent->is_chance)
        {
            res.push_back(Action::CHECK);
        }
        else
        {
            // if there is a bet in the action history in this round then check is not allowed
            // can we add an variable with type action into the Action node
            // if added then
            // if (parent->action_type == Action::Bet);
            // else res.push_back(1);
            std::vector<Action> actions_in_round = dynamic_cast<ActionNode*> (parent)->action_this_round;
            if (std::find(actions_in_round.begin(), actions_in_round.end(), Action::BET) != actions_in_round.end());
            else
            {
                res.push_back(Action::CHECK);
            }
        }
    }
    return res;
}


void GameTree::recursive_build_tree(TreeNode* parent, Board_state args)
{
    if (parent->is_chance)
    {
        Board_state original_args = args;
        // std::vector<Hand> chance_events = all_combination(args.remaining_deck, parent->round_idx);
        std::vector<Hand> chance_events = all_combination_with_limit(args.remaining_deck, parent->round_idx, 1);
        dynamic_cast<ChanceNode*> (parent)->chance_prob = 1.0f / chance_events.size();
        // first action node in preflop round 
        // if it building the first action node in preflop round, let the first node temporarily hold all 4 private card
        // then the second action node query it's parent and get it's private hand and delete it in parents 
        for (Hand & h : chance_events)
        {   
            args.remove_from_deck(h);
            if (parent->round_idx == 0)
            {
                args.add_to_private_card(h);
            }
            else
            {
                args.add_to_community_card(h);
            }
            // build all action nodes result from a chance event
            ActionNode* node = new ActionNode();
            node->build_action_node(parent, Action::INVALID, args);
            parent->children.push_back(node);
            recursive_build_tree(node, args);
            // back track
            args = original_args;
        }
    }

    // parent must be an action node
    else
    {
        std::vector<Action> legal_actions = get_legal_actions(parent);
        for (Action a : legal_actions)
        {
            // 2 base case, no more recursive call after terminal nodes
            // if playing a lead to terminal node
            Node_type next_node = get_next_node_type(parent, a);
            if (next_node == Node_type::Terminal)
            {
                TerminalNode* node = new TerminalNode();
                node->build_terminal_node(parent, false, a, m_eval, args);
                parent->children.push_back(node);
            }
            else if (next_node == Node_type::Showdown)
            {
                TerminalNode* node = new TerminalNode();
                node->build_terminal_node(parent, true, a, m_eval, args);
                parent->children.push_back(node);
            }

            else if (next_node == Node_type::Chance)
            {
                ChanceNode* node = new ChanceNode();
                node->build_chance_node(parent, a, args);
                parent->children.push_back(node);
                recursive_build_tree(node, args);
            }
            else
            {
                ActionNode* node = new ActionNode();
                node->build_action_node(parent, a, args);
                parent->children.push_back(node);
                recursive_build_tree(node, args);
            }
        }
    }
}


void GameTree::print_tree(std::ostream& s)
{
    std::queue<std::pair<int, TreeNode*> > nodes;
    nodes.push(std::make_pair(0, this->root));
    while (!nodes.empty())
    {
        int curr_depth = nodes.front().first;
        TreeNode* node = nodes.front().second;
        nodes.pop();
        tree_with_depth[curr_depth].push_back(node);
        for (TreeNode* child : node->children)
        {
            nodes.push(std::make_pair(curr_depth + 1, child));
        }
    }
    for (int depth = 0; depth < this->tree_with_depth.size(); depth ++)
    {
        // std::cout << "Nodes at depth: " << depth << "\n";
        for (TreeNode* node : this->tree_with_depth[depth])
        {
            if (node->is_chance)
            {
                node = dynamic_cast<ChanceNode*> (node);
            }
            else if (node->is_terminal)
            {
                node = dynamic_cast<TerminalNode*> (node);
            }
            else
            {
                node = dynamic_cast<ActionNode*> (node);
            }
            node->print_node(depth, s);
        }
    }
}

