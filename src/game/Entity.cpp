#include "Entity.hpp"

Ent EntityManager::Create()
{
	Ent entity = static_cast<Ent>(m_Entities.size());
	m_Entities.emplace_back();
	m_Entities[entity].Id = entity;
	return m_Entities[entity].Id;
}