#pragma once

#include "Hazel/Renderer/Camera.hpp"
#include "OrthographicCamera.hpp"
#include "Shader.hpp"
#include "SubTexture2D.hpp"
#include "Texture.hpp"
#include "VertexArray.hpp"

namespace Hazel
{

class Renderer2D
{
public:
	static void Init();
	static void Shutdown();

	static void BeginScene(const OrthographicCamera& camera);
	static void BeginScene(const Camera& camera, const glm::mat4& transform);
	static void EndScene();
	static void Flush();

	static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& colour);
	static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& colour);
	// Textures
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
	// Subtextures
	static void DrawQuad(const glm::vec2& position,
						 const glm::vec2& size,
						 const Ref<SubTexture2D>& subtexture,
						 float tilingFactor = 1.f,
						 glm::vec4 tintColour = glm::vec4(1.f));
	static void DrawQuad(const glm::vec3& position,
						 const glm::vec2& size,
						 const Ref<SubTexture2D>& subtexture,
						 float tilingFactor = 1.f,
						 glm::vec4 tintColour = glm::vec4(1.f));

	// Matrix
	static void DrawQuad(const glm::mat4& transform, const glm::vec4& colour);
	static void DrawQuad(const glm::mat4& transform,
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
	// Textures
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
	// Subtextures
	static void DrawRotatedQuad(const glm::vec2& position,
								const glm::vec2& size,
								float rotation,
								const Ref<SubTexture2D>& subtexture,
								float tilingFactor = 1.f,
								glm::vec4 tintColour = glm::vec4(1.f));
	static void DrawRotatedQuad(const glm::vec3& position,
								const glm::vec2& size,
								float rotation,
								const Ref<SubTexture2D>& subtexture,
								float tilingFactor = 1.f,
								glm::vec4 tintColour = glm::vec4(1.f));

	struct Statistics
	{
		uint32_t DrawCalls = 0;
		uint32_t QuadCount = 0;

		uint32_t GetTotalVertexCount() { return QuadCount * 4; }
		uint32_t GetTotalIndexCount() { return QuadCount * 6; }
	};
	static Statistics GetStats();
	static void ResetStats();

	static void SetMaxQuadsPerDrawCall(uint32_t value);

private:
	static void FlushAndReset();

	static glm::mat4
	ComputeTransformationMatrix(const glm::vec3& position, float rotation, const glm::vec2& size);

	static float GetTextureIndex(const Ref<Texture2D>& texture);

	static void InitVertexBuffer(const glm::mat4& transform,
								 const glm::vec4& colour,
								 float textureIndex,
								 float tilingFactor,
								 const glm::vec2 textureCoords[4] = nullptr);

	static float noRotation;
};

struct QuadVertex
{
	glm::vec3 Position;
	glm::vec4 Colour;
	glm::vec2 TexCoord;
	float TexIndex;
	float TilingFactor;
};

struct Renderer2DData
{
	// Max allowed per batch
	static uint32_t MaxQuads;
	static uint32_t MaxVertices;
	static uint32_t MaxIndices;
	// TODO: Query driver for what the GPU actually supports
	static const uint32_t MaxTextureSlots = 32;

	Ref<VertexArray> QuadVertexArray;
	Ref<VertexBuffer> QuadVertexBuffer;
	Ref<Shader> TextureShader;
	Ref<Texture2D> WhiteTexture;

	uint32_t QuadIndexCount = 0;
	QuadVertex* QuadVertexBufferBase = nullptr;
	QuadVertex* QuadVertexBufferPtr = nullptr;

	std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
	uint32_t TextureSlotIndex = 1; // 0 = Default texture

	glm::vec4 QuadVertexPositions[4];

	Renderer2D::Statistics Stats;
};

} // namespace Hazel
