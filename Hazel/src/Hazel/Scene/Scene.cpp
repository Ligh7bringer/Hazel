#include "Scene.hpp"

#include "Components.hpp"
#include "Hazel/Renderer/Renderer2D.hpp"

namespace Hazel
{

Scene::Scene() { entt::entity entity = m_Registry.create(); }

Scene::~Scene() { }

entt::entity Scene::CreateEntity() { return m_Registry.create(); }

void Scene::OnUpdate(Timestep ts)
{
	auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
	for(auto entity : group)
	{
		const auto& [transform, sprite] =
			group.get<TransformComponent, SpriteRendererComponent>(entity);

		Renderer2D::DrawQuad(transform, sprite.Colour);
	}
}

} // namespace Hazel