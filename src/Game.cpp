#include "Game.h"
#include "Utils.h"

#include <fstream>
#include <iostream>
#include <math.h>
#include <sstream>

Game::Game(const std::string& config) { init(config); }

void Game::init(const std::string& path)
{
  setConfigFromFile(path);

  if (!m_font.loadFromFile(m_config.font.path))
  {
    std::cerr << "Could not load game font." << std::endl;
    exit(-1);
  }

  m_window.create(sf::VideoMode(m_config.window.width, m_config.window.height), "Assignment II");
  m_window.setFramerateLimit(m_config.window.frameLimit);

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
    sUserInput();
    sReset();

    if (!m_gameOver)
    {
      sEnemySpawner();
      sPlayerBulletSpawner();
      sLifespan();
      sMovement();
      sCollision();
      sUpdatePlayerVelocity(); // TODO: figure out if this is actually a good idea
    }

    sRender();

    // increment the current frame
    // may need to be moved when pause implemented
    m_currentFrame++;
  }
}

void Game::setConfigFromFile(const std::string& path)
{
  std::ifstream fin(path);
  fin >>
      // Window config
      m_config.window.width >> m_config.window.height >> m_config.window.frameLimit >> m_config.window.fullScreen >>
      // Font config
      m_config.font.path >> m_config.font.size >> m_config.font.color.red >> m_config.font.color.green >>
      m_config.font.color.blue >>
      // Player config
      m_config.player.shapeRadius >> m_config.player.collisionRadius >> m_config.player.maxSpeed >>
      m_config.player.fillColor.red >> m_config.player.fillColor.green >> m_config.player.fillColor.blue >>
      m_config.player.outlineColor.red >> m_config.player.outlineColor.green >> m_config.player.outlineColor.blue >>
      m_config.player.outlineThickness >> m_config.player.vertices >>
      // Enemy config
      m_config.enemy.shapeRadius >> m_config.enemy.collisionRadius >> m_config.enemy.minSpeed >> m_config.enemy.maxSpeed >>
      m_config.enemy.fillColor.red >> m_config.enemy.fillColor.green >> m_config.enemy.fillColor.blue >>
      m_config.enemy.outlineColor.red >> m_config.enemy.outlineColor.green >> m_config.enemy.outlineColor.blue >>
      m_config.enemy.outlineThickness >> m_config.enemy.minVertices >> m_config.enemy.maxVertices >>
      m_config.enemy.spawnInterval >>
      // Bullet config
      m_config.bullet.shapeRadius >> m_config.bullet.collisionRadius >> m_config.bullet.speed >>
      m_config.bullet.fillColor.red >> m_config.bullet.fillColor.green >> m_config.bullet.fillColor.blue >>
      m_config.bullet.outlineColor.red >> m_config.bullet.outlineColor.green >> m_config.bullet.outlineColor.blue >>
      m_config.bullet.outlineThickness >> m_config.bullet.vertices >> m_config.bullet.lifespan >> m_config.bullet.rate;
}

void Game::setPaused(bool paused) { m_paused = paused; }

void Game::reset()
{
  m_reset = false;
  m_gameOver = false;
  m_killCount = 0;
  m_entities.destroyAll();
  spawnPlayer();
}

void Game::spawnPlayer()
{
  // TODO: Finish adding all properties of the player with the correct values from the config

  auto entity = m_entities.addEntity("player");

  float radius = m_config.player.collisionRadius;
  // float thickness = 4.0f;
  // float points = 8;

  // Give this entity a transform so it spawns at (200,200) with velocity (0,0) and angle 0
  // spawn at the center
  float mx = m_window.getSize().x / 2.0f;
  float my = m_window.getSize().y / 2.0f;
  entity->cTransform = std::make_shared<CTransform>(Vec2(mx, my), Vec2(0.0f, 0.0f), 0.0f);
  // Set collision component separately
  entity->cCollision = std::make_shared<CCollision>(radius);

  // The entity's shape will have a radius 32, 8 sides, dark grey fill, and red outline of thickness 4
  // entity->cShape = std::make_shared<CShape>(m_config.player.shapeRadius, m_config.player.V, ...);
  // entity->cShape = std::make_shared<CShape>(radius, points, sf::Color(10, 10, 10), sf::Color(255, 0, 0), thickness);
  entity->cSprite = std::make_shared<CSprite>("./assets/textures/Sam.png", radius * 2);

  // Add an input component to the player so that we can use inputs
  entity->cInput = std::make_shared<CInput>();

  // Since we want this Entity to be our player, set our Game's player variable to be this Entity
  // This goes slightly against the EntityManager paradigm, but we use the player so much it's worth it
  m_player = entity;
}

