#ifndef VECTOR_H
#define VECTOR_H
#include <cmath>

class Vector4
{
public:
    float x, y, z, w;

    Vector4 operator-(const Vector4& other) const
    {
        return Vector4(x - other.x, y - other.y, z - other.z, w - other.w);
    }
    Vector4 operator+(const Vector4& other) const
    {
        return Vector4(x + other.x, y + other.y, z + other.z, w + other.w);
    }
    Vector4 operator*(const float delta) const
    {
        return Vector4(x * delta, y * delta, z * delta, w * delta);
    }
};

class Vector3
{
public:
    float x, y, z;

    Vector3(float x = 0, float y = 0, float z = 0);
    float dot(const Vector3& other);
    Vector3 cross(const Vector3& b);
    Vector3 operator-(const Vector3& other) const {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }
    Vector3 operator+(const Vector3& other) const {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }
    Vector3 operator*(const float delta) const {
        return Vector3(x * delta, y * delta, z * delta);
    }
    bool operator!=(const Vector3& other) const {
        return x != other.x || y != other.y || z != other.z;
    }

    Vector3 operator/(const float delta) const {
        return Vector3(x / delta, y / delta, z / delta);
    }

     // Normalizing a vector
    void new_normalize() {
        float length = sqrt(x * x + y * y + z * z);
        x /= length;
        y /= length;
        z /= length;
    }

    static Vector3 multiply(Vector3& a, Vector3& b)
    {
        return Vector3(a.x * b.x, a.y * b.y, a.z * b.z);
    }

    static Vector3 new_cross(const Vector3& a, const Vector3& b)
    {
        return Vector3(a.y * b.z - a.z * b.y,
                       a.z * b.x - a.x * b.z,
                       a.x * b.y - a.y * b.x);
    }
    static float new_dot(const Vector3& a, const Vector3& b)
    {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    static Vector3 lerp(const Vector3& start, const Vector3& end, float t)
    {
        return Vector3(
            start.x + (end.x - start.x) * t,
            start.y + (end.y - start.y) * t,
            start.z + (end.z - start.z) * t
        );

    }

    static float angle_diff(Vector3 a, Vector3 b)
    {
        float dot_prod = a.normalized().dot(b.normalized());
        return acos(dot_prod);
    }

    static Vector3 slerp(Vector3 start, Vector3 end, float percent) {
        float dot = start.normalized().dot(end.normalized());
        dot = fmax(fmin(dot, 1.0f), -1.0f); // Clamp dot product to be within acos range
        float theta = acos(dot) * percent;
        Vector3 relativeVec = (end - start * dot).normalized(); // Orthogonal vector
        return start * cos(theta) + relativeVec * sin(theta);
    }

    
// Function to rotate a vector around an axis by a given angle
static Vector3 rotateAroundAxis(const Vector3& v, const Vector3& axis, float angle) {
    // Normalize the axis
    float axisLength = sqrt(axis.x * axis.x + axis.y * axis.y + axis.z * axis.z);
    Vector3 unitAxis = {axis.x / axisLength, axis.y / axisLength, axis.z / axisLength};

    // Calculate the cross product (v x axis)
    Vector3 crossProduct = {
        v.y * unitAxis.z - v.z * unitAxis.y,
        v.z * unitAxis.x - v.x * unitAxis.z,
        v.x * unitAxis.y - v.y * unitAxis.x
    };

    // Calculate the dot product (v . axis)
    float dotProduct = v.x * unitAxis.x + v.y * unitAxis.y + v.z * unitAxis.z;

    // Calculate the rotated vector using Rodrigues' rotation formula
    Vector3 part1 = {v.x * cos(angle), v.y * cos(angle), v.z * cos(angle)};
    Vector3 part2 = {crossProduct.x * sin(angle), crossProduct.y * sin(angle), crossProduct.z * sin(angle)};
    Vector3 part3 = {unitAxis.x * dotProduct * (1 - cos(angle)),
                     unitAxis.y * dotProduct * (1 - cos(angle)),
                     unitAxis.z * dotProduct * (1 - cos(angle))};

    // Combine all parts to get the final rotated vector
    Vector3 rotatedVector = {part1.x + part2.x + part3.x, part1.y + part2.y + part3.y, part1.z + part2.z + part3.z};

    return rotatedVector;
}

    float magnitude();
    Vector3 normalized();
};

#endif
