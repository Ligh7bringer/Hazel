#pragma once

#include "Scene.hpp"

#include "entt/entt.hpp"

namespace Hazel
{

class Entity
{
public:
	Entity() = default;
	Entity(entt::entity handle, Scene* scene);

	template <typename T, typename... Args>
	T& AddComponent(Args&&... args)
	{
		HZ_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
		return m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
	}

	template <typename T>
	T& GetComponent()
	{
		HZ_CORE_ASSERT(HasComponent<T>(), "Entity doesn't have requested component!");
		return m_Scene->m_Registry.get<T>(m_EntityHandle);
	}

	template <typename T>
	bool HasComponent()
	{
		return m_Scene->m_Registry.has<T>(m_EntityHandle);
	}

	template <typename T>
	void RemoveComponent()
	{
		HZ_CORE_ASSERT(HasComponent<T>(), "Entity doesn't have requested component!");
		m_Scene->m_Registry.remove<T>(m_EntityHandle);
	}

	// operator bool() const { return m_EntityHandle != 0; }

private:
	entt::entity m_EntityHandle{0};
	Scene* m_Scene{nullptr};
};

} // namespace Hazel