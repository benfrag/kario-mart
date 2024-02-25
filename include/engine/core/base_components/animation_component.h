#ifndef ANIMATION_COMPONENT_H
#define ANIMATION_COMPONENT_H

#include "engine/utils/utils_colour.h"
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



/*class ColourInterpTask : public AnimationTask
{
public:
    uint32_t* value;
    HSL start;
    HSL end;
    float elapsed = 0;
    float total_time; // seconds
    bool finished = false;

    ColourInterpTask(uint32_t* value, RGB _start, RGB _end, float time) : value(value), total_time(time) {
        this->start = rgb_to_hsl(_start);
        this->end = rgb_to_hsl(_end);
}

    void update(float dt) override
    {
        elapsed += dt;
        float t = elapsed / total_time;
        HSL interp = {
            start.h + (end.h - start.h) * t,
            start.s + (end.s - start.s) * t,
            start.l + (end.l - start.l) * t
        };
        uint32_t newcol = hsl_to_packed_rgb(interp);
        *value = newcol;
    }

    bool is_complete() override
    {
        if (elapsed >= total_time)
        {
//            *value = end;
            
            *value = hsl_to_packed_rgb(end);
            return true;
        }
        return false;
    }
};
*/

class LoopColourInterpTask : public AnimationTask
{
public:
    uint32_t* value;
    std::vector<HSL> colours;
    float elapsed = 0;
    float segment_time;
    int start_segment = 0;
    int end_segment = 0;

    LoopColourInterpTask(uint32_t* value, std::vector<RGB>_colours, float segment_time) : value(value), segment_time(segment_time) {
        colours.reserve(_colours.size());
        for (std::size_t i = 0; i < _colours.size(); ++i)
        {
            colours.push_back(rgb_to_hsl(_colours[i]));
        }
        start_segment = 0;
        end_segment = start_segment + 1 >= colours.size() ? 0 : start_segment + 1; 
    }

    void update(float dt) override
    {
        elapsed += dt;
        if (elapsed >= segment_time)
        {
            elapsed = 0;
            start_segment = end_segment;
            end_segment = start_segment + 1 >= colours.size() ? 0 : start_segment + 1; 
        }
        float t = elapsed / segment_time;

        HSL start = colours[start_segment];
        HSL end = colours[end_segment];

        HSL interp = {
            start.h + (end.h - start.h) * t,
            start.s + (end.s - start.s) * t,
            start.l + (end.l - start.l) * t
        };

        uint32_t newcol = hsl_to_packed_rgb(interp);
        *value = newcol;
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
