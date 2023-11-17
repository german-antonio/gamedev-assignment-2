#include <cstdlib>

#include "Utils.h"

const int Utils::randBetween(int min, int max) { return (rand() % (max - min)) + min; }