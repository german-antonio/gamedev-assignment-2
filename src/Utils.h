#pragma once
#include <SFML/Graphics.hpp>

namespace Utils
{
static bool seeded = false;

const int randBetween(int min, int max);
const sf::Uint8 toUint8(int number);
}; // namespace Utils