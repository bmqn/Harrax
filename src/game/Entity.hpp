#pragma once

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

static constexpr size_t k_MaxComponents = 2 << 5;

using Ent = uint32_t;

template<size_t CompId>
struct GetComponentById
{
	static uint32_t Id() { return CompId; }
};

template<typename Comp>
struct GetComponentByType
{
	typedef Comp Type;
	static uint32_t Id();
};

class EntityManager
{
private:
	struct Entity
	{
		using CompMask = std::bitset<k_MaxComponents>;
		
		uint32_t Id;
		CompMask ComponentMask;
	};

public:
	using ViewFunc = std::function<void(Ent)>;

public:
	static EntityManager *Get()
	{
		static EntityManager manager;
		return &manager;
	}
	
	uint32_t Create();

	template<typename Comp>
	bool HasComponent(Ent entity)
	{
		ASSERT(entity < m_Entities.size(), "Attempt to access invalid entities component !");
		return m_Entities[entity].ComponentMask[GetComponentByType<Comp>::Id()];
	}

	template<typename Comp>
	void AddComponent(Ent entity, const Comp& component)
	{
		if (!HasComponent<Comp>(entity))
		{
			m_Entities[entity].ComponentMask[GetComponentByType<Comp>::Id()] = true;
			DynamicPool& pool = m_Components[GetComponentByType<Comp>::Id()];
			pool.Get<Comp>(entity) = component;
		}
	}

	template<typename Comp>
	Comp &GetComponent(Ent entity)
	{
		ASSERT(HasComponent<Comp>(entity), "Attempt to access component which has not been added !");
		DynamicPool& pool = m_Components[GetComponentByType<Comp>::Id()];
		return pool.Get<Comp>(entity);
	}

	template<typename ...Comps>
	void View(ViewFunc forEach)
	{
		std::vector<Ent> entityIds;
		std::vector<uint32_t> compIds = {
			(GetComponentByType<Comps>::Id())...
		};

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

		std::for_each(entityIds.begin(), entityIds.end(), forEach);
	}

private:
	EntityManager() = default;

private:
	using EntityStore = std::vector<Entity>;
	using CompStore = std::array<DynamicPool, k_MaxComponents>;

	EntityStore m_Entities;
	CompStore m_Components;
};

#define DECL_COMPONENT(type, id)                                       \
	template<> struct GetComponentByType<type>                         \
	{                                                                  \
		static constexpr uint32_t Id()                                 \
		{                                                              \
			return id;                                                 \
		}                                                              \
	};                                                                 \
	template<> struct GetComponentById<id>                             \
	{                                                                  \
		typedef type Type;                                             \
	};

#include "game/Components.hpp"