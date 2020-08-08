#include "Scene.hpp"

#include "Components.hpp"
#include "Entity.hpp"
#include "Hazel/Renderer/Renderer2D.hpp"

namespace Hazel
{

Scene::Scene() { entt::entity entity = m_Registry.create(); }

Scene::~Scene() { }

Entity Scene::CreateEntity(const std::string& name)
{
	Entity entity = {m_Registry.create(), this};
	entity.AddComponent<TransformComponent>();
	auto& tag = entity.AddComponent<TagComponent>();
	tag.Tag = name.empty() ? "Entity" : name;

	return entity;
}

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