#pragma once

#include "game/Entity.hpp"

#include <functional>
#include <tuple>

class Registry
{
	template<typename> friend class ComponentRegisterer;

public:
	static Registry *Get()
	{
		static Registry reg;
		return &reg;
	}

public:
	EntId Create()
	{
		return m_EntityManager.Create();
	}

	template<typename Comp>
	bool HasComponent(EntId id)
	{
		return m_EntityManager.HasComponent<Comp>(id);
	}

	template<typename Comp, typename... Args>
	void AddComponent(EntId id, Args &&...args)
	{
		m_EntityManager.AddComponent<Comp>(id, std::forward<Args>(args)...);
	}

	template<typename Comp>
	Comp &GetComponent(EntId id)
	{
		return m_EntityManager.GetComponent<Comp>(id);
	}

	template<typename... Comps, typename Func>
	void View(const Func &func)
	{
		auto compMask = m_EntityManager.GetMask<Comps...>();
		for (auto entity : m_EntityManager.m_Entities)
		{
			if (compMask == (entity.Mask & compMask))
			{
				std::apply(func, std::tuple_cat(
					std::make_tuple(entity.Id),
					std::forward_as_tuple(m_EntityManager.GetComponent<Comps>(entity.Id)...)
				));
			}
		}
	}

private:
	Registry() = default;

private:
	EntityManager m_EntityManager;
};

template <typename T = uint32_t, std::size_t N>
constexpr T StrHash(char const(&s)[N]) noexcept
{
	T val{};
	for (size_t i = 0; i < N; ++i)
	{
		val |= s[i] << (i * CHAR_BIT);
	}
	return val;
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

#define DECL_COMPONENT(type)                                                                      \
	template<> constexpr const char *GetComponentName<type>() { return #type; }                   \
	template<> constexpr uint32_t GetComponentId<type>() { return StrHash(#type); }               \
	static ComponentRegisterer<type> _RegisterComponent_##type;

#include "game/Registration.hpp"