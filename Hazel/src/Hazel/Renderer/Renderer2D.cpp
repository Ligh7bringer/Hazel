#include "Renderer2D.h"

#include "RenderCommand.h"
#include "Shader.h"
#include "VertexArray.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Hazel
{

float Renderer2D::noRotation = 0.f;

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
	static const uint32_t MaxQuads = 20000;
	static const uint32_t MaxVertices = MaxQuads * 4;
	static const uint32_t MaxIndices = MaxQuads * 6;
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

static Renderer2DData* s_Data;

void Renderer2D::Init()
{
	HZ_PROFILE_FUNCTION();

	s_Data = new Renderer2DData();
	s_Data->QuadVertexArray = VertexArray::Create();
	s_Data->QuadVertexBuffer = VertexBuffer::Create(s_Data->MaxVertices * sizeof(QuadVertex));
	s_Data->QuadVertexBuffer->SetLayout({{ShaderDataType::Float3, "a_Position"},
										 {ShaderDataType::Float4, "a_Colour"},
										 {ShaderDataType::Float2, "a_TexCoord"},
										 {ShaderDataType::Float, "a_TexIndex"},
										 {ShaderDataType::Float, "a_TilingFactor"}});
	s_Data->QuadVertexArray->AddVertexBuffer(s_Data->QuadVertexBuffer);

	s_Data->QuadVertexBufferBase = new QuadVertex[s_Data->MaxVertices];

	uint32_t* quadIndices = new uint32_t[s_Data->MaxIndices];

	uint32_t offset = 0;
	for(uint32_t i = 0; i < s_Data->MaxIndices; i += 6)
	{
		quadIndices[i + 0] = offset + 0;
		quadIndices[i + 1] = offset + 1;
		quadIndices[i + 2] = offset + 2;

		quadIndices[i + 3] = offset + 2;
		quadIndices[i + 4] = offset + 3;
		quadIndices[i + 5] = offset + 0;

		offset += 4;
	}

	Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, s_Data->MaxIndices);
	s_Data->QuadVertexArray->SetIndexBuffer(quadIB);
	delete[] quadIndices;

	s_Data->WhiteTexture = Texture2D::Create(1, 1);
	uint32_t whiteTexData = 0xffffffff;
	s_Data->WhiteTexture->SetData(&whiteTexData, sizeof(whiteTexData));

	int32_t samplers[s_Data->MaxTextureSlots];
	for(uint32_t i = 0; i < s_Data->MaxTextureSlots; ++i)
	{
		samplers[i] = i;
	}

	s_Data->TextureShader = Shader::Create("assets/shaders/Texture.glsl");
	s_Data->TextureShader->Bind();
	s_Data->TextureShader->SetIntArray("u_Textures", samplers, s_Data->MaxTextureSlots);

	s_Data->TextureSlots[0] = s_Data->WhiteTexture;

	s_Data->QuadVertexPositions[0] = {-0.5f, -0.5f, -0.f, 1.f};
	s_Data->QuadVertexPositions[1] = {0.5f, -0.5f, -0.f, 1.f};
	s_Data->QuadVertexPositions[2] = {0.5f, 0.5f, -0.f, 1.f};
	s_Data->QuadVertexPositions[3] = {-0.5f, 0.5f, -0.f, 1.f};
}

void Renderer2D::Shutdown()
{
	HZ_PROFILE_FUNCTION();
	delete s_Data;
}

void Renderer2D::FlushAndReset()
{
	EndScene();

	s_Data->TextureSlotIndex = 1;
	s_Data->QuadIndexCount = 0;
	s_Data->QuadVertexBufferPtr = s_Data->QuadVertexBufferBase;
}

void Renderer2D::BeginScene(const OrthographicCamera& camera)
{
	HZ_PROFILE_FUNCTION();

	s_Data->TextureShader->Bind();
	s_Data->TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());

	s_Data->TextureSlotIndex = 1;
	s_Data->QuadIndexCount = 0;
	s_Data->QuadVertexBufferPtr = s_Data->QuadVertexBufferBase;
}

