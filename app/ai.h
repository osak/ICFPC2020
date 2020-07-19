#pragma once

#include "command.h"
#include "game.h"

class AI {
  public:
    virtual StartParams init() = 0;
    virtual void post_start(const GameState&) = 0;
    virtual vector<Command*> think(const GameState&) = 0;
}