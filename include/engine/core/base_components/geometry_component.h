#ifndef GEOMETRY_COMPONENT_H
#define GEOMETRY_COMPONENT_H

#include "engine/math/vector.h"
#include "engine/core/base_components/transform_component.h"
#include <vector>

struct GeometryComponent
{
    std::vector<Vector3> vertices;
    std::vector<Vector3> render_vertices;
    TransformComponent old_transform{{0, 0, 0}};
    bool has_init = false;
    uint32_t clr;
};

#endif
