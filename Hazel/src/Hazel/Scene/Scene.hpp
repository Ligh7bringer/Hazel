#pragma once

#include "entt/entt.hpp"

#include "Hazel/Core/Timestep.hpp"

#include <string>

namespace Hazel
{

class Entity;

class Scene
{
public:
	Scene();
	~Scene();

	Entity CreateEntity(const std::string& name = std::string());

	void OnUpdate(Timestep ts);

	// FIXME: Only temporary
	entt::registry& Reg() { return m_Registry; }

private:
	entt::registry m_Registry;

	friend class Entity;
};

} // namespace Hazel
