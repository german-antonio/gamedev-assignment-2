#pragma once

#include <memory>
#include <string>

#include "Components.h"

typedef std::vector<std::shared_ptr<Entity>> EntityVec; // 'EntityVec' as an alias of all that's before it after typedef
typedef std::map<std::string, EntityVec> EntityMap;     // Same for EntityMap

class EntityManager
{
  EntityVec m_entities;
  EntityVec m_toAdd;
  EntityMap m_entityMap;
  size_t m_totalEntities = 0;

public:
  EntityManager();
  void update();
  std::shared_ptr<Entity> addEntity(const std::string& tag);
  EntityVec& getEntities();
  EntityVec& getEntities(const std::string& tag);
  std::shared_ptr<Entity> addEntity(const std::string& tag);
};