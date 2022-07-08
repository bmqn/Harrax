#pragma once

#include "Config.h"
#include "util/Log.h"
#include "util/DynamicPool.hpp"

#include <cstdint>
#include <type_traits>
#include <limits>
#include <bitset>
#include <vector>
#include <array>
#include <functional>
#include <algorithm>

class EntityManager;

template<typename ...Comps>
std::vector<uint32_t> GetComponentIds()
{
	return { (EntityManager::Get()->GetComponentId<Comps>())... };
}

template<typename Comp>
constexpr const char *GetComponentName() { return nullptr }

template<typename Comp>
class ComponentRegisterer
{
public:
	ComponentRegisterer()
	{
		EntityManager::Get()->RegisterComponent<Comp>();
	}
};

class EntityManager
{
	struct Entity;
	template<typename> friend class ComponentRegisterer;
	template<typename...> friend std::vector<uint32_t> GetComponentIds();

	using EntityStore = std::vector<Entity>;
	using CompStore = std::array<DynamicPool, k_MaxComponents>;
	using CompRegistry = std::unordered_map<const char*, uint32_t>;
	using ViewFunc = std::function<void(uint32_t)>;

private:
	struct Entity
	{
		using CompMask = std::bitset<k_MaxComponents>;
		
		uint32_t Id;
		CompMask ComponentMask;
	};

public:
	static EntityManager *Get()
	{
		static EntityManager manager;
		return &manager;
	}
	
	uint32_t Create()
	{
		uint32_t entityId = static_cast<uint32_t>(m_Entities.size());
		m_Entities.emplace_back();
		m_Entities[entityId].Id = entityId;
		return m_Entities[entityId].Id;
	}

	template<typename Comp>
	bool HasComponent(uint32_t entityId)
	{
		ASSERT(entityId < m_Entities.size(), "Attempt to access invalid entities component !");
		return m_Entities[entityId].ComponentMask[GetComponentId<Comp>()];
	}

	template<typename Comp>
	void AddComponent(uint32_t entityId, const Comp& component)
	{
		if (!HasComponent<Comp>(entityId))
		{
			m_Entities[entityId].ComponentMask[GetComponentId<Comp>()] = true;
			DynamicPool &pool = m_Components[GetComponentId<Comp>()];
			pool.Get<Comp>(entityId) = component;
		}
	}

	template<typename Comp>
	Comp &GetComponent(uint32_t entityId)
	{
		if (HasComponent<Comp>(entityId))
		{
			DynamicPool& pool = m_Components[GetComponentId<Comp>()];
			return pool.Get<Comp>(entityId);
		}
		else
		{
			LOG_EVERY(1, "Attempt to access component which has not been added !");
			static Comp s_Dummy;
			return s_Dummy;
		}
	}

	std::vector<uint32_t> View(std::vector<uint32_t> compIds)
	{
		std::vector<uint32_t> entityIds;
		for (Entity entity : m_Entities)
		{
			bool valid = true;
			for (uint32_t compId : compIds)
			{
				if (!entity.ComponentMask[compId])
				{
					valid = false;
					break;
				}
			}
			if (valid)
			{
				entityIds.push_back(entity.Id);
			}
		}
		return entityIds;
	}

	template<typename ...Comps>
	std::vector<uint32_t> View()
	{
		std::vector<uint32_t> compIds = {
			(GetComponentId<Comps>())...
		};
		return View(compIds);
	}

	template<typename ...Comps>
	void View(ViewFunc forEach)
	{
		std::vector<uint32_t> entityIds = View<Comps>;
		std::for_each(entityIds.begin(), entityIds.end(), forEach);
	}

private:
	EntityManager() = default;

	template<typename Comp>
	void RegisterComponent()
	{
		const char *compName = GetComponentName<Comp>();
		if (m_ComponentsRegistry.size() < k_MaxComponents)
		{
			if (m_ComponentsRegistry.find(compName) == m_ComponentsRegistry.end())
			{
				m_ComponentsRegistry[compName] = static_cast<uint32_t>(m_ComponentsRegistry.size());
				LOG("Registered component '%s' !", compName);
			}
		}
		else
		{
			LOG("Failed to register component '%s'. "
			    "Increase the limit (currently %zu) !", compName, k_MaxComponents);
		}
	};

	template<typename Comp>
	uint32_t GetComponentId()
	{
		const char *compName = GetComponentName<Comp>();
		ASSERT(m_ComponentsRegistry.find(compName) != m_ComponentsRegistry.end(),
		       "Component '%s' is not registered !", compName
		);
		return m_ComponentsRegistry[compName];
	}

private:
	EntityStore m_Entities;
	CompStore m_Components;
	CompRegistry m_ComponentsRegistry;
};
 
#define DECL_COMPONENT(type)                                                                       \
	template<> constexpr const char *GetComponentName<type>() { return #type; }                    \
	static ComponentRegisterer<type> _RegisterComponent_##type;