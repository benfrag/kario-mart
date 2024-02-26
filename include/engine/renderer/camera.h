#ifndef CAMERA_H
#define CAMERA_H

#include "engine/math/matrix.h"
#include "engine/math/vector.h"

#include <windows.h>

class Camera
{
public:
    Vector3 position;
    Vector3 look_at;
    Vector3 up_direction;
    Matrix4 view_matrix;
    Matrix4 projection_matrix;
    Matrix4 view_projection_matrix;

    Camera(Vector3 position, Vector3 look_at, Vector3 up_direction);
};

#endif
