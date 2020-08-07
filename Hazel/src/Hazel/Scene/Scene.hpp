#pragma once

#include "entt/entt.hpp"

#include "Hazel/Core/Timestep.hpp"

namespace Hazel
{

class Scene
{
public:
	Scene();
	~Scene();

	entt::entity CreateEntity();

	void OnUpdate(Timestep ts);

	// FIXME: Only temporary
	entt::registry& Reg() { return m_Registry; }

private:
	entt::registry m_Registry;
};

} // namespace Hazel
