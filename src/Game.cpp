#include "Game.h"
#include "Utils.h"

#include <fstream>
#include <iostream>
#include <math.h>
#include <sstream>

Game::Game(const std::string& config) { init(config); }

void Game::init(const std::string& path)
{
  // TODO : read in config file here
  //        use the premade PlayerConfig, EnemyConfig, BulletConfig variables

  //  std::ifstream fin(path);
  // fin >> m_playerConfig.SR >> m_playerConfig.CR >>

  if (!m_font.loadFromFile("./assets/fonts/prospero.ttf"))
  {
    std::cerr << "Could not load game font." << std::endl;
    exit(-1);
  }

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

  float radius = 32.0f;
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
  // entity->cShape = std::make_shared<CShape>(m_playerConfig.SR, m_playerConfig.V, ...);
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

  // Set transform component with above data
  entity->cTransform = std::make_shared<CTransform>(position, velocity, angle);
  // Set collision component separately
  entity->cCollision = std::make_shared<CCollision>(radius + thickness);

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
  auto bullet = m_entities.addEntity("bullet");

  // TODO: give the bullet all of its properties (read from config file)

  float speed = 10;
  float radius = 10;
  float points = 8;

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
  bullet->cShape = std::make_shared<CShape>(radius, points, sf::Color(255, 255, 255), sf::Color(255, 0, 0), 2);
  bullet->cTransform = std::make_shared<CTransform>(playerPos, velocity, angle);
  bullet->cCollision = std::make_shared<CCollision>(radius + 1);
  bullet->cLifespan = std::make_shared<CLifespan>(50);

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
  int enemySpawnRate = 50; // TODO: Read from config file
  if ((m_currentFrame - m_lastEnemySpawnTime) >= enemySpawnRate)
    spawnEnemy();
}

void Game::sPlayerBulletSpawner()
{
  int playerBulletSpawnRate = 10; // TODO: Read from config file
  if (m_player->cInput->shoot == true && (m_currentFrame - m_lastPlayerBulletSpawnTime) >= playerBulletSpawnRate)
    spawnBullet(m_player, Vec2(m_lastMousePos.x, m_lastMousePos.y));
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

void Game::renderGameOver()
{
  sf::Text title("Game Over", m_font, m_fontSize);
  title.setPosition((m_window.getSize().x / 2) - ((float)title.getLocalBounds().width / 2),
                    (m_window.getSize().y / 2) - (((float)title.getLocalBounds().height / 2) * 3));

  std::ostringstream oss;
  oss << "Killed: " << m_killCount;

  // std::string killCountMsg = "Killed: " + m_killCount;
  sf::Text killCount(oss.str(), m_font, m_fontSize);
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