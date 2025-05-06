#pragma once
#include <iostream>

class Vector2D {
public:
    float x;
    float y;

    Vector2D();
    Vector2D(float x_, float y_);

    Vector2D& operator+=(const Vector2D& v);
    Vector2D& operator-=(const Vector2D& v);
    Vector2D operator+(const Vector2D& v) const;
    Vector2D operator-(const Vector2D& v) const;
    Vector2D operator*(float scalar) const;
    Vector2D operator/(float scalar) const;

    float magnitude() const;

    friend std::ostream& operator<<(std::ostream& os, const Vector2D& v);
};
