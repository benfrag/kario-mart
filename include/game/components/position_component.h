#ifndef POSITION_COMPONENT_H
#define POSITION_COMPONENT_H

#include "engine/math/vector.h"

struct PositionComponent
{
    Vector3 position;
    Vector3 rotation;

    bool operator!=(const PositionComponent& other) const {
        return position != other.position; // Assuming Vector3 has a != operator defined
    }
};

#endif
