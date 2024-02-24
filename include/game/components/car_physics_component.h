#ifndef CAR_PHYSICS_COMPONENT_H
#define CAR_PHYSICS_COMPONENT_H

#include "engine/math/vector.h"

struct CarPhysicsComponent
{
    Vector3 acceleration = {0, 0, 0};
    Vector3 velocity = {0, 0, 0};
};

#endif
