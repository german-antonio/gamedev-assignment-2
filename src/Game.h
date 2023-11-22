#pragma once

#include <SFML/Graphics.hpp>

#include "Entity.h"
#include "EntityManager.h"

struct PlayerConfig
{
  int SR, CR, FR, FG, FB, OR, OG, OB, OT, V;
  float S;
};

struct EnemyConfig
{
  int SR, CR, OR, OG, OB, OT, VMIN, VMAX, L, SI;
  float SMIN, SMAX;
};

struct BulletConfig
{
  int SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L;
  float S;
};

class Game
{
  /* Relevant global Game data */
  sf::RenderWindow m_window; // the window we will draw to
  EntityManager m_entities;  // vector of entities to maintain
  sf::Font m_font;           // the font we will use to draw
  sf::Text m_scoreText;      // the score text to be drawn to the screen
  PlayerConfig m_playerConfig;
  EnemyConfig m_enemyConfig;
  BulletConfig m_bulletConfig;
  Vec2 m_lastMousePos = {0.0f, 0.0f};
  int m_score = 0;
  int m_fontSize = 32;
  int m_currentFrame = 0;
  int m_lastEnemySpawnTime = 0;
  int m_lastPlayerBulletSpawnTime = 0;
  bool m_paused = false;   // wether we update game logic
  bool m_running = true;   // wether the game is running
  bool m_gameOver = false; // wether the game is over
  bool m_reset = false;    // wether a game reset is due
  bool m_debug = false;    // wether debug mode is active

  std::shared_ptr<Entity> m_player;

  /* Game Systems */
  void init(const std::string& config); // initialize the GameState with a config
  void setPaused(bool paused);          // pause the game

  void sReset();                // System: Reset the game
  void sEnemySpawner();         // System: Spawn Enemies
  void sPlayerBulletSpawner();  // System: Spawn Player Bullets
  void sLifespan();             // System: Lifespan
  void sMovement();             // System: Entity position / movement update
  void sCollision();            // System: Collisions
  void sUserInput();            // System: User Input
  void sUpdatePlayerVelocity(); // System: Update player velocity vector
  void sRenderGameOver();       // System: Render / Game Over
  void sRender();               // System: Render / Drawing

  void reset();
  void spawnPlayer();
  void spawnEnemy();
  void spawnSmallenemies(std::shared_ptr<Entity> entity);
  void spawnBullet(std::shared_ptr<Entity> entity, const Vec2& mousePos);
  void spawnSpecialWeapon(std::shared_ptr<Entity> entity);

  void resolveKeyPressedAction(sf::Keyboard::Key key);
  void resolveKeyReleasedAction(sf::Keyboard::Key key);
  void resolveMouseButtonPressedAction(sf::Event::MouseButtonEvent mouseButton);
  void resolveMouseMoveAction(sf::Event::MouseMoveEvent mouseMove);
  void resolveMouseButtonReleasedAction(sf::Event::MouseButtonEvent mouseButton);
  bool collides(Vec2 pos1, Vec2 pos2, float totalRadius);

public:
  Game(const std::string& config); // constructor, takes in game config

  void run();
};