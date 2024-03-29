#pragma once

#include <memory>
#include <string>

#include "Components.h"

class Entity
{
  friend class EntityManager;

  bool m_active = true;
  size_t m_id = 0;
  std::string m_tag = "default";

  /* Private Constructor */
  Entity(const size_t id, const std::string& tag);

public:
  /* Component Pointers */
  std::shared_ptr<CTransform> cTransform;
  std::shared_ptr<CShape> cShape;
  std::shared_ptr<CEdge> cEdge;
  std::shared_ptr<CCollision> cCollision;
  std::shared_ptr<CInput> cInput;
  std::shared_ptr<CSpecial> cSpecial;
  std::shared_ptr<CScore> cScore;
  std::shared_ptr<CLifespan> cLifespan;
  std::shared_ptr<CSprite> cSprite;

  /* Private member access */
  const size_t id() const;
  const std::string& tag() const;
  bool isActive() const;

  void destroy();
};