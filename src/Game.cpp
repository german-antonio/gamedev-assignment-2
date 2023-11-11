#include "Game.h"

#include <fstream>
#include <iostream>

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
    sMovement();
    sCollision();
    sUserInput();
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
  entity->cTransform = std::make_shared<CTransform>(Vec2(mx, my), Vec2(1.0f, 1.0f), 0.0f);

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

  // Give this entity a transform so it spawns at (200,200) with velocity (1,1) and angle 0
  // spawn at the center
  float ex = rand() % m_window.getSize().x; // TODO: this must be fixed to prevent spawning colliding with edges
  float ey = rand() % m_window.getSize().y;

  entity->cTransform = std::make_shared<CTransform>(Vec2(ex, ey), Vec2(1.0f, 1.0f), 0.0f);

  // The entity's shape will have a radius 32, 8 sides, dark grey fill, and red outline of thickness 4
  // entity->cShape = std::make_shared<CShape>(m_playerConfig.SR, m_playerConfig.V, ...);
  entity->cShape = std::make_shared<CShape>(16.0f, 3, sf::Color(0, 0, 255), sf::Color(255, 255, 255), 4.0f);

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
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity)
{
  // TODO: implement your own special weapon
}

void Game::sMovement()
{
  // TODO: implement all entity movement in this function
  //       you should read the m_player->cInput component to determine if the player is moving

  // sample movement speed update
  m_player->cTransform->pos.x += m_player->cTransform->velocity.x;
  m_player->cTransform->pos.y += m_player->cTransform->velocity.y;
}

void Game::sLifespan()
{
  // TODO: implement all lifespan functionality

  // for all entities
  //   if entity has no lifespan component, skip it
  //   if entity has > 0 remaining lifespan, subtract 1
  //   if it has lifespan and is alive
  //     scale its alpha channel properly
  //   if it has lifespan and its time is up
  //     destroy the entity
}

void Game::sCollision()
{
  // TODO: implement all proper collisions between entities
  //       be sure to use the collision radius, NOT the shape radius
}

void Game::sEnemySpawner()
{
  // TODO: code which implements enemy spawning should go here

  //       use (m_currentFrame - m_lastEnemySpawnTime) to determine
  //       how long it has been since the last enemy spawned
  if ((m_currentFrame - m_lastEnemySpawnTime) >= 100)
    spawnEnemy();
}

void Game::sRender()
{
  // TODO: change the code below to draw ALL of the entities
  //       sample drawing of the player Entity that we have created
  m_window.clear();

  // set the position of the shape based on the entity's transform->pos
  m_player->cShape->circle.setPosition(m_player->cTransform->pos.x, m_player->cTransform->pos.y);

  // set the rotation of the shape based on the entity's transform->angle
  m_player->cTransform->angle += 1.0f;
  m_player->cShape->circle.setRotation(m_player->cTransform->angle);

  // draw the entity's sf::CircleShape
  m_window.draw(m_player->cShape->circle);

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
    m_player->cInput->up = true;
    break;

  case sf::Keyboard::Down:
    m_player->cInput->down = true;
    break;

  case sf::Keyboard::Left:
    m_player->cInput->left = true;
    break;

  case sf::Keyboard::Right:
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

  case sf::Keyboard::D:
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
    m_player->cInput->up = false;
    break;

  case sf::Keyboard::Down:
    m_player->cInput->down = false;
    break;

  case sf::Keyboard::Left:
    m_player->cInput->left = false;
    break;

  case sf::Keyboard::Right:
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
    std::cout << "Mouse L Clicked at (" << mouse.x << "," << mouse.y << ")" << std::endl;
  // call spawnBullet here
  if (mouse.button == sf::Mouse::Right)
    // call spawnSpecialWeapon here
    std::cout << "Mouse R Clicked at (" << mouse.x << "," << mouse.y << ")" << std::endl;
}