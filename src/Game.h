#pragma once

#include <SFML/Graphics.hpp>

#include "Entity.h"
#include "EntityManager.h"

// Forward declarations prevent circular dependencies and declarations order problems
struct WindowConfig;
struct FontConfig;
struct PlayerConfig;
struct EnemyConfig;
struct BulletConfig;

struct WindowConfig
{
  int width, height, frameLimit, fullScreen;
};

struct FontConfig
{
  std::string path;
  int sizeL, sizeM, sizeS;
  sf::Color color;
};

struct PlayerConfig
{
  int shapeRadius, collisionRadius, outlineThickness, vertices;
  sf::Color fillColor, outlineColor;
  float maxSpeed;
};

struct EnemyConfig
{
  int shapeRadius, collisionRadius, outlineThickness, minVertices, maxVertices, spawnInterval;
  sf::Color fillColor, outlineColor;
  float minSpeed, maxSpeed;
};

struct BulletConfig
{
  int shapeRadius, collisionRadius, outlineThickness, vertices, lifespan, rate;
  sf::Color fillColor, outlineColor;
  float speed;
};

struct GameConfig
{
  WindowConfig window;
  FontConfig font;
  PlayerConfig player;
  EnemyConfig enemy;
  BulletConfig bullet;
};

class Game
{
  /* Relevant global Game data */
  sf::RenderWindow m_window; // the window we will draw to
  EntityManager m_entities;  // vector of entities to maintain
  sf::Font m_font;           // the font we will use to draw
  sf::Text m_scoreText;      // the score text to be drawn to the screen
  GameConfig m_config;
  sf::Cursor m_cursor;
  sf::Cursor m_crosshair;
  Vec2 m_lastMousePos = {0.0f, 0.0f};
  float m_maxPlayerAngle = 10.0f;
  float m_angleTransform = 0.5f;
  int m_score = 0;
  int m_currentFrame = 0;
  int m_lastEnemySpawnTime = 0;
  int m_lastPlayerBulletSpawnTime = 0;
  int m_score = 0;
  bool m_paused = false;   // wether we update game logic
  bool m_running = true;   // wether the game is running
  bool m_gameOver = false; // wether the game is over
  bool m_reset = false;    // wether a game reset is due
  bool m_debug = false;    // wether debug mode is active

  std::shared_ptr<Entity> m_player;

  void init(const std::string& config);            // initialize the GameState with a config
  void setConfigFromFile(const std::string& path); // read a set of config values from a text file
  void setPaused(bool paused);                     // pause the game

  void reset();
  void spawnPlayer();
  void spawnEnemy();
  void spawnSmallenemies(std::shared_ptr<Entity> entity);
  void spawnBullet(std::shared_ptr<Entity> entity, const Vec2& mousePos);
  void spawnSpecialWeapon(std::shared_ptr<Entity> entity);
  void renderGameOver();
  void renderScore();
  void renderEntities();
  void resolveKeyPressedAction(sf::Keyboard::Key key);
  void resolveKeyReleasedAction(sf::Keyboard::Key key);
  void resolveMouseButtonPressedAction(sf::Event::MouseButtonEvent mouseButton);
  void resolveMouseMoveAction(sf::Event::MouseMoveEvent mouseMove);
  void resolveMouseButtonReleasedAction(sf::Event::MouseButtonEvent mouseButton);
  bool collides(Vec2 pos1, Vec2 pos2, float totalRadius);

public:
  Game(const std::string& config); // constructor, takes in game config
  void run();

  /* Game Systems */
  void sReset();                // System: Reset the game
  void sEnemySpawner();         // System: Spawn Enemies
  void sPlayerBulletSpawner();  // System: Spawn Player Bullets
  void sLifespan();             // System: Lifespan
  void sMovement();             // System: Entity position / movement update
  void sCollision();            // System: Collisions
  void sUserInput();            // System: User Input
  void sUpdatePlayerVelocity(); // System: Update player velocity vector
  void sRender();               // System: Render / Drawing
};