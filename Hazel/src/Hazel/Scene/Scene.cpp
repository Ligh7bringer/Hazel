#include "Scene.hpp"

#include "Components.hpp"
#include "Entity.hpp"
#include "Hazel/Renderer/Renderer2D.hpp"

namespace Hazel
{

Scene::Scene() { }

Scene::~Scene() { }

Entity Scene::CreateEntity(const std::string& name)
{
	Entity entity = {m_Registry.create(), this};
	entity.AddComponent<TransformComponent>();
	auto& tag = entity.AddComponent<TagComponent>();
	tag.Tag = name.empty() ? "Entity" : name;

	return entity;
}

void Scene::DestroyEntity(Entity entity) { m_Registry.destroy(entity); }

void Scene::OnUpdate(Timestep ts)
{
	// Update scripts
	{
		// FIXME: This should happen during runtime when it is implemented
		m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc) {
			if(!nsc.Instance)
			{
				nsc.Instance = nsc.InstantiateScript();
				nsc.Instance->m_Entity = Entity{entity, this};
				nsc.Instance->OnCreate();
			}

			nsc.Instance->OnUpdate(ts);
		});
	}

	Camera* mainCamera = nullptr;
	glm::mat4 cameraTransform;
	{
		auto view = m_Registry.view<CameraComponent, TransformComponent>();
		for(auto entity : view)
		{
			auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

			if(camera.Primary)
			{
				mainCamera = &camera.Camera;
				cameraTransform = transform.GetTransform();
				break;
			}
		}
	}

	if(mainCamera)
	{
		Renderer2D::BeginScene(*mainCamera, cameraTransform);

		auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for(auto entity : group)
		{
			auto [transform, sprite] =
				group.get<TransformComponent, SpriteRendererComponent>(entity);

			Renderer2D::DrawQuad(transform.GetTransform(), sprite.Colour);
		}

		Renderer2D::EndScene();
	}
}

void Scene::OnViewportResize(uint32_t width, uint32_t height)
{
	m_ViewportWidth = width;
	m_ViewportHeight = height;

	// Resize non-FixedAspectRatio cameras
	auto view = m_Registry.view<CameraComponent>();
	for(auto entity : view)
	{
		auto& cameraComponent = view.get<CameraComponent>(entity);
		if(!cameraComponent.FixedAspectRatio)
		{
			cameraComponent.Camera.SetViewportSize(width, height);
		}
	}
}

template <typename T>
void Scene::OnComponentAdded(Entity entity, T& component)
{
	HZ_CORE_ERROR("Unreachable!");
}

#define DECLARE_ADD_COMPONENT_CALLBACK_STUB(COMPONENT_TYPE)                                        \
	template <>                                                                                    \
	void Scene::OnComponentAdded<COMPONENT_TYPE>(Entity entity, COMPONENT_TYPE & component)        \
	{ }

DECLARE_ADD_COMPONENT_CALLBACK_STUB(TransformComponent)
DECLARE_ADD_COMPONENT_CALLBACK_STUB(SpriteRendererComponent)
DECLARE_ADD_COMPONENT_CALLBACK_STUB(TagComponent)
DECLARE_ADD_COMPONENT_CALLBACK_STUB(NativeScriptComponent)

#undef DECLARE_ADD_COMPONENT_CALLBACK_STUB

template <>
void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
{
	component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
}

} // namespace Hazel
