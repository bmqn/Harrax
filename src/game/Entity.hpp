#pragma once

#include "Config.h"
#include "util/Log.h"
#include "util/DynamicPool.hpp"

#include <cstdint>
#include <utility>
#include <algorithm>
#include <functional>
#include <bitset>
#include <vector>
#include <array>
#include <unordered_map>

using CompIds = std::vector<uint32_t>;
using EntIds = std::vector<uint32_t>;
using ViewFunc = std::function<void(uint32_t)>;

template<typename Comp>
constexpr const char *GetComponentName() { return nullptr; }

class Entity
{
	friend class EntityManager;
	using CompMask = std::bitset<k_MaxComponents>;
	
private:
	uint32_t m_Id;
	CompMask m_ComponentMask;
};

class EntityManager
{
	friend class Registry;
	template<typename> friend class ComponentRegisterer;
	template<typename...> friend CompIds GetComponentIds();

	using EntityStore = std::vector<Entity>;
	using CompStore = std::array<DynamicPool, k_MaxComponents>;
	using CompRegistry = std::unordered_map<const char*, uint32_t>;

private:
	EntityManager() = default;

	uint32_t Create()
	{
		uint32_t entityId = static_cast<uint32_t>(m_Entities.size());
		m_Entities.emplace_back();
		m_Entities[entityId].m_Id = entityId;
		return m_Entities[entityId].m_Id;
	}

	template<typename Comp>
	bool HasComponent(uint32_t entityId)
	{
		ASSERT(entityId < m_Entities.size(), "Attempt to access invalid entities component !");
		return m_Entities[entityId].m_ComponentMask[GetComponentId<Comp>()];
	}

	template<typename Comp>
	void AddComponent(uint32_t entityId, const Comp& component)
	{
		if (!HasComponent<Comp>(entityId))
		{
			m_Entities[entityId].m_ComponentMask[GetComponentId<Comp>()] = true;
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

	EntIds View(CompIds compIds)
	{
		EntIds entityIds;
		for (Entity entity : m_Entities)
		{
			bool valid = true;
			for (uint32_t compId : compIds)
			{
				if (!entity.m_ComponentMask[compId])
				{
					valid = false;
					break;
				}
			}
			if (valid)
			{
				entityIds.push_back(entity.m_Id);
			}
		}
		return entityIds;
	}

	template<typename ...Comps>
	EntIds View()
	{
		CompIds compIds = {
			(GetComponentId<Comps>(), ...)
		};
		return View(compIds);
	}

	template<typename ...Comps>
	void View(ViewFunc forEach)
	{
		EntIds entityIds = View<Comps...>;
		std::for_each(entityIds.begin(), entityIds.end(), forEach);
	}

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