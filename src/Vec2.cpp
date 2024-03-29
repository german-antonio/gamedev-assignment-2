#include "Vec2.h"
#include <math.h>

Vec2::Vec2() {}
Vec2::Vec2(float xin, float yin) : x(xin), y(yin) {}

bool Vec2::operator==(const Vec2& rhs) const { return (x == rhs.x) && (y == rhs.y); }

bool Vec2::operator!=(const Vec2& rhs) const { return (x != rhs.x) || (y != rhs.y); }

Vec2 Vec2::operator+(const Vec2& rhs) const { return Vec2(x + rhs.x, y + rhs.y); }
Vec2 Vec2::operator-(const Vec2& rhs) const { return Vec2(x - rhs.x, y - rhs.y); }
Vec2 Vec2::operator*(const float s) const { return Vec2(x * s, y * s); }
Vec2 Vec2::operator/(const float s) const { return Vec2(x / s, y / s); }

void Vec2::operator+=(const Vec2& rhs)
{
  x = x + rhs.x;
  y = y + rhs.y;
}

void Vec2::operator-=(const Vec2& rhs)
{
  x = x - rhs.x;
  y = y - rhs.y;
}

void Vec2::operator*=(const float val)
{
  x *= val;
  y *= val;
}

void Vec2::operator/=(const float val)
{
  x /= val;
  y /= val;
}

float Vec2::dist(const Vec2& rhs) const { return (rhs - *this).length(); }
float Vec2::length() const { return sqrtf((x * x) + (y * y)); }