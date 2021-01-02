#pragma once

#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "SceneCamera.hpp"
#include "ScriptableEntity.hpp"

namespace Hazel
{

struct TagComponent
{
	std::string Tag;
	TagComponent() = default;
	TagComponent(const TagComponent&) = default;
	explicit TagComponent(const std::string& tag)
		: Tag(tag)
	{ }
};

struct TransformComponent
{
	glm::vec3 Translation{0.f, 0.f, 0.f};
	glm::vec3 Rotation{0.f, 0.f, 0.f};
	glm::vec3 Scale{1.f, 1.f, 1.f};

	TransformComponent() = default;
	TransformComponent(const TransformComponent&) = default;
	explicit TransformComponent(const glm::vec3& translation)
		: Translation(translation)
	{ }

	glm::mat4 GetTransform() const
	{
		glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

		return glm::translate(glm::mat4(1.f), Translation) * rotation *
			   glm::scale(glm::mat4(1.f), Scale);
	}
};

struct SpriteRendererComponent
{
	glm::vec4 Colour{1.f, 1.f, 1.f, 1.f};

	SpriteRendererComponent() = default;
	SpriteRendererComponent(const SpriteRendererComponent&) = default;
	explicit SpriteRendererComponent(const glm::vec4& colour)
		: Colour(colour)
	{ }
};

struct CameraComponent
{
	SceneCamera Camera;
	bool Primary{true}; // FIXME: Maybe move to Scene
	bool FixedAspectRatio{false};

	CameraComponent() = default;
	CameraComponent(const CameraComponent&) = default;
};

struct NativeScriptComponent
{
	ScriptableEntity* Instance = nullptr;

	ScriptableEntity* (*InstantiateScript)();
	void (*DestroyScript)(NativeScriptComponent*);

	template <typename T>
	void Bind()
	{
		// FIXME: Constructor can't have parameters
		InstantiateScript = []() -> ScriptableEntity* { return new T(); };

		// FIXME: Actually call this function to free the allocated memory for Instance
		DestroyScript = [](NativeScriptComponent* nsc) -> void {
			delete nsc->Instance;
			nsc->Instance = nullptr;
		};
	}
};

} // namespace Hazel