void Game::spawnEnemy()
{
  auto entity = m_entities.addEntity("enemy");

  // spawn at random point
  int offset = m_config.enemy.shapeRadius + m_config.enemy.outlineThickness;
  int maxX = m_window.getSize().x - offset;
  int maxY = m_window.getSize().y - offset;
  float rndX = Utils::randBetween(offset, maxX);
  float rndY = Utils::randBetween(offset, maxY);
  float speed = Utils::randBetween(m_config.enemy.minSpeed, m_config.enemy.maxSpeed);
  int points = Utils::randBetween(m_config.enemy.minVertices, m_config.enemy.maxVertices);

  std::cout << "Enemy will be spawned at: (" << rndX << "," << rndY << ")" << std::endl;

  // set position
  Vec2 position(rndX, rndY);
  // generate random angle
  float angle = (rand() % 361);
  // calculate velocity vector
  Vec2 velocity((speed * cos(angle)), (speed * sin(angle)));

  // Set transform component with above data
  entity->cTransform = std::make_shared<CTransform>(position, velocity, angle);
  // Set collision component separately
  entity->cCollision = std::make_shared<CCollision>(m_config.enemy.shapeRadius + m_config.enemy.outlineThickness);

  // The entity's shape will have a radius 16, 3 sides, blue fill, and white outline of thickness 4
  // TODO: entity->cShape = std::make_shared<CShape>(m_config.player.shapeRadius, m_config.player.V, ...);
  sf::Color fill(m_config.enemy.fillColor.red, m_config.enemy.fillColor.green, m_config.enemy.fillColor.blue);
  sf::Color outline(m_config.enemy.outlineColor.red, m_config.enemy.outlineColor.green, m_config.enemy.outlineColor.blue);
  entity->cShape =
      std::make_shared<CShape>(m_config.enemy.shapeRadius, points, fill, outline, m_config.enemy.outlineThickness);

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
  auto bullet = m_entities.addEntity("bullet");

  // so we have two vectors: playerPos and targetPos
  Vec2 playerPos(m_player->cTransform->pos.x, m_player->cTransform->pos.y);
  Vec2 targetPos(target.x, target.y);

  // we need to get the difference (distance) between both vectors
  Vec2 diff = targetPos - playerPos;
  // now we can use archtangent to get the angle
  float angle = atan2f(diff.y, diff.x);
  // calculate velocity vector
  Vec2 velocity((m_config.bullet.speed * cos(angle)),
                (m_config.bullet.speed * sin(angle))); // TODO: somewhow actually understand this

  // Set transform component
  bullet->cShape = std::make_shared<CShape>(
      m_config.bullet.shapeRadius, m_config.bullet.vertices,
      sf::Color(m_config.bullet.fillColor.red, m_config.bullet.fillColor.green, m_config.bullet.fillColor.blue),
      sf::Color(m_config.bullet.outlineColor.red, m_config.bullet.outlineColor.green, m_config.bullet.outlineColor.blue),
      m_config.bullet.outlineThickness);
  bullet->cTransform = std::make_shared<CTransform>(playerPos, velocity, angle);
  bullet->cCollision = std::make_shared<CCollision>(m_config.bullet.collisionRadius);
  bullet->cLifespan = std::make_shared<CLifespan>(m_config.bullet.lifespan);

  m_lastPlayerBulletSpawnTime = m_currentFrame;
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity)
{
  // TODO: implement your own special weapon
}

