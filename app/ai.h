#pragma once

#include "command.h"
#include "game.h"

class AI {
  public:
    virtual JoinParams join_params() = 0;
    virtual StartParams start_params(const GameResponse&) = 0;
    virtual CommandParams command_params(const GameResponse&) = 0;
};