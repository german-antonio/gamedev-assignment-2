#pragma once

#include "Vec2.h"
#include "cmath"
#include <SFML/Graphics.hpp>

class CTransform
{
public:
  // Position
  Vec2 pos = {0.0, 0.0};
  Vec2 maxPos = {0.0, 0.0};
  Vec2 minPos = {0.0, 0.0};
  // Velocity
  Vec2 velocity = {0.0, 0.0};
  Vec2 maxVel = {0.0, 0.0};
  Vec2 minVel = {0.0, 0.0};
  // Acceleration
  Vec2 acceleration = {0.0, 0.0};
  float accelerationStep = 0.0f;
  // Others
  float angle = 0;

  CTransform(const Vec2& p, const Vec2& v, float a) : pos(p), velocity(v), angle(a) {}
  CTransform(const Vec2& p, const Vec2& maxp, const Vec2& minp, const Vec2& v, const Vec2& maxv, const Vec2& minv,
             const Vec2& acc, float step, float a)
      : pos(p), maxPos(maxp), minPos(minp), velocity(v), maxVel(maxv), minVel(minv), acceleration(acc),
        accelerationStep(step), angle(a)
  {
  }

  float getVelScalar() { return std::hypot(velocity.x, velocity.y); }
  float getAccScalar() { return std::hypot(acceleration.x, acceleration.y); }
};

class CShape
{
public:
  sf::CircleShape circle;

  CShape(float radius, int points, const sf::Color& fill, const sf::Color& outline, float thickness) : circle(radius, points)
  {
    circle.setFillColor(fill);
    circle.setOutlineColor(outline);
    circle.setOutlineThickness(thickness);
    circle.setOrigin(radius, radius);
  }
};
class CEdge
{
public:
  int factor = 0;
  CEdge(int f) : factor(f) {}
};
class CCollision
{
public:
  float radius = 0;
  CCollision(float r) : radius(r) {}
};
class CScore
{
public:
  int score = 0;
  CScore(int s = 1) : score(s) {}
};
class CInput
{
public:
  bool up = false;
  bool left = false;
  bool right = false;
  bool down = false;
  bool shoot = false;
  bool debug = false;
  CInput() {}
};
class CSpecial
{
public:
  int duration = 300;  // frames
  int cooldown = 1800; // frames
  bool enabled = true;
  bool active = false;
  CSpecial() {}
  CSpecial(int d, int cd) : duration(d), cooldown(cd) {}
};
class CLifespan
{
public:
  int remaining = 0;
  int total = 0;
  CLifespan(int total) : remaining(total), total(total) {}
};

class CFont
{
public:
  sf::Font font;
  int size;
  CFont(sf::Font fin, int sin) : font(fin), size(sin) {}
};

class CSprite
{
public:
  sf::Sprite sprite;
  sf::Texture texture;
  float size;
  CSprite(std::string texturePath, float size) : size(size)
  {
    texture.loadFromFile(texturePath);
    texture.setSmooth(true);
    sprite.setTexture(texture);
    sprite.setOrigin(size / 2, size / 2);
  }
};