void Game::sReset()
{
  if (m_reset)
    reset();
}

void Game::sMovement()
{
  for (auto& e : m_entities.getEntities())
  {
    e->cTransform->pos.x += e->cTransform->velocity.x;
    e->cTransform->pos.y += e->cTransform->velocity.y;
  }
}

void Game::sLifespan()
{
  for (auto& e : m_entities.getEntities())
  {
    //   if entity has no lifespan component, skip it
    if (!e->cLifespan)
      continue;

    if (e->cLifespan->remaining > 0)
    {
      e->cLifespan->remaining -= 1;
      if (e->cLifespan && e->isActive())
      {
        sf::Color fill(e->cShape->circle.getFillColor());
        sf::Color outline(e->cShape->circle.getOutlineColor());
        int fadeOutRate = 250 / e->cLifespan->total; // 255 is max alpha, 250 prevents dissappearing too soon
        fill.a -= fadeOutRate;
        outline.a -= fadeOutRate;
        e->cShape->circle.setFillColor(fill);
        e->cShape->circle.setOutlineColor(outline);
      }
    }
    else
      e->destroy();
  }
}

bool Game::collides(Vec2 pos1, Vec2 pos2, float totalRadius)
{
  Vec2 origin1(pos1.x, pos1.y);
  Vec2 origin2(pos2.x, pos2.y);

  Vec2 diff = origin1 - origin2;

  float dist = (diff.x * diff.x) + (diff.y * diff.y);

  // if their distance is less than the sum of their radiuses, they are colliding
  return dist < totalRadius * totalRadius;
}

void Game::sCollision()
{
  // TODO: implement all proper collisions between entities
  //       be sure to use the collision radius, NOT the shape radius

  for (auto e : m_entities.getEntities("enemy"))
  {
    // wether current enemy is colliding with player
    if (collides(m_player->cTransform->pos, e->cTransform->pos, e->cCollision->radius + m_player->cCollision->radius))
    {
      m_gameOver = true;
      m_entities.destroyAll();
      break;
    }

    // wether current enemy is colliding with any of the bullets
    for (auto b : m_entities.getEntities("bullet"))
      if (collides(e->cTransform->pos, b->cTransform->pos, e->cCollision->radius + b->cCollision->radius))
      {
        m_killCount++;
        std::cout << "!! ENEMY KILLED. Remaining: (" << m_entities.getEntities("enemy").size() << ")" << std::endl;
        std::cout << "CURRENT KILL COUNT: [" << m_killCount << "]" << std::endl;
        e->destroy();
        b->destroy();
      }

    // wether current enemy is hitting an edge
    if (e->cTransform->pos.x <= e->cCollision->radius ||
        e->cTransform->pos.x + e->cCollision->radius >= m_window.getSize().x)
      e->cTransform->velocity.x *= -1;
    if (e->cTransform->pos.y <= e->cCollision->radius ||
        e->cTransform->pos.y + e->cCollision->radius >= m_window.getSize().y)
      e->cTransform->velocity.y *= -1;
  }
}

void Game::sEnemySpawner()
{
  if ((m_currentFrame - m_lastEnemySpawnTime) >= m_config.enemy.spawnInterval)
    spawnEnemy();
}

void Game::sPlayerBulletSpawner()
{
  if (m_player->cInput->shoot == true && (m_currentFrame - m_lastPlayerBulletSpawnTime) >= m_config.bullet.rate)
    spawnBullet(m_player, Vec2(m_lastMousePos.x, m_lastMousePos.y));
}

void Game::sUpdatePlayerVelocity()
{
  m_player->cTransform->velocity = {0, 0}; // TODO: make it deaccelerate like greenberry did

  if (m_player->cInput->up && (m_player->cTransform->pos.y - m_player->cCollision->radius) >= 0)
    m_player->cTransform->velocity.y = -1 * m_config.player.maxSpeed;

  if (m_player->cInput->down && (m_player->cTransform->pos.y + m_player->cCollision->radius) <= m_window.getSize().y)
    m_player->cTransform->velocity.y = m_config.player.maxSpeed;

  if (m_player->cInput->left && (m_player->cTransform->pos.x - m_player->cCollision->radius) >= 0)
    m_player->cTransform->velocity.x = -1 * m_config.player.maxSpeed;

  if (m_player->cInput->right && (m_player->cTransform->pos.x + m_player->cCollision->radius) <= m_window.getSize().x)
    m_player->cTransform->velocity.x = m_config.player.maxSpeed;
}

