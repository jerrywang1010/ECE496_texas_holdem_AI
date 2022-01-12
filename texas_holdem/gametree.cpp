#include "gametree.h"


void GameTree::build_tree(Hand deck)
{
    //create a chance node
    root = new ChanceNode();
    //return all combination of choosing 4 cards from deck
    //std::vector<Card> comb = combination(int num_card, Hand deck);
}


typedef unsigned Card;
typedef std::vector<Card> Hand;
std::vector<std::vector<unsigned> > all_combination(std::vector<unsigned> deck, int round_idx)
{
    // pre flop round
    /*
    (1,2,3,4)

    (1, 2) (3, 4)
    (1, 3) (2, 4)
    (1, 4) (2, 3)

    (2, 3) (1, 4)
    (2, 4) (1, 3)
    
    (3, 4) (1, 2)
    */
    if (round_idx == 0)
    // first find all 2 card hand from deck, then find all 2 card hand from the remaining deck, return vector<4 card hand> 

    // flop round
    if (round_idx == 1)
    // find all combination of 3 card hands from deck, no duplication, return as result

    // 
    if (round_idx == 2 || 3 || 4)
    // find all combination of 1 card hands from deck, no duplication, return as result
}




void GameTree::recursive_build_tree(BaseNode* parent)
{
    if (parent->is_chance)
    {
        // TODO
        std::vector<Hand> all_comb = all_combination(std::static_cast<ChanceNode*> (parent)->remainder_deck, parent->round_idx);
        std::static_cast<ChanceNode*> parent->chance_prob = 1.0f / all_comb.size();

        for (Hand & h : all_comb)
        {   
            // TODO, update info set
            ActionNode* node = build_action_node(parent);
            parent->children.push_back(node);
            recursive_build_tree(node);
        }
    }

    // parent must be an action node
    else
    {
        // TODO
        std::vector<Action> legal_actions = get_legal_actions(parent);

        for (Action a : legal_actions)
        {
            // if playing a lead to terminal node
            // TODO
            Node_type next_node = get_next_node_type(parent, a);
            if (next_node == Node_type::Terminal)
            {
                // TODO
                TerminalNode* node = build_terminal_node();
                parent->children.push_back(node);
            }

            else if (next_node == Node_type::Chance)
            {
                // TODO
                ChanceNode* node = build_chance_node();
                parent->children.push_back(node);
                recursive_build_tree(node);
            }

            else
            {
                // TODO
                ActionNode* node = build_action_node();
                parent->children.push_back(node);
                recursive_build_tree(node);
            }
        }
    }
}


//pesudcode
/*

void recursive_build_tree(BaseNode* parent){

    //determine which type of node this is
    //if parent is chance node, this is action node
    //if parent is action node, this can be terminal,chance or action
    //use action_this_round of parent, if action_this_round = BB/CC/CBB, this is chance node
    //use action_this_round of parent, if action_this_round = xxF, this is terminal node
    //or use max_depth of tree to determine

    if(chancenode){
        for comb in combinations:
            build_chance_node(comb);
    }elseif(terminalnode){
        build_terminal_node();
    }else{
        if(fold){
            build_action_node(fold);
        }
        if(check){
            build_action_node(check);
        }
        if(bet){
            build_action_node(bet);
        }
        
    }
}

void build_chance_node(BaseNode* parent){

    //constructor
    ChanceNode* CN = new ChanceNode();
    CN.parent = parent;
    //update everything in CN

    parent.child.append(CN);

    recursive_build_tree();

}

void build_terminal_node(){

    //constructor
    TerminalNode* TN = new TerminalNode();
    TN.parent = parent;
    //update everything in TN

    parent.child.append(TN);

    //construct tree end here
}

void build_action_node(){

    //constructor
    ActionNode* AN = new ActionNode();
    AN.parent = parent;
    //update everything in TN

    parent.child.append(AN);

    recursive_build_tree();
}



*/
