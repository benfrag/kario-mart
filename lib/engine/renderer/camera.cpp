#include "engine/renderer/camera.h"
#include <cmath>
#include <iostream>
#include <windows.h>

Camera::Camera(Vector3 position, Vector3 look_at, Vector3 up_direction) : position(position), look_at(look_at), up_direction(up_direction)
{
}
