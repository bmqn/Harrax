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
		return m_SystemManager.Create<Sys, Args...>(std::forward<Args>(args)...);
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
		return m_EntityManager.View<Comps>();
	}

	template<typename ...Comps>
	void View(ViewFunc forEach)
	{
		m_EntityManager.View<Comps>(forEach);
	}

private:
	Registry() = default;

private:
	EntityManager m_EntityManager;
	SystemManager m_SystemManager;
};

#include "game/Registration.hpp"