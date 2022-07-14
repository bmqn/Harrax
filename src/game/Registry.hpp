#pragma once

#include "game/Entity.hpp"
#include "game/System.hpp"

#include <functional>

class Registry
{
	template<typename> friend class ComponentRegisterer;
	template<typename> friend class SystemRegisterer;
	template<typename...> friend CompIds GetComponentIds();

public:
	static Registry *Get()
	{
		static Registry reg;
		return &reg;
	}

public:
	uint32_t CreateEntity()
	{
		return m_EntityManager.Create();
	}

	template<class Sys, typename ...Args>
	Sys *CreateSystem(const Args&&... args)
	{
		Sys *sys = m_SystemManager.Create<Sys, Args...>(
			std::forward<Args>(args)...
		);
		sys->m_Components = GetSystemComponentIds<Sys>();
		m_NeedsEntityRefresh = true;
		return sys;
	}

	template<typename Comp>
	bool HasComponent(uint32_t entityId)
	{
		return m_EntityManager.HasComponent<Comp>(entityId);
	}

	template<typename Comp>
	void AddComponent(uint32_t entityId, const Comp &component)
	{
		m_EntityManager.AddComponent<Comp>(entityId, component);
		m_NeedsEntityRefresh = true;
	}

	template<typename Comp>
	Comp &GetComponent(uint32_t entityId)
	{
		return m_EntityManager.GetComponent<Comp>(entityId);
	}

	EntIds View(CompIds compIds)
	{
		return m_EntityManager.View(compIds);
	}

	template<typename ...Comps>
	EntIds View()
	{
		return m_EntityManager.View<Comps...>();
	}

	template<typename ...Comps>
	void View(ViewFunc forEach)
	{
		m_EntityManager.View<Comps...>(forEach);
	}

	void Update()
	{
		if (m_NeedsEntityRefresh)
		{
			std::unordered_map<CompIds, EntIds, CompIdsHasher> entityIdsFromCompIdsCache;

			for (auto &sys : m_SystemManager.m_Systems)
			{
				if (sys)
				{
					if (   entityIdsFromCompIdsCache.find(sys->m_Components)
						!= entityIdsFromCompIdsCache.end())
					{
						sys->SetEntities(entityIdsFromCompIdsCache[sys->m_Components]);
					}
					else
					{
						EntIds entityIds = m_EntityManager.View(sys->m_Components);
						entityIdsFromCompIdsCache[sys->m_Components] = entityIds;
						
						sys->SetEntities(entityIds);
					}
				}
			}

			m_NeedsEntityRefresh = false;
		}
	}

private:
	Registry() = default;

private:
	EntityManager m_EntityManager;
	SystemManager m_SystemManager;
	bool m_NeedsEntityRefresh;
};

template<typename ...Comps>
CompIds GetComponentIds()
{
	return { (Registry::Get()->m_EntityManager.GetComponentId<Comps>(), ...) };
}

template<typename Comp>
class ComponentRegisterer
{
public:
	ComponentRegisterer()
	{
		Registry::Get()->m_EntityManager.RegisterComponent<Comp>();
	}
};

template<class Sys>
class SystemRegisterer
{
public:
	SystemRegisterer()
	{
		Registry::Get()->m_SystemManager.RegisterSystem<Sys>();
	}
};

#define DECL_SYSTEM(type, ...)                                                                    \
	template<> constexpr const char *GetSystemName<type>() { return #type; }                      \
	template<> CompIds GetSystemComponentIds<type>() { return GetComponentIds<__VA_ARGS__>(); }   \
	static SystemRegisterer<type> _RegisterSystem_##type;

#define DECL_SYSTEM_TEMPLATED(type, template_type, ...)                                           \
	template<> constexpr const char *GetSystemName<type<template_type>>()                         \
	{ return #type "<" #template_type ">"; }                                                      \
	template<> CompIds GetSystemComponentIds<type<template_type>>()                               \
	{ return GetComponentIds<__VA_ARGS__>(); }                                                    \
	static SystemRegisterer<type<template_type>> _RegisterSystem_##type##_##template_type;

#define DECL_COMPONENT(type)                                                                      \
	template<> constexpr const char *GetComponentName<type>() { return #type; }                   \
	static ComponentRegisterer<type> _RegisterComponent_##type;

#define DECL_COMPONENT_TEMPLATED(type, template_type)                                             \
	template<> constexpr const char *GetComponentName<type<template_type>>()                      \
	{ return #type "<" #template_type ">"; }                                                      \
	static ComponentRegisterer<type<template_type>> _RegisterComponent_##type##_##template_type;

#include "game/Registration.hpp"