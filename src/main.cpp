#include "Game.h"

#include <SFML/Graphics.hpp>

int main()
{
  Game g("config.txt");
  g.run();
}