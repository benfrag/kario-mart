#include "engine/utils/utils_colour.h"

#include <cmath>

HSL rgb_to_hsl(RGB col)
{
    RGB norm = {col.r / 255.f, col.g / 255.f, col.b / 255.f};
    float max = MAX(MAX(norm.r, norm.g), norm.b);
    float min = MIN(MIN(norm.r, norm.g), norm.b);
    float diff = max - min;
    HSL result;
    result.l = (max + min) / 2;
    if (diff != 0)
    {
        result.s = diff / (1 - std::abs(2 * result.l - 1));
        if (max == norm.r)
        {
            result.h = 60 * fmod(((norm.g - norm.b) / diff), 6);
        }
        else if (max == norm.g)
        {
            result.h = 60 * ((norm.b - norm.r) / diff) + 2;
        }
        else if (max == norm.b)
        {
            result.h = 60 * ((norm.r - norm.g) / diff) + 4;
        }
    }
    if (result.h < 0)
    {
        result.h += 360;
    }

    return result;
};

uint8_t hsl_val(float p, float q, float t)
{
    if (t < 0) t += 1;
    if (t > 1) t -= 1;
    if (t < 1/6.0f) return (p + (q - p) * 6 * t) * 255;
    if (t < 1/2.0f) return q * 255;
    if (t < 2/3.0f) return (p + (q - p) * (2/3.0f - t) * 6) * 255;
    return p * 255;
}

inline uint32_t UNSAFE_PACK(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
        return (static_cast<uint32_t>(a) << 24) |
            (static_cast<uint32_t>(r) << 16) |
            (static_cast<uint32_t>(g) << 8) |
            static_cast<uint32_t>(b);
}

uint32_t hsl_to_packed_rgb(HSL hsl)
{
    float q = hsl.l < 0.5 ? hsl.l * (1 + hsl.s) : hsl.l + hsl.s - hsl.l * hsl.s;
    float p = 2 * hsl.l - q;

    float h = hsl.h / 360;

    uint8_t r = hsl_val(p, q, h + 1/3.0f);
    uint8_t g = hsl_val(p, q, h);
    uint8_t b = hsl_val(p, q, h - 1/3.0f);

    return UNSAFE_PACK(r, g, b, 255); // not mainstream renderer.h PACK, needs refactor
}
