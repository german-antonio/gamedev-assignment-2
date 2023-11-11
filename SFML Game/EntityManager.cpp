#include "EntityManager.h"

// Empty constructor
EntityManager::EntityManager() {}

// Will be called at beginning of each frame by game engine
// entities added to m_entitiesToAdd will be available to use this frame
void EntityManager::update()
{
  for (auto e : m_entitiesToAdd)
  {
    m_entities.push_back(e);
    m_entityMap[e->tag()].push_back(e);
  }

  removeDeadEntities(m_entities);

  for (auto& [tag, entityVec] : m_entityMap)
  {
    removeDeadEntities(entityVec);
  }

  m_entitiesToAdd.clear();
};

std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag)
{
  auto entity = std::shared_ptr<Entity>(
      new Entity(m_totalEntities++, tag)); // this syntax is for private constructors (instead of make_shared)

  m_entitiesToAdd.push_back(entity); // push the shared pointer of newly instanced entity e to the toAdd vector
  return entity; // we are preventing iteration invalidation (i.e. modifying iterators during their iteration)
}

// remove all of the entities from this vector if their m_active is false.
void EntityManager::removeDeadEntities(EntityVec& vec)
{
  vec.erase(std::remove_if(vec.begin(), vec.end(), [](const auto& e) { return !e.m_active; }), vec.end());
}