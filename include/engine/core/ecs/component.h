#ifndef COMPONENT_H
#define COMPONENT_H

#include "engine/core/ecs/entity.h"


#include <unordered_map>
#include <memory>
#include <typeindex>
#include <type_traits>


class ComponentManager
{
public:
    template<typename T>
    void add_component(Entity entity, T component)
    {
        auto& component_array = get_component_array<T>();
        component_array[entity] = std::move(component);
    }

    template<typename T>
    void remove_component(Entity entity)
    {
        auto& component_array = get_component_array<T>();
        auto it = component_array.find(entity);
        if (it != component_array.end())
        {
            component_array.erase(it);
        }
    }

    template<typename T>
    T* get_component(Entity entity)
    {
        auto& component_array = get_component_array<T>();
        auto it = component_array.find(entity);
        if (it != component_array.end())
        {
            return &it->second;
        }
        return nullptr;
    }

    void entity_destroyed(Entity entity)
    {
        for (auto& [type_i, component_array] : component_arrays)
        {
            component_array->entity_destroyed(entity);
        }
    }

private:
    class IComponentArray
    {
    public:
        virtual ~IComponentArray() = default;
        virtual void entity_destroyed(Entity entity) = 0;
    };

    template<typename T>
    class ComponentArray : public IComponentArray
    {
    public:
        void entity_destroyed(Entity entity) override
        {
            auto it = components.find(entity);
            if (it != components.end()) {
                components.erase(it);
            }
        }
        std::unordered_map<Entity, T> components;
    };

    template<typename T>
    std::unordered_map<Entity, T>& get_component_array()
    {
        std::type_index type_i = std::type_index(typeid(T));
        if (component_arrays.find(type_i) == component_arrays.end())
        {
            auto new_component_array = std::make_unique<ComponentArray<T>>();
            component_arrays[type_i] = std::move(new_component_array);
        }
        return static_cast<ComponentArray<T>*>(component_arrays[type_i].get())->components;
    }

    std::unordered_map<std::type_index, std::unique_ptr<IComponentArray>> component_arrays;
};

#endif
