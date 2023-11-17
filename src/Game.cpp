#include "Game.h"
#include "Utils.h"

#include <fstream>
#include <iostream>
#include <math.h>

Game::Game(const std::string& config) { init(config); }

void Game::init(const std::string& path)
{
  // TODO : read in config file here
  //        use the premade PlayerConfig, EnemyConfig, BulletConfig variables

  //  std::ifstream fin(path);
  // fin >> m_playerConfig.SR >> m_playerConfig.CR >>

  // set up default window parameters
  m_window.create(sf::VideoMode(1280, 720), "Assignment II");
  m_window.setFramerateLimit(60);

  spawnPlayer();
}

void Game::run()
{
  // TODO: add pause functionality in here
  //       some systems should function while paused (e.g. rendering)
  //       some systems shouldn't (e.g. movement, input)
  while (m_running)
  {
    m_entities.update();

    sEnemySpawner();
    sLifespan();
    sMovement();
    sCollision();
    sUserInput();
    sUpdatePlayerVelocity();
    sRender();

    // increment the current frame
    // may need to be moved when pause implemented
    m_currentFrame++;
  }
}

void Game::setPaused(bool paused) { m_paused = paused; }

void Game::spawnPlayer()
{
  // TODO: Finish adding all properties of the player with the correct values from the config

  // We create every entity by calling EntityManager.addEntity(tag)
  // This returns a std::shared_ptr<Entity>, so we use 'auto' to save typing
  auto entity = m_entities.addEntity("player");

  // Give this entity a transform so it spawns at (200,200) with velocity (1,1) and angle 0
  // spawn at the center
  float mx = m_window.getSize().x / 2.0f;
  float my = m_window.getSize().y / 2.0f;
  entity->cTransform = std::make_shared<CTransform>(Vec2(mx, my), Vec2(0.0f, 0.0f), 0.0f);

  // The entity's shape will have a radius 32, 8 sides, dark grey fill, and red outline of thickness 4
  // entity->cShape = std::make_shared<CShape>(m_playerConfig.SR, m_playerConfig.V, ...);
  entity->cShape = std::make_shared<CShape>(32.0f, 8, sf::Color(10, 10, 10), sf::Color(255, 0, 0), 4.0f);

  // Add an input component to the player so that we can use inputs
  entity->cInput = std::make_shared<CInput>();

  // Since we want this Entity to be our player, set our Game's player variable to be this Entity
  // This goes slightly against the EntityManager paradigm, but we use the player so much it's worth it
  m_player = entity;
}

void Game::spawnEnemy()
{
  // TODO: make sure the enemy is spawned properly with the m_enemyConfig variables
  //       the enemy must be spawned completely within the bounds of the window

  auto entity = m_entities.addEntity("enemy");

  // spawn at random point
  int radius = 16;
  float thickness = 4.0f;
  int offset = radius + thickness;
  int maxX = m_window.getSize().x - offset;
  int maxY = m_window.getSize().y - offset;
  float rndX = Utils::randBetween(offset, maxX);
  float rndY = Utils::randBetween(offset, maxY);
  float speed = Utils::randBetween(2, 6);
  int points = Utils::randBetween(3, 8);

  std::cout << "Enemy will be spawned at: (" << rndX << "," << rndY << ")" << std::endl;

  // set position
  Vec2 position(rndX, rndY);
  // generate random angle
  float angle = (rand() % 361);
  // calculate velocity vector
  Vec2 velocity((speed * cos(angle)), (speed * sin(angle)));

  // Set transform component
  entity->cTransform = std::make_shared<CTransform>(position, velocity, angle);

  // The entity's shape will have a radius 16, 3 sides, blue fill, and white outline of thickness 4
  // TODO: entity->cShape = std::make_shared<CShape>(m_playerConfig.SR, m_playerConfig.V, ...);
  entity->cShape = std::make_shared<CShape>(radius, points, sf::Color(0, 0, 255), sf::Color(255, 255, 255), thickness);

  // record when the most recent enemy was spawned
  m_lastEnemySpawnTime = m_currentFrame;
}

