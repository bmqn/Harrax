#pragma once

#include "game/Entity.hpp"
#include "game/System.hpp"

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
		sys->SetEntities(
			m_EntityManager.View(GetSystemComponentIds<Sys>())
		);
		return sys;
	}

	template<typename Comp>
	bool HasComponent(uint32_t entityId)
	{
		return m_EntityManager.HasComponent<Comp>(entityId);
	}

	template<typename Comp>
	void AddComponent(uint32_t entityId, const Comp& component)
	{
		m_EntityManager.AddComponent<Comp>(entityId, component);
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

private:
	Registry() = default;

private:
	EntityManager m_EntityManager;
	SystemManager m_SystemManager;
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

#define DECL_COMPONENT(type)                                                                      \
	template<> constexpr const char *GetComponentName<type>() { return #type; }                   \
	static ComponentRegisterer<type> _RegisterComponent_##type;

#include "game/Registration.hpp"