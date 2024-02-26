#ifndef SYSTEM_H
#define SYSTEM_H

#include "engine/core/ecs/component.h"
#include "engine/core/ecs/interface.h"
#include <set>

class System
{
public:
//    explicit System(IECSController* _ecs) : ecs(_ecs) {}
    void add_entity(Entity entity)
    {
        entities.insert(entity);
    }

    void remove_entity(Entity entity)
    {
        entities.erase(entity);
    }

    void on_entity_destroyed(Entity entity)
    {
        remove_entity(entity);
    }

    virtual void update(void* engine_core, float dt) = 0;

    std::set<Entity> entities;

 //   IECSController* ecs;
};

class SystemManager
{
public:
//    IECSController* ecs;


    template<typename T>
    std::shared_ptr<T> register_system() {
        const char* type_name = typeid(T).name();

        auto system = std::make_shared<T>();
        systems.insert({type_name, system});
        return system;
    }

    template<typename T>
    void set_signature(Signature signature)
    {
        const char* type_name = typeid(T).name();

        signatures.insert({type_name, signature});
    }

    void entity_destroyed(Entity entity)
    {
        for (auto const& pair : systems)
        {
            auto const& system = pair.second;
            system->on_entity_destroyed(entity);
        }
    }

    void entity_signature_changed(Entity entity, Signature entity_signature)
    {
        for (auto const& pair : systems)
        {
            auto const& type = pair.first;
            auto const& system = pair.second;
            auto const& system_signature = signatures[type];

            if ((entity_signature & system_signature) == system_signature)
            {
                system->entities.insert(entity);
            }
            else
            {
                system->entities.erase(entity);
            }
        }
    }

    void update_all(void* engine_core, float dt)
    {
        for (auto const& pair : systems)
        {
            auto const& system = pair.second;
            system->update(engine_core, dt);
        }
    }

private:
    std::unordered_map<const char*, Signature> signatures;
    std::unordered_map<const char*, std::shared_ptr<System>> systems;

};

#endif
