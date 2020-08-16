#pragma once

#include <glm/glm.hpp>

#include <string>

#include "Hazel/Renderer/Camera.hpp"

namespace Hazel
{

struct TagComponent
{
	std::string Tag;
	TagComponent() = default;
	TagComponent(const TagComponent&) = default;
	TagComponent(const std::string& tag)
		: Tag(tag)
	{}
};

struct TransformComponent
{
	glm::mat4 Transform{1.f};

	TransformComponent() = default;
	TransformComponent(const TransformComponent&) = default;
	TransformComponent(const glm::mat4& transform)
		: Transform(transform)
	{}

	operator glm::mat4&() { return Transform; }
	operator const glm::mat4&() const { return Transform; }
};

struct SpriteRendererComponent
{
	glm::vec4 Colour{1.f, 1.f, 1.f, 1.f};

	SpriteRendererComponent() = default;
	SpriteRendererComponent(const SpriteRendererComponent&) = default;
	SpriteRendererComponent(const glm::vec4& colour)
		: Colour(colour)
	{}
};

struct CameraComponent
{
	Hazel::Camera Camera;
	bool Primary{true}; // FIXME: Maybe move to Scene

	CameraComponent() = default;
	CameraComponent(const CameraComponent&) = default;
	CameraComponent(const glm::mat4& projection)
		: Camera(projection)
	{}
};

} // namespace Hazel