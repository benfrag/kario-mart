#ifndef ANIMATION_SYSTEM_H
#define ANIMATION_SYSTEM_H

#include "engine/core/ecs/system.h"

class AnimationSystem : public System
{
public:
    void update(void* engine_core, float dt) override;
};

#endif
