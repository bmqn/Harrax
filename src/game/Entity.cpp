#include "Entity.hpp"

Ent EntityManager::Create()
{
	Ent entity = static_cast<Ent>(m_Entities.size());
	m_Entities.emplace_back();
	m_Entities[entity].Id = entity;
	return m_Entities[entity].Id;
}

template<typename Comp>
bool EntityManager::HasComponent(Ent entity)
{
	ASSERT(entity < m_Entities.size(), "Attempt to access invalid entities component !");
	return m_Entities[entity].ComponentMask[GetComponentByType<Comp>::Id()];
}

template<typename Comp>
void EntityManager::AddComponent(Ent entity, const Comp& component)
{
	if (!HasComponent<Comp>(entity))
	{
		m_Entities[entity].ComponentMask[GetComponentByType<Comp>::Id()] = true;
		m_Components[GetComponentByType<Comp>::Id()].Get<Comp>(entity) = component;
	}
}

template<typename Comp>
Comp &EntityManager::GetComponent(Ent entity)
{
	ASSERT(HasComponent<Comp>(entity), "Attempt to access component which has not been added !");
	return m_Components[GetComponentByType<Comp>::Id()].Get<Comp>(entity);
}