void Game::spawnSmallenemies(std::shared_ptr<Entity> e)
{
  // TODO: spawn small enemies at the location of the input enemy e

  // when we create the smaller enemy, we have to read the values of the original enemy
  // - spawn a number of small enemies equal to the vertices of the original enemy
  // - set each small enemy to the same color as the original, half the size
  // - small enemies are worth double points of the original enemy
}

void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2& target)
{
  // TODO: Implement the spawning of a bullet which travels toward target
  //       - bullet speed is given as a scalar speed
  //       - you must set the velocity by using formula in notes
  auto bullet = m_entities.addEntity("bullet");

  // TODO: give the bullet all of its properties (read from config file)
  float speed = 10;

  // so we have two vectors: playerPos and targetPos
  Vec2 playerPos(m_player->cTransform->pos.x, m_player->cTransform->pos.y);
  Vec2 targetPos(target.x, target.y);

  // we need to get the difference (distance) between both vectors
  Vec2 diff = targetPos - playerPos;
  // now we can use archtangent to get the angle
  float angle = atan2f(diff.y, diff.x);
  // calculate velocity vector
  Vec2 velocity((speed * cos(angle)), (speed * sin(angle))); // TODO: somewhow actually understand this

  // Set transform component
  bullet->cTransform = std::make_shared<CTransform>(playerPos, velocity, angle);
  bullet->cShape = std::make_shared<CShape>(10, 8, sf::Color(255, 255, 255), sf::Color(255, 0, 0), 2);
  bullet->cLifespan = std::make_shared<CLifespan>(50);
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity)
{
  // TODO: implement your own special weapon
}

void Game::sMovement()
{
  // TODO: implement all entity movement in this function
  //       you should read the m_player->cInput component to determine if the player is moving

  // UPDATE TO THE ABOVE:
  // Personal note: we might not really do that, we are determining stuff based on cInput on a
  // separate system function Game::sUpdatePlayerVelocity()
  //
  // We only move all the stuff from here, nothing else
  for (auto& e : m_entities.getEntities())
  {
    e->cTransform->pos.x += e->cTransform->velocity.x;
    e->cTransform->pos.y += e->cTransform->velocity.y;
  }
}

void Game::sLifespan()
{
  // TODO: implement all lifespan functionality

  // for all entities
  for (auto& e : m_entities.getEntities())
  {
    //   if entity has no lifespan component, skip it
    if (!e->cLifespan)
      continue;

    std::cout << "Entity " << e->tag() << ": " << e->id() << ", lifespan left " << e->cLifespan->remaining << std::endl;
    //   if entity has > 0 remaining lifespan, subtract 1
    if (e->cLifespan->remaining > 0)
    {
      std::cout << "Entity " << e->id() << " gets lifespan reduced by 1" << std::endl;
      e->cLifespan->remaining -= 1;
    }
    //   if it has lifespan and is alive
    // if (e->cLifespan && e->isActive())
    //     scale its alpha channel properly
    // e->cShape->circle.setFillColor()

    // if it has lifespan and its time is up
    // destroy the entity
    else
    {
      std::cout << "Entity " << e->id() << " is being destroyed." << std::endl;
      e->destroy();
    }
  }
}

void Game::sCollision()
{
  // TODO: implement all proper collisions between entities
  //       be sure to use the collision radius, NOT the shape radius
}

void Game::sEnemySpawner()
{
  int spawnRate = 100; // TODO: Read from config file
  if ((m_currentFrame - m_lastEnemySpawnTime) >= spawnRate)
    spawnEnemy();
}

void Game::sUpdatePlayerVelocity()
{
  m_player->cTransform->velocity = {0, 0}; // TODO: make it deaccelerate like greenberry did

  if (m_player->cInput->up)
    m_player->cTransform->velocity.y = -3;

  if (m_player->cInput->down)
    m_player->cTransform->velocity.y = 3;

  if (m_player->cInput->left)
    m_player->cTransform->velocity.x = -3;

  if (m_player->cInput->right)
    m_player->cTransform->velocity.x = 3;
}