void Renderer2D::EndScene()
{
	HZ_PROFILE_FUNCTION();

	uint32_t dataSize =
		(uint8_t*)s_Data->QuadVertexBufferPtr - (uint8_t*)s_Data->QuadVertexBufferBase;
	s_Data->QuadVertexBuffer->SetData(s_Data->QuadVertexBufferBase, dataSize);

	Flush();
}

void Renderer2D::Flush()
{
	for(uint32_t i = 0; i < s_Data->TextureSlotIndex; ++i)
	{
		s_Data->TextureSlots[i]->Bind(i);
	}

	RenderCommand::DrawIndexed(s_Data->QuadVertexArray, s_Data->QuadIndexCount);
	s_Data->Stats.DrawCalls++;
}

void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& colour)
{
	DrawQuad({position.x, position.y, 0.f}, size, colour);
}

void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& colour)
{
	HZ_PROFILE_FUNCTION();

	constexpr float textureIndex = 0.f; // Default texture
	constexpr float tilingFactor = 1.f;
	const auto transform = ComputeTransformationMatrix(position, noRotation, size);

	InitVertexBuffer(transform, position, size, colour, textureIndex, tilingFactor);
}

void Renderer2D::DrawQuad(const glm::vec2& position,
						  const glm::vec2& size,
						  const Ref<Texture2D>& texture,
						  float tilingFactor,
						  glm::vec4 tintColour)
{
	HZ_PROFILE_FUNCTION();
	DrawQuad({position.x, position.y, 0.f}, size, texture, tilingFactor, tintColour);
}

void Renderer2D::DrawQuad(const glm::vec3& position,
						  const glm::vec2& size,
						  const Ref<Texture2D>& texture,
						  float tilingFactor,
						  glm::vec4 tintColour)
{
	HZ_PROFILE_FUNCTION();

	constexpr glm::vec4 colour{1.f};
	const auto transform = ComputeTransformationMatrix(position, noRotation, size);
	const float textureIndex = GetTextureIndex(texture);

	InitVertexBuffer(transform, position, size, colour, textureIndex, tilingFactor);
}

void Renderer2D::DrawRotatedQuad(const glm::vec2& position,
								 const glm::vec2& size,
								 float rotation,
								 const glm::vec4& colour)
{
	HZ_PROFILE_FUNCTION();
	DrawRotatedQuad({position.x, position.y, 0.f}, size, rotation, colour);
}

void Renderer2D::DrawRotatedQuad(const glm::vec3& position,
								 const glm::vec2& size,
								 float rotation,
								 const glm::vec4& colour)
{
	HZ_PROFILE_FUNCTION();

	constexpr float textureIndex = 0.f; // Default texture
	constexpr float tilingFactor = 1.f;
	const auto transform = ComputeTransformationMatrix(position, rotation, size);

	InitVertexBuffer(transform, position, size, colour, textureIndex, tilingFactor);
}

void Renderer2D::DrawRotatedQuad(const glm::vec2& position,
								 const glm::vec2& size,
								 float rotation,
								 const Ref<Texture2D>& texture,
								 float tilingFactor /*= 1.f*/,
								 glm::vec4 tintColour)
{
	HZ_PROFILE_FUNCTION();
	DrawRotatedQuad(
		{position.x, position.y, 0.f}, size, rotation, texture, tilingFactor, tintColour);
}

void Renderer2D::DrawRotatedQuad(const glm::vec3& position,
								 const glm::vec2& size,
								 float rotation,
								 const Ref<Texture2D>& texture,
								 float tilingFactor /*= 1.f*/,
								 glm::vec4 tintColour)
{
	HZ_PROFILE_FUNCTION();

	constexpr glm::vec4 colour{1.f};
	const auto transform = ComputeTransformationMatrix(position, rotation, size);
	const float textureIndex = GetTextureIndex(texture);

	InitVertexBuffer(transform, position, size, colour, textureIndex, tilingFactor);
}

