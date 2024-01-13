#include <cstdlib>
#include <stdlib.h>
#include <time.h>

#include "Utils.h"

const int Utils::randBetween(int min, int max) { return rand() % (max - min) + min; }

const sf::Uint8 Utils::toUint8(int number) { return static_cast<sf::Uint8>(number); }
