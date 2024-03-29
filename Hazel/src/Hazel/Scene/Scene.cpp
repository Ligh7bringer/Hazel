#include "Scene.hpp"

#include "Components.hpp"
#include "Entity.hpp"
#include "Hazel/Renderer/Renderer2D.hpp"
#include "ScriptableEntity.hpp"

#include <box2d/b2_body.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_world.h>

namespace Hazel
{

static b2BodyType HazelToBox2DRigidbodyType(Rigidbody2DComponent::BodyType bodyType)
{
	switch(bodyType)
	{
	case Rigidbody2DComponent::BodyType::Static: return b2_staticBody;
	case Rigidbody2DComponent::BodyType::Dynamic: return b2_dynamicBody;
	case Rigidbody2DComponent::BodyType::Kinematic: return b2_kinematicBody;
	}

	HZ_CORE_ASSERT(false, "Invalid body type");
	return b2_staticBody;
}

Scene::Scene() { }

Scene::~Scene() { }

Entity Scene::CreateEntity(const std::string& name) { return CreateEntityWithUUID(UUID(), name); }

Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name)
{
	Entity entity = {m_Registry.create(), this};
	entity.AddComponent<IDComponent>(uuid);
	entity.AddComponent<TransformComponent>();
	auto& tag = entity.AddComponent<TagComponent>();
	tag.Tag = name.empty() ? "Entity" : name;

	return entity;
}

void Scene::DestroyEntity(Entity entity) { m_Registry.destroy(entity); }

void Scene::OnRuntimeStart()
{
	m_PhysicsWorld = new b2World({0.f, -9.8f});

	const auto& view = m_Registry.view<Rigidbody2DComponent>();
	for(auto e : view)
	{
		Entity entity = {e, this};
		auto& transform = entity.GetComponent<TransformComponent>();
		auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

		b2BodyDef bodyDef;
		bodyDef.type = HazelToBox2DRigidbodyType(rb2d.Type);
		bodyDef.position.Set(transform.Translation.x, transform.Translation.y);
		bodyDef.angle = transform.Rotation.z;
		b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);
		body->SetFixedRotation(rb2d.FixedRotation);
		rb2d.RuntimeBody = body;

		if(entity.HasComponent<BoxCollider2DComponent>())
		{
			auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();

			b2PolygonShape shape;
			shape.SetAsBox(transform.Scale.x * bc2d.Size.x, transform.Scale.y * bc2d.Size.y);

			b2FixtureDef fixtureDef;
			fixtureDef.shape = &shape;
			fixtureDef.density = bc2d.Density;
			fixtureDef.friction = bc2d.Friction;
			fixtureDef.restitution = bc2d.Restitution;
			fixtureDef.restitutionThreshold = bc2d.RestitutionThreshold;
			body->CreateFixture(&fixtureDef);
		}
	}
}

void Scene::OnRuntimeStop()
{
	delete m_PhysicsWorld;
	m_PhysicsWorld = nullptr;
}

void Scene::OnUpdateEditor(Timestep, EditorCamera& camera)
{
	Renderer2D::BeginScene(camera);

	auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
	for(auto entity : group)
	{
		auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

		Renderer2D::DrawSprite(transform.GetTransform(), sprite, static_cast<int>(entity));
	}

	Renderer2D::EndScene();
}

void Scene::OnUpdateRuntime(Timestep ts)
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

	// Physics
	{
		constexpr int32_t velocityIterations = 6;
		constexpr int32_t positionIterations = 2;
		m_PhysicsWorld->Step(ts, velocityIterations, positionIterations);

		const auto& view = m_Registry.view<Rigidbody2DComponent>();
		for(auto e : view)
		{
			Entity entity = {e, this};
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

			b2Body* body = reinterpret_cast<b2Body*>(rb2d.RuntimeBody);
			const auto& position = body->GetPosition();
			transform.Translation.x = position.x;
			transform.Translation.y = position.y;
			transform.Rotation.z = body->GetAngle();
		}
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

			Renderer2D::DrawSprite(transform.GetTransform(), sprite, static_cast<int>(entity));
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

Entity Scene::GetPrimaryCameraEntity()
{
	auto view = m_Registry.view<CameraComponent>();
	for(auto entity : view)
	{
		const auto& camera = view.get<CameraComponent>(entity);
		if(camera.Primary) return Entity{entity, this};
	}

	return {};
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
DECLARE_ADD_COMPONENT_CALLBACK_STUB(Rigidbody2DComponent)
DECLARE_ADD_COMPONENT_CALLBACK_STUB(BoxCollider2DComponent)
DECLARE_ADD_COMPONENT_CALLBACK_STUB(IDComponent)

#undef DECLARE_ADD_COMPONENT_CALLBACK_STUB

template <>
void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
{
	component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
}

} // namespace Hazel
