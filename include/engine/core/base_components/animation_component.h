#ifndef ANIMATION_COMPONENT_H
#define ANIMATION_COMPONENT_H

#include <vector>

class AnimationTask
{
public:
    virtual void update(float dt) = 0;
    virtual bool is_complete() = 0;
};

class GenericAnimTask : public AnimationTask
{
public:
    float* value;
    float start;
    float end;
    float elapsed = 0;
    float total_time; // seconds
    bool finished = false;

    GenericAnimTask(float* value, float start, float end, float time) : value(value), start(start), end(end), total_time(time) {}

    void update(float dt) override
    {
        elapsed += dt;
        *value = start + (end - start) * elapsed / total_time; 
    }

    bool is_complete() override
    {
        if (elapsed >= total_time)
        {
            *value = end;
            return true;
        }
        return false;
    }
};

class LoopingFlipGenericAnimTask : public AnimationTask
{
public:
    float* value;
    float start;
    float end;
    float elapsed = 0;
    float total_time; // seconds
    bool finished = false;

    LoopingFlipGenericAnimTask(float* value, float start, float end, float time) : value(value), start(start), end(end), total_time(time) {}

    void update(float dt) override
    {
        elapsed += dt;
        if (elapsed >= total_time)
        {
            elapsed = 0;
            *value = end;
            float temp = start;
            start = end;
            end = temp;
        }
        *value = start + (end - start) * elapsed / total_time; 
    }

    bool is_complete() override
    {
        return false;
    }
};

struct AnimationComponent
{
    std::vector<AnimationTask*> animation_tasks;

    void add_task(AnimationTask* task)
    {
        animation_tasks.push_back(task);
    }

};

#endif
