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

    float magnitude();
    Vector3 normalized();
};

#endif
