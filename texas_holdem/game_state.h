#pragma once

// forward declaration to resolve circular dependency
class Game;

class Game_state
{
    
public:

    virtual void enter(Game* game) = 0;

    virtual void process(Game* game) = 0;
    
    virtual void exit(Game* game) = 0;

    virtual ~Game_state() {}

};