void Renderer2D::ResetStats() { memset(&s_Data->Stats, 0, sizeof(Statistics)); }

Renderer2D::Statistics Renderer2D::GetStats() { return s_Data->Stats; }

glm::mat4 Renderer2D::ComputeTransformationMatrix(const glm::vec3& position,
												  float rotation,
												  const glm::vec2& size)
{
	return glm::translate(glm::mat4{1.f}, position) *
		   glm::rotate(glm::mat4{1.f}, glm::radians(rotation), {0.f, 0.f, 1.f}) *
		   glm::scale(glm::mat4{1.f}, {size.x, size.y, 1.f});
}

float Renderer2D::GetTextureIndex(const Ref<Texture2D>& texture)
{
	float textureIndex = 0.f;
	for(uint32_t i = 1; i < s_Data->TextureSlotIndex; ++i)
	{
		if(*s_Data->TextureSlots[i] == *texture)
		{
			textureIndex = static_cast<float>(i);
			break;
		}
	}

	if(textureIndex == 0.f)
	{
		textureIndex = static_cast<float>(s_Data->TextureSlotIndex);
		s_Data->TextureSlots[s_Data->TextureSlotIndex] = texture;
		s_Data->TextureSlotIndex++;
	}

	return textureIndex;
}

void Renderer2D::InitVertexBuffer(const glm::mat4& transform,
								  const glm::vec3& position,
								  const glm::vec2& size,
								  const glm::vec4& colour,
								  float textureIndex,
								  float tilingFactor)
{
	if(s_Data->QuadIndexCount >= Renderer2DData::MaxIndices)
	{
		FlushAndReset();
	}

	// Bottom left
	s_Data->QuadVertexBufferPtr->Position = transform * s_Data->QuadVertexPositions[0];
	;
	s_Data->QuadVertexBufferPtr->Colour = colour;
	s_Data->QuadVertexBufferPtr->TexCoord = {0.f, 0.f};
	s_Data->QuadVertexBufferPtr->TexIndex = textureIndex;
	s_Data->QuadVertexBufferPtr->TilingFactor = tilingFactor;
	s_Data->QuadVertexBufferPtr++;

	// Bottom right
	s_Data->QuadVertexBufferPtr->Position = transform * s_Data->QuadVertexPositions[1];
	s_Data->QuadVertexBufferPtr->Colour = colour;
	s_Data->QuadVertexBufferPtr->TexCoord = {1.f, 0.f};
	s_Data->QuadVertexBufferPtr->TexIndex = textureIndex;
	s_Data->QuadVertexBufferPtr->TilingFactor = tilingFactor;
	s_Data->QuadVertexBufferPtr++;

	// Top right
	s_Data->QuadVertexBufferPtr->Position = transform * s_Data->QuadVertexPositions[2];
	s_Data->QuadVertexBufferPtr->Colour = colour;
	s_Data->QuadVertexBufferPtr->TexCoord = {1.f, 1.f};
	s_Data->QuadVertexBufferPtr->TexIndex = textureIndex;
	s_Data->QuadVertexBufferPtr->TilingFactor = tilingFactor;
	s_Data->QuadVertexBufferPtr++;

	// Top left
	s_Data->QuadVertexBufferPtr->Position = transform * s_Data->QuadVertexPositions[3];
	s_Data->QuadVertexBufferPtr->Colour = colour;
	s_Data->QuadVertexBufferPtr->TexCoord = {0.f, 1.f};
	s_Data->QuadVertexBufferPtr->TexIndex = textureIndex;
	s_Data->QuadVertexBufferPtr->TilingFactor = tilingFactor;
	s_Data->QuadVertexBufferPtr++;

	s_Data->QuadIndexCount += 6;
	s_Data->Stats.QuadCount++;
}

} // namespace Hazel
