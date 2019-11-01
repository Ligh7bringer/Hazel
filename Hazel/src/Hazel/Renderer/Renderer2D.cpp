#include "Renderer2D.h"

#include "RenderCommand.h"
#include "Shader.h"
#include "VertexArray.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Hazel
{

struct Renderer2DStorage
{
	Ref<VertexArray> QuadVertexArray;
	Ref<Shader> FlatColShader;
	Ref<Shader> TextureShader;
};

static Renderer2DStorage* s_Data;

void Renderer2D::Init()
{
	s_Data = new Renderer2DStorage();
	s_Data->QuadVertexArray = VertexArray::Create();

	float squareVertices[5 * 4] = {-0.5f, -0.5f, 0.f, 0.f, 0.f, 0.5f,  -0.5f, 0.f, 1.f, 0.f,
								   0.5f,  0.5f,	 0.f, 1.f, 1.f, -0.5f, 0.5f,  0.f, 0.f, 1.f};

	Ref<VertexBuffer> squareVB;
	squareVB = VertexBuffer::Create(squareVertices, sizeof(squareVertices));
	squareVB->SetLayout(
		{{ShaderDataType::Float3, "a_Position"}, {ShaderDataType::Float2, "a_TexCoord"}});
	s_Data->QuadVertexArray->AddVertexBuffer(squareVB);

	uint32_t squareIndices[6] = {0, 1, 2, 2, 3, 0};
	Ref<IndexBuffer> squareIB;
	squareIB = IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
	s_Data->QuadVertexArray->SetIndexBuffer(squareIB);

	s_Data->FlatColShader = Shader::Create("assets/shaders/FlatCol.glsl");
	s_Data->TextureShader = Shader::Create("assets/shaders/Texture.glsl");
	s_Data->TextureShader->Bind();
	s_Data->TextureShader->SetInt("u_Texture", 0);
}

void Renderer2D::Shutdown() { delete s_Data; }

void Renderer2D::BeginScene(const OrthographicCamera& camera)
{
	s_Data->FlatColShader->Bind();
	s_Data->FlatColShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());

	s_Data->TextureShader->Bind();
	s_Data->TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
}

void Renderer2D::EndScene() {}

void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& colour)
{
	DrawQuad({position.x, position.y, 0.f}, size, colour);
}

void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& colour)
{
	s_Data->FlatColShader->Bind();
	s_Data->FlatColShader->SetFloat4("u_Colour", colour);

	glm::mat4 transform = glm::translate(glm::mat4(1.f), position) * /* * rotation */
						  glm::scale(glm::mat4(1.f), {size.x, size.y, 1.f});
	s_Data->FlatColShader->SetMat4("u_Transform", transform);

	s_Data->QuadVertexArray->Bind();
	RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
}

void Renderer2D::DrawQuad(const glm::vec2& position,
						  const glm::vec2& size,
						  const Ref<Texture2D>& texture)
{
	DrawQuad({position.x, position.y, 0.f}, size, texture);
}

void Renderer2D::DrawQuad(const glm::vec3& position,
						  const glm::vec2& size,
						  const Ref<Texture2D>& texture)
{
	s_Data->TextureShader->Bind();

	glm::mat4 transform = glm::translate(glm::mat4(1.f), position) * /* * rotation */
						  glm::scale(glm::mat4(1.f), {size.x, size.y, 1.f});
	s_Data->TextureShader->SetMat4("u_Transform", transform);

	texture->Bind();

	s_Data->QuadVertexArray->Bind();
	RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
}

} // namespace Hazel
