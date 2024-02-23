#ifndef REAL_ROTATION_COMPONENT_H
#define REAL_ROTATION_COMPONENT_H

#include "engine/math/vector.h"

struct RealRotationComponent
{
    Vector3 rotation;

    bool operator!=(const RealRotationComponent& other) const {
        return rotation != other.rotation; // Assuming Vector3 has a != operator defined
    }
};

#endif
