#include "Game.h"
#include "Utils.h"

#include <fstream>
#include <iostream>
#include <math.h>
#include <sstream>

using namespace Utils;

Game::Game(const std::string& config) { init(config); }

void Game::init(const std::string& path)
{
  // read and set game configuration from config file
  setConfigFromFile(path);
  if (!m_font.loadFromFile(m_config.font.path))
  {
    std::cerr << "Could not load game font." << std::endl;
    exit(-1);
  }

  // seed the random number generator with current time on game initialization
  srand(time(NULL));

  // set game window
  m_window.create(sf::VideoMode(m_config.window.width, m_config.window.height), "Assignment II");
  m_window.setFramerateLimit(m_config.window.frameLimit);

  // spawn the player character
  spawnPlayer();
}

void Game::run()
{
  while (m_running)
  {
    m_entities.update();
    sUserInput();
    sReset();

    if (!m_gameOver)
    {
      if (!m_paused)
      {
        sEnemySpawner();
        sPlayerBulletSpawner();
        sLifespan();
        sUpdatePlayerVelocity(); // TODO: figure out if this is actually a good idea
        sMovement();
        sSpecial();
        sCollision();
        // sAcceleration();
      }
    }

    sRender();

    // increment the current active frame
    if (!m_paused)
      m_currentActiveFrame++;

    // increment the current total frame
    m_currentFrame++;
  }
}

void Game::setConfigFromFile(const std::string& path)
{
  std::ifstream fin(path);
  int fontR, fontG, fontB, playerFillR, playerFillG, playerFillB, playerOutlineR, playerOutlineG, playerOutlineB, enemyFillR,
      enemyFillG, enemyFillB, enemyOutlineR, enemyOutlineG, enemyOutlineB, bulletFillR, bulletFillG, bulletFillB,
      bulletOutlineR, bulletOutlineG, bulletOutlineB, specialBulletFillR, specialBulletFillG, specialBulletFillB,
      specialBulletOutlineR, specialBulletOutlineG, specialBulletOutlineB;
  fin >>
      // Window config
      m_config.window.width >> m_config.window.height >> m_config.window.frameLimit >> m_config.window.fullScreen >>
      // Font config
      m_config.font.path >> m_config.font.sizeS >> m_config.font.sizeM >> m_config.font.sizeL >> fontR >> fontG >> fontB >>
      // Player config
      m_config.player.shapeRadius >> m_config.player.collisionRadius >> m_config.player.maxSpeed >> playerFillR >>
      playerFillG >> playerFillB >> playerOutlineR >> playerOutlineG >> playerOutlineB >> m_config.player.outlineThickness >>
      m_config.player.vertices >> m_config.player.special.duration >> m_config.player.special.cooldown >>
      m_config.player.special.moveSpeed >>
      // Enemy config
      m_config.enemy.shapeRadius >> m_config.enemy.collisionRadius >> m_config.enemy.minSpeed >> m_config.enemy.maxSpeed >>
      enemyFillR >> enemyFillG >> enemyFillB >> enemyOutlineR >> enemyOutlineG >> enemyOutlineB >>
      m_config.enemy.outlineThickness >> m_config.enemy.minVertices >> m_config.enemy.maxVertices >>
      m_config.enemy.spawnInterval >> m_config.enemy.lifespan >>
      // Bullet config
      m_config.bullet.shapeRadius >> m_config.bullet.collisionRadius >> m_config.bullet.speed >> bulletFillR >>
      bulletFillG >> bulletFillB >> bulletOutlineR >> bulletOutlineG >> bulletOutlineB >> m_config.bullet.outlineThickness >>
      m_config.bullet.vertices >> m_config.bullet.lifespan >> m_config.bullet.rate >>
      m_config.player.special.bulletShapeRadius >> m_config.player.special.bulletCollisionRadius >>
      m_config.player.special.bulletRate >> m_config.player.special.bulletSpeed >> specialBulletFillR >>
      specialBulletFillG >> specialBulletFillB >> specialBulletOutlineR >> specialBulletOutlineG >> specialBulletOutlineB;

  m_config.font.color = sf::Color(toUint8(fontR), toUint8(fontG), toUint8(fontB));
  m_config.player.fillColor = sf::Color(toUint8(playerFillR), toUint8(playerFillG), toUint8(playerFillB));
  m_config.player.outlineColor = sf::Color(toUint8(playerOutlineR), toUint8(playerOutlineG), toUint8(playerOutlineB));
  m_config.enemy.fillColor = sf::Color(toUint8(enemyFillR), toUint8(enemyFillG), toUint8(enemyFillB));
  m_config.enemy.outlineColor = sf::Color(toUint8(enemyOutlineR), toUint8(enemyOutlineG), toUint8(enemyOutlineB));
  m_config.bullet.fillColor = sf::Color(toUint8(bulletFillR), toUint8(bulletFillG), toUint8(bulletFillB));
  m_config.bullet.outlineColor = sf::Color(toUint8(bulletOutlineR), toUint8(bulletOutlineG), toUint8(bulletOutlineB));
  m_config.player.special.bulletFillColor =
      sf::Color(toUint8(specialBulletFillR), toUint8(specialBulletFillG), toUint8(specialBulletFillB));
  m_config.player.special.bulletOutlineColor =
      sf::Color(toUint8(specialBulletOutlineR), toUint8(specialBulletOutlineG), toUint8(specialBulletOutlineB));
}

