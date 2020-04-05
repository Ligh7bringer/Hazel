#pragma once

#include "OrthographicCamera.h"

#include "Texture.h"

namespace Hazel
{

class Renderer2D
{
public:
	static void Init();
	static void Shutdown();

	static void BeginScene(const OrthographicCamera& camera);
	static void EndScene();
	static void Flush();

	static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& colour);
	static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& colour);
	static void DrawQuad(const glm::vec2& position,
						 const glm::vec2& size,
						 const Ref<Texture2D>& texture,
						 float tilingFactor = 1.f,
						 glm::vec4 tintColour = glm::vec4(1.f));
	static void DrawQuad(const glm::vec3& position,
						 const glm::vec2& size,
						 const Ref<Texture2D>& texture,
						 float tilingFactor = 1.f,
						 glm::vec4 tintColour = glm::vec4(1.f));

	static void DrawRotatedQuad(const glm::vec2& position,
								const glm::vec2& size,
								float rotation,
								const glm::vec4& colour);
	static void DrawRotatedQuad(const glm::vec3& position,
								const glm::vec2& size,
								float rotation,
								const glm::vec4& colour);
	static void DrawRotatedQuad(const glm::vec2& position,
								const glm::vec2& size,
								float rotation,
								const Ref<Texture2D>& texture,
								float tilingFactor = 1.f,
								glm::vec4 tintColour = glm::vec4(1.f));
	static void DrawRotatedQuad(const glm::vec3& position,
								const glm::vec2& size,
								float rotation,
								const Ref<Texture2D>& texture,
								float tilingFactor = 1.f,
								glm::vec4 tintColour = glm::vec4(1.f));
};

} // namespace Hazel
