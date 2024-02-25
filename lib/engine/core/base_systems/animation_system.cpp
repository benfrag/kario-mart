#include "engine/core/engine.h"
#include "engine/core/base_systems/animation_system.h"

void AnimationSystem::update(void* engine_core, float dt)
{
    EngineCore* engine = static_cast<EngineCore*>(engine_core);
    for (auto entity : entities)
    {
        AnimationComponent* animation = engine->ecs.get_component<AnimationComponent>(entity);

        for (std::size_t i = 0; i < animation->animation_tasks.size(); ++i)
        {
            if (animation->animation_tasks[i]->is_complete())
            {
                animation->animation_tasks.erase(animation->animation_tasks.begin() + i);
                --i;
                continue;
            }

            animation->animation_tasks[i]->update(dt);
        }

    }
}
