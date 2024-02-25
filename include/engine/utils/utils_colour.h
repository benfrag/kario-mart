#ifndef UTILS_COLOUR_H
#define UTILS_COLOUR_H

#include <cstdint>

struct HSL
{
    float h, s, l;
};

struct RGB 
{
    float r, g, b;
};

// standard lib min max windows.h always giving issues on clang
#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))

HSL rgb_to_hsl(RGB col);
uint32_t hsl_to_packed_rgb(HSL col);

#endif
