#pragma once

#include "Entity.h"

#include <map>
#include <vector>

typedef std::vector<std::shared_ptr<Entity>> EntityVec; // EntityVec is an alias for all that's before it after typedef
typedef std::map<std::string, EntityVec> EntityMap;     // Same for EntityMap

class EntityManager
{
  EntityVec m_entities;
  EntityVec m_entitiesToAdd;
  EntityMap m_entityMap;
  size_t m_totalEntities = 0;

  void removeDeadEntities(EntityVec& vec);

public:
  EntityManager();
  void update();
  std::shared_ptr<Entity> addEntity(const std::string& tag);
  EntityVec& getEntities();
  EntityVec& getEntities(const std::string& tag);
};