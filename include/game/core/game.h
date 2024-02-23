#ifndef GAME_H
#define GAME_H

#include "engine/core/engine.h"
#include "game/components/include_components.h"
#include "game/systems/include_systems.h"

class Game
{
public:
    Game(EngineCore* engine);
    
    void setup();

private:
    EngineCore* engine;

};

#endif
