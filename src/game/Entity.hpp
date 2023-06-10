#pragma once

#include "Config.h"
#include "util/Log.h"
#include "util/DynamicPool.hpp"

#include <cstdint>
#include <utility>
#include <algorithm>
#include <bitset>
#include <vector>
#include <unordered_map>
#include <tuple>

using EntId = uint32_t;
using CompId = uint32_t;
using CompMask = std::bitset<k_MaxComponents>;

template<typename Comp>
constexpr const char *GetComponentName() { return nullptr; }
template<typename Comp>
constexpr uint32_t GetComponentId() { return 0; }

struct Entity
{
	EntId Id;
	CompMask Mask;
};

class EntityManager
{
	friend class Registry;
	template<typename> friend class ComponentRegisterer;

	using EntityStore = std::vector<Entity>;
	using CompStore = std::unordered_map<CompId, std::pair<size_t, DynamicPool>>;

private:
	EntityManager() = default;

	EntId Create()
	{
		EntId id = static_cast<EntId>(m_Entities.size());
		m_Entities.emplace_back();
		m_Entities[id].Id = id;
		m_Entities[id].Mask = CompMask();
		return id;
	}

	template<typename Comp>
	bool HasComponent(EntId id)
	{
		ASSERT(m_Components.find(GetComponentId<Comp>()) != m_Components.end(),
			"Attempt to access invalid entities component !");
		return m_Entities[id].Mask[m_Components[GetComponentId<Comp>()].first];
	}

	template<typename Comp, typename... Args>
	void AddComponent(EntId id, Args &&...args)
	{
		if (!HasComponent<Comp>(id))
		{
			DynamicPool &pool = m_Components[GetComponentId<Comp>()].second;
			pool.Get<Comp>(id) = Comp{args...};
			
			m_Entities[id].Mask.flip(m_Components[GetComponentId<Comp>()].first);
		}
	}

	template<typename Comp>
	Comp &GetComponent(EntId id)
	{
		ASSERT(HasComponent<Comp>(id),
			"Attempt to access invalid entities component !");
		DynamicPool& pool = m_Components[GetComponentId<Comp>()].second;
		return pool.Get<Comp>(id);
	}

	template <typename... Comps>
	CompMask GetMask()
	{
		CompMask mask;
		(mask.flip(m_Components[GetComponentId<Comps>()].first), ...);
		return mask;
	}

	template<typename Comp>
	void RegisterComponent()
	{
		ASSERT(m_Components.size() < k_MaxComponents,
			"Cannot register more than '" STRINGIFY(k_MaxComponents) "' components !");
		auto &component = m_Components[GetComponentId<Comp>()];
		component.first = m_Components.size() - 1;
		LOG("Registered component '%s' !", GetComponentName<Comp>());
	};

private:
	EntityStore m_Entities;
	CompStore m_Components;
};