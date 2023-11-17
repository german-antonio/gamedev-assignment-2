#include <cstdlib>

#include "Utils.h"

Utils::Utils() {};

const int Utils::randBetween(int min, int max) { return (rand() % (max - min)) + min; }