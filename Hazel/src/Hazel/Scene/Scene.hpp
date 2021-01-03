#pragma once

#include <string>

#include "entt/entt.hpp"

#include "Hazel/Core/Timestep.hpp"
#include "Hazel/Renderer/EditorCamera.hpp"
#include "SceneSerializer.hpp"

namespace Hazel
{

class Entity;

class Scene
{
public:
	Scene();
	~Scene();

	Entity CreateEntity(const std::string& name = std::string());
	void DestroyEntity(Entity entity);

	void OnUpdateEditor(Timestep ts, EditorCamera& camera);
	void OnUpdateRuntime(Timestep ts);
	void OnViewportResize(uint32_t width, uint32_t height);

	Entity GetPrimaryCameraEntity();

private:
	template <typename T>
	void OnComponentAdded(Entity entity, T& component);

private:
	entt::registry m_Registry;
	uint32_t m_ViewportWidth{0}, m_ViewportHeight{0};

	friend class Entity;
	friend class SceneHierarchyPanel;
	friend class SceneSerializer;
};

} // namespace Hazel