void Game::sRender()
{
  // TODO: change the code below to draw ALL of the entities
  //       sample drawing of the player Entity that we have created
  m_window.clear();

  // set the position of the shape based on the entity's transform->pos
  // m_player->cShape->circle.setPosition(m_player->cTransform->pos.x, m_player->cTransform->pos.y);

  // set the rotation of the shape based on the entity's transform->angle
  m_player->cTransform->angle += 1.0f;
  // m_player->cShape->circle.setRotation(m_player->cTransform->angle);

  // draw the entity's sf::CircleShape
  // m_window.draw(m_player->cShape->circle);

  for (auto& e : m_entities.getEntities())
  {
    if (!e->isActive())
    {
      std::cout << "Skipping dead " << e->tag() << " entity (" << e->id() << ")" << std::endl;
      continue;
    }

    // std::cout << "Drawing " << e->tag() << " at: (" << e->cShape->circle.getPosition().x << ","
    //           << e->cShape->circle.getPosition().y << ")" << std::endl;

    e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);
    e->cTransform->angle += 1.0f;
    e->cShape->circle.setRotation(e->cTransform->angle);
    m_window.draw(e->cShape->circle);
  }

  m_window.display();
};

void Game::sUserInput()
{
  // TODO: handle user input here
  //       note that you should only be setting the player's input component variables here
  //       you should not implement the player's movement logic here
  //       the movement system will read the variables you set in this function
  sf::Event event;
  while (m_window.pollEvent(event))
  {
    // this event triggers when the window is closed
    if (event.type == sf::Event::Closed)
      m_running = false;

    // Keyboard input handling
    if (event.type == sf::Event::KeyPressed)
      resolveKeyPressedAction(event.key.code);
    if (event.type == sf::Event::KeyReleased)
      resolveKeyReleasedAction(event.key.code);

    // Mouse input handling
    if (event.type == sf::Event::MouseButtonPressed)
      resolveMouseButtonPressedAction(event.mouseButton);
  }
}

void Game::resolveKeyPressedAction(sf::Keyboard::Key key)
{
  switch (key)
  {
  case sf::Keyboard::Up:
  case sf::Keyboard::W:
    m_player->cInput->up = true;
    break;

  case sf::Keyboard::Down:
  case sf::Keyboard::S:
    m_player->cInput->down = true;
    break;

  case sf::Keyboard::Left:
  case sf::Keyboard::A:
    m_player->cInput->left = true;
    break;

  case sf::Keyboard::Right:
  case sf::Keyboard::D:
    m_player->cInput->right = true;
    break;

  case sf::Keyboard::R:
    m_gameOver = false;
    // TODO: implement proper reset behaviour
    break;

    // case sf::Keyboard::S:
    //   stop(true);
    //   break;

  case sf::Keyboard::X:
    m_gameOver = true;
    break;

  case sf::Keyboard::F1:
    m_debug = !m_debug;
    break;

  case sf::Keyboard::Escape:
    m_window.close();
    break;

  default:
    break;
  }
}

void Game::resolveKeyReleasedAction(sf::Keyboard::Key key)
{
  std::cout << "Key released with code = " << key << std::endl;

  switch (key)
  {
  case sf::Keyboard::Up:
  case sf::Keyboard::W:
    m_player->cInput->up = false;
    break;

  case sf::Keyboard::Down:
  case sf::Keyboard::S:
    m_player->cInput->down = false;
    break;

  case sf::Keyboard::Left:
  case sf::Keyboard::A:
    m_player->cInput->left = false;
    break;

  case sf::Keyboard::Right:
  case sf::Keyboard::D:
    m_player->cInput->right = false;
    break;

  default:
    break;
  }
}

void Game::resolveMouseButtonPressedAction(sf::Event::MouseButtonEvent mouse)
{
  // TODO: Implement actual logic from mouse pressed actions
  if (mouse.button == sf::Mouse::Left)
  {
    std::cout << "Mouse L Clicked at (" << mouse.x << "," << mouse.y << ")" << std::endl;
    // call spawnBullet here
    spawnBullet(m_player, Vec2(mouse.x, mouse.y));
  }

  if (mouse.button == sf::Mouse::Right)
    // call spawnSpecialWeapon here
    std::cout << "Mouse R Clicked at (" << mouse.x << "," << mouse.y << ")" << std::endl;
}