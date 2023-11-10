#include <math.h>

class Vec2
{
public:
  float x = 0;
  float y = 0;

  Vec2() {}
  Vec2(float xin, float yin) : x(xin), y(yin) {}

  bool operator==(Vec2& rhs) { return (x == rhs.x) && (y == rhs.y); }

  bool operator!=(Vec2& rhs) { return (x != rhs.x) || (y != rhs.y); }

  Vec2 operator+(Vec2& rhs) { return Vec2(x + rhs.x, y + rhs.y); }
  Vec2 operator-(Vec2& rhs) { return Vec2(x - rhs.x, y - rhs.y); }
  Vec2 operator*(float s) { return Vec2(x * s, y * s); }
  Vec2 operator/(float s) { return Vec2(x / s, y / s); }

  void operator+=(const Vec2& rhs)
  {
    x = x + rhs.x;
    y = y + rhs.y;
  }

  void operator-=(const Vec2& rhs)
  {
    x = x - rhs.x;
    y = y - rhs.y;
  }

  void operator*=(const float val)
  {
    x *= val;
    y *= val;
  }

  void operator/=(const float val)
  {
    x /= val;
    y /= val;
  }

  float length() { return sqrtf((x * x) + (y * y)); }
  float dist(Vec2& rhs) { return (rhs - *this).length(); }
};