void Game::setPaused(bool paused) { m_paused = paused; }

void Game::reset()
{
  m_reset = false;
  m_gameOver = false;
  m_score = 0;
  m_entities.destroyAll();
  spawnPlayer();
}

void Game::spawnPlayer()
{
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

  // Our player entity will be a sprite instead of a shape, exceptionally against the assignment
  entity->cSprite = std::make_shared<CSprite>("./assets/textures/Sam.png", radius * 2);

  // Add an input component to the player so that we can use inputs
  entity->cInput = std::make_shared<CInput>();
  // Add the special ability component
  entity->cSpecial = std::make_shared<CSpecial>(m_config.player.special.duration, m_config.player.special.cooldown);

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
  float rndX = randBetween(offset, maxX);
  float rndY = randBetween(offset, maxY);
  float speed = randBetween(m_config.enemy.minSpeed, m_config.enemy.maxSpeed);
  int points = randBetween(m_config.enemy.minVertices, m_config.enemy.maxVertices);

  std::cout << "Enemy will be spawned at: (" << rndX << "," << rndY << ")" << std::endl;

  // set position
  Vec2 position(rndX, rndY);
  // generate random angle
  float angle = (rand() % 361) * M_PI / 180.0;
  // calculate velocity vector
  Vec2 velocity((speed * cos(angle)), (speed * sin(angle)));

  std::cout << "REGULAR enemy has a velocity vector of (" << velocity.x << "," << velocity.y << ")" << std::endl;

  // Set transform component with above data
  entity->cTransform = std::make_shared<CTransform>(position, velocity, angle);
  // Set collision component separately
  entity->cCollision = std::make_shared<CCollision>(m_config.enemy.shapeRadius + m_config.enemy.outlineThickness);
  // Set score component based on generated points
  entity->cScore = std::make_shared<CScore>(points);

  sf::Color fillColor(randBetween(0, 255), randBetween(0, 255), randBetween(0, 255));
  sf::Color outlineColor(randBetween(0, 255), randBetween(0, 255), randBetween(0, 255));
  entity->cShape =
      std::make_shared<CShape>(m_config.enemy.shapeRadius, points, fillColor, outlineColor, m_config.enemy.outlineThickness);
}

void Game::spawnEnemy(const int points, Vec2& pos, Vec2& originalVelocity, const float angle, const int radius,
                      const sf::Color fillColor, const sf::Color outlineColor)
{
  auto entity = m_entities.addEntity("enemy");

  // std::cout << "Enemy will be spawned at: (" << pos.x << "," << pos.y << ")" << std::endl;

  const float speed = std::sqrt((originalVelocity.x * originalVelocity.x) + (originalVelocity.y * originalVelocity.y));

  std::cout << "Calculated hypotenuse (speed) was " << speed << std::endl;

  Vec2 velocity((speed * cos(angle)), (speed * sin(angle)));

  std::cout << "Small enemy has a velocity vector of (" << velocity.x << "," << velocity.y << ")" << std::endl;

  entity->cTransform = std::make_shared<CTransform>(pos, velocity, angle);
  entity->cCollision = std::make_shared<CCollision>(radius + m_config.enemy.outlineThickness);
  entity->cScore = std::make_shared<CScore>(points * 2);
  entity->cLifespan = std::make_shared<CLifespan>(m_config.enemy.lifespan);
  entity->cShape = std::make_shared<CShape>(radius, points, fillColor, outlineColor, m_config.enemy.outlineThickness);
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> e)
{
  const int points = e->cShape->circle.getPointCount();
  const float angleSize = (360 / points) * M_PI / 180.0f; // converted to radians

  for (size_t i = 1; i <= points; i++)
  {
    Vec2 pos(e->cTransform->pos.x, e->cTransform->pos.y);
    const float angle = i * angleSize;
    const int radius = m_config.enemy.shapeRadius / 2;

    spawnEnemy(points, pos, e->cTransform->velocity, angle, radius, e->cShape->circle.getFillColor(),
               e->cShape->circle.getOutlineColor());
  }
}

