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

  // remove all of the entities from this vector if their m_active is false.
  m_entities.erase(std::remove_if(m_entities.begin(), m_entities.end(), [](const auto& e) { return !e.m_active; }),
                   m_entities.end());

  // same for the tagged map
  for (auto it = m_entityMap.begin(); it != m_entityMap.end();)
  {
    it->second.erase(std::remove_if(it->second.begin(), it->second.end(), [](const auto& e) { return !e->m_active; }));
  }

  m_entitiesToAdd.clear();
};

std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag)
{
  auto e = std::shared_ptr<Entity>(
      new Entity(m_totalEntities++, tag)); // this syntax is for private constructors (instead of make_shared)

  m_entitiesToAdd.push_back(e); // push the shared pointer of newly instanced entity e to the toAdd vector
  return e;             // we are preventing iteration invalidation (i.e. modifying iterators during their iteration)
}
