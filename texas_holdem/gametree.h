#pragma once
#include <treenode.h>

class GameTree
{
    BaseNode* root;

    GameTree() : root(nullptr) {}

public:
    void build_tree();
    
};