void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2& target)
{
  auto bullet = m_entities.addEntity("bullet");
  int shapeRadius = m_config.bullet.shapeRadius;
  int collisionRadius = m_config.bullet.collisionRadius;
  int bulletSpeed = m_config.bullet.speed;

  if (m_player->cSpecial->active)
  {
    shapeRadius = m_config.player.special.bulletShapeRadius;
    collisionRadius = m_config.player.special.bulletCollisionRadius;
    bulletSpeed = m_config.player.special.bulletSpeed;
  }

  // so we have two vectors: playerPos and targetPos
  Vec2 playerPos(m_player->cTransform->pos.x, m_player->cTransform->pos.y);
  Vec2 targetPos(target.x, target.y);

  // we need to get the difference (distance) between both vectors
  Vec2 diff = targetPos - playerPos;
  // now we can use archtangent to get the angle
  float angle = atan2f(diff.y, diff.x);
  // calculate velocity vector
  Vec2 velocity((bulletSpeed * cos(angle)), (bulletSpeed * sin(angle))); // TODO: somewhow actually understand this

  sf::Color bulletFillColor = m_config.bullet.fillColor;
  sf::Color bulletOutlineColor = m_config.bullet.outlineColor;

  if (m_player->cSpecial->active)
  {
    bulletFillColor = m_config.player.special.bulletFillColor;
    bulletOutlineColor = m_config.player.special.bulletOutlineColor;
  }

  // Set transform component
  bullet->cShape = std::make_shared<CShape>(shapeRadius, m_config.bullet.vertices, bulletFillColor, bulletOutlineColor,
                                            m_config.bullet.outlineThickness);
  bullet->cTransform = std::make_shared<CTransform>(playerPos, velocity, angle);
  bullet->cCollision = std::make_shared<CCollision>(collisionRadius);
  bullet->cLifespan = std::make_shared<CLifespan>(m_config.bullet.lifespan);

  m_lastPlayerBulletSpawnTime = m_currentActiveFrame;
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity)
{
  // TODO: implement your own special weapon
  // spawn 6 entities
  //   calculate their positions with segmented angles like small enemies
  //   give them lifespans using the same value as the special duration
  //   the angle is always 1 plus the previous value
  //   some how determine their movement so that the movement system knows what to do
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
    // if entity has no lifespan component, skip it
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

bool Game::collides(const Vec2& pos1, const Vec2& pos2, const float totalRadius)
{
  Vec2 origin1(pos1.x, pos1.y);
  Vec2 origin2(pos2.x, pos2.y);

  Vec2 diff = origin1 - origin2;

  float dist = (diff.x * diff.x) + (diff.y * diff.y);

  // if their distance is less than the sum of their radiuses, they are colliding
  return dist < (totalRadius * totalRadius);
}

void Game::sCollision()
{
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
        m_score += e->cScore->score;
        // std::cout << "!! ENEMY KILLED. Remaining: (" << m_entities.getEntities("enemy").size() << ")" << std::endl;
        // std::cout << "CURRENT KILL COUNT: [" << m_score << "]" << std::endl;
        if (e->cShape->circle.getRadius() == m_config.enemy.shapeRadius)
          spawnSmallEnemies(e);
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
  if ((m_currentActiveFrame - m_lastEnemySpawnTime) >= m_config.enemy.spawnInterval)
  {
    spawnEnemy();

    // record when the most recent enemy was spawned
    m_lastEnemySpawnTime = m_currentActiveFrame;
  }
}

void Game::sPlayerBulletSpawner()
{
  int bulletRate = m_config.bullet.rate;

  if (m_player->cSpecial->active)
    bulletRate = m_config.player.special.bulletRate;

  if (m_player->cInput->shoot == true && (m_currentActiveFrame - m_lastPlayerBulletSpawnTime) >= bulletRate)
    spawnBullet(m_player, Vec2(m_lastMousePos.x, m_lastMousePos.y));
}

void Game::sUpdatePlayerVelocity()
{
  m_player->cTransform->velocity = {0, 0}; // TODO: make it deaccelerate like greenberry did
  float playerSpeed = m_config.player.maxSpeed;
  if (m_player->cSpecial->active)
    playerSpeed = m_config.player.special.moveSpeed;

  if (m_player->cInput->up && (m_player->cTransform->pos.y - m_player->cCollision->radius) >= 0)
    m_player->cTransform->velocity.y = -1 * playerSpeed;

  if (m_player->cInput->down && (m_player->cTransform->pos.y + m_player->cCollision->radius) <= m_window.getSize().y)
    m_player->cTransform->velocity.y = playerSpeed;

  if (m_player->cInput->left && (m_player->cTransform->pos.x - m_player->cCollision->radius) >= 0)
    m_player->cTransform->velocity.x = -1 * playerSpeed;

  if (m_player->cInput->right && (m_player->cTransform->pos.x + m_player->cCollision->radius) <= m_window.getSize().x)
    m_player->cTransform->velocity.x = playerSpeed;
}

void Game::renderGameOver()
{
  sf::Text title("Game Over", m_font, m_config.font.sizeL);
  title.setPosition((m_window.getSize().x / 2) - ((float)title.getLocalBounds().width / 2),
                    (m_window.getSize().y / 2) - (((float)title.getLocalBounds().height / 2) * 3));

  std::ostringstream oss;
  oss << "Killed: " << m_killCount << " | Score: " << m_score;

  // std::string killCountMsg = "Killed: " + m_killCount;
  sf::Text killCount(oss.str(), m_font, m_config.font.sizeL);
  killCount.setPosition((m_window.getSize().x / 2) - ((float)killCount.getLocalBounds().width / 2),
                        (m_window.getSize().y / 2));

  m_window.draw(title);
  m_window.draw(killCount);
}

void Game::renderScore()
{
  std::ostringstream oss;
  oss << "SCORE: " << m_score;

  sf::Text score(oss.str(), m_font, m_config.font.sizeM);
  score.setPosition(10, 0); // TODO: find out why margins don't work as expected

  m_window.draw(score);
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
      // the following code makes the player character go tilting from side to side
      if (abs(e->cTransform->angle) >= m_maxPlayerAngle)
        m_angleTransform *= -1;
      e->cTransform->angle += m_angleTransform;
      e->cSprite->sprite.setRotation(e->cTransform->angle);
      // TODO: consider checking this only if there's been changes, NOT every time
      if (m_player->cSpecial->active)
        // TODO: this is inefficient because we wil be loading textures on every frame
        // consider loading all textures on game init, save them in memory, then use them accordingly
        e->cSprite->texture.loadFromFile("./assets/textures/SamSpecial.png");
      else
        e->cSprite->texture.loadFromFile("./assets/textures/Sam.png");

      e->cSprite->sprite.setTexture(e->cSprite->texture);
      // then drawn to the window
      m_window.draw(e->cSprite->sprite);
    }
    else
    {
      // whilst this code makes other entities (all but player) spin around continuously
      e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);
      e->cTransform->angle += 1.0f;
      e->cShape->circle.setRotation(e->cTransform->angle);
      // and then drawn to the window as well
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

    renderScore();
    renderEntities();
  }

  m_window.display();
};

void Game::sSpecial()
{
  if (m_player->cSpecial->active && m_currentActiveFrame - m_lastPlayerSpecial >= m_player->cSpecial->duration)
    m_player->cSpecial->active = false;
  if (!m_player->cSpecial->enabled && m_currentActiveFrame - m_lastPlayerSpecial >= m_player->cSpecial->cooldown)
    m_player->cSpecial->enabled = true;
};

void Game::sUserInput()
{
  sf::Event event;
  while (m_window.pollEvent(event))
  {
    // this event triggers when the window is closed
    if (event.type == sf::Event::Closed)
    {
      m_running = false;
      m_window.close();
    }

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
    if (!m_paused)
      m_reset = true;
    break;

  case sf::Keyboard::Space:
    m_paused = !m_paused;
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
    m_running = false;
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
  {
    std::cout << "Mouse R Clicked at (" << mouse.x << "," << mouse.y << ")" << std::endl;
    if (m_player->cSpecial->enabled && !m_player->cSpecial->active)
    {
      m_player->cSpecial->active = true;
      m_player->cSpecial->enabled = false;
      m_lastPlayerSpecial = m_currentActiveFrame;
    }
  }
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