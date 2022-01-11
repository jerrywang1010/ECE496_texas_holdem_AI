#pragma once
#include "game_state.h"
#include "game.h"

class Pre_flop : public Game_state
{

public:
    void enter(Game* game);

    void exit(Game* game);
    
    void process(Game* game);

    static Game_state& get_instance();


// Following the sigleton design pattern to avoid unnecessary memory allocations, keep all constructors private
private:
    Pre_flop() {}

    Pre_flop(const Pre_flop& other);

    Pre_flop& operator=(const Pre_flop& other);
};


class Flop : public Game_state
{

public:
    void enter(Game* game);

    void exit(Game* game);
    
    void process(Game* game);

    static Game_state& get_instance();


// Following the sigleton design pattern to avoid unnecessary memory allocations, keep all constructors private
private:
    Flop() {}

    Flop(const Flop& other);

    Flop& operator=(const Flop& other);

    bool check_allowed;

    
};



class Turn : public Game_state
{

public:
    void enter(Game* game);

    void exit(Game* game);
    
    void process(Game* game);

    static Game_state& get_instance();


// Following the sigleton design pattern to avoid unnecessary memory allocations, keep all constructors private
private:
    Turn() {}

    Turn(const Turn& other);

    Turn& operator=(const Turn& other);

    bool check_allowed;
};


class River : public Game_state
{

public:
    void enter(Game* game);

    void exit(Game* game);
    
    void process(Game* game);

    static Game_state& get_instance();


// Following the sigleton design pattern to avoid unnecessary memory allocations, keep all constructors private
private:
    River() {}

    River(const River& other);

    River& operator=(const River& other);

    bool check_allowed;
};


class Showdown : public Game_state
{

public:
    void enter(Game* game);

    void exit(Game* game);
    
    void process(Game* game);

    static Game_state& get_instance();


// Following the sigleton design pattern to avoid unnecessary memory allocations, keep all constructors private
private:
    Showdown() {}

    Showdown(const Showdown& other);

    Showdown& operator=(const Showdown& other);
};


class Terminal : public Game_state
{

public:
    void enter(Game* game);

    void exit(Game* game);
    
    void process(Game* game);

    static Game_state& get_instance();


// Following the sigleton design pattern to avoid unnecessary memory allocations, keep all constructors private
private:
    Terminal() {}

    Terminal(const Terminal& other);

    Terminal& operator=(const Terminal& other);
};