void Game::renderGameOver()
{
  sf::Text title("Game Over", m_font, m_config.font.size);
  title.setPosition((m_window.getSize().x / 2) - ((float)title.getLocalBounds().width / 2),
                    (m_window.getSize().y / 2) - (((float)title.getLocalBounds().height / 2) * 3));

  std::ostringstream oss;
  oss << "Killed: " << m_killCount;

  // std::string killCountMsg = "Killed: " + m_killCount;
  sf::Text killCount(oss.str(), m_font, m_config.font.size);
  killCount.setPosition((m_window.getSize().x / 2) - ((float)killCount.getLocalBounds().width / 2),
                        (m_window.getSize().y / 2));

  m_window.draw(title);
  m_window.draw(killCount);
}

void Game::renderEntities()
{
  for (auto& e : m_entities.getEntities())
  {
    // NOT TODO: We are not going to fix this because in the future we'll never render shapes
    // because it's stupid. Everything should be sprites

    // render the player separately
    if (e->tag() == "player")
    {
      e->cSprite->sprite.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);
      if (abs(e->cTransform->angle) >= m_maxPlayerAngle)
        m_angleTransform *= -1;
      e->cTransform->angle += m_angleTransform;
      e->cSprite->sprite.setRotation(e->cTransform->angle);
      m_window.draw(e->cSprite->sprite);
    }
    else
    {
      // std::cout << "Drawing " << e->tag() << " at: (" << e->cShape->circle.getPosition().x << ","
      //           << e->cShape->circle.getPosition().y << ")" << std::endl;
      e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);
      e->cTransform->angle += 1.0f;
      e->cShape->circle.setRotation(e->cTransform->angle);
      m_window.draw(e->cShape->circle);
    }
  }
}

void Game::sRender()
{
  m_window.clear();

  if (m_gameOver)
    renderGameOver();
  else
  {
    // Experimental stuff
    sf::Cursor cursor;
    if (cursor.loadFromSystem(sf::Cursor::Cross))
      m_window.setMouseCursor(cursor);

    renderEntities();
  }

  m_window.display();
};

void Game::sUserInput()
{
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
    if (event.type == sf::Event::MouseMoved)
      resolveMouseMoveAction(event.mouseMove);
    if (event.type == sf::Event::MouseButtonReleased)
      resolveMouseButtonReleasedAction(event.mouseButton);
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
    m_reset = true;
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
  if (mouse.button == sf::Mouse::Left)
  {
    std::cout << "Mouse L Clicked at (" << mouse.x << "," << mouse.y << ")" << std::endl;
    m_player->cInput->shoot = true;
    m_lastMousePos = Vec2(mouse.x, mouse.y);
  }

  if (mouse.button == sf::Mouse::Right)
    std::cout << "Mouse R Clicked at (" << mouse.x << "," << mouse.y << ")" << std::endl;
}

void Game::resolveMouseMoveAction(sf::Event::MouseMoveEvent mouseMove)
{
  if (m_player->cInput->shoot == true)
    // std::cout << "Moved mouse shooting, target POS (" << mouseMove.x << "," << mouseMove.y << ")" << std::endl;
    m_lastMousePos = Vec2(mouseMove.x, mouseMove.y);
}

void Game::resolveMouseButtonReleasedAction(sf::Event::MouseButtonEvent mouse)
{
  if (mouse.button == sf::Mouse::Left)
  {
    std::cout << "Mouse L Released" << std::endl;
    m_player->cInput->shoot = false;
  }

  if (mouse.button == sf::Mouse::Right)
    std::cout << "Mouse R Clicked at (" << mouse.x << "," << mouse.y << ")" << std::endl;
}