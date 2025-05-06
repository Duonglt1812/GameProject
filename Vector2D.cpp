#include "Vector2D.h"
#include <cmath>

Vector2D::Vector2D() : x(0), y(0) {}
Vector2D::Vector2D(float x_, float y_) : x(x_), y(y_) {}

Vector2D& Vector2D::operator+=(const Vector2D& v) {
    x += v.x;
    y += v.y;
    return *this;
}

Vector2D& Vector2D::operator-=(const Vector2D& v) {
    x -= v.x;
    y -= v.y;
    return *this;
}

Vector2D Vector2D::operator+(const Vector2D& v) const {
    return Vector2D(x + v.x, y + v.y);
}

Vector2D Vector2D::operator-(const Vector2D& v) const {
    return Vector2D(x - v.x, y - v.y);
}

Vector2D Vector2D::operator*(float scalar) const {
    return Vector2D(x * scalar, y * scalar);
}

Vector2D Vector2D::operator/(float scalar) const {
    if (scalar == 0) {
        return Vector2D(x, y);
    }
    return Vector2D(x / scalar, y / scalar);
}

float Vector2D::magnitude() const {
    return sqrt(x * x + y * y);
}

std::ostream& operator<<(std::ostream& os, const Vector2D& v) {
    os << "(" << v.x << ", " << v.y << ")";
    return os;
}
