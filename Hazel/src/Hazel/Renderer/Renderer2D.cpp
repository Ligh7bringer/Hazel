#include "Renderer2D.h"

#include "RenderCommand.h"
#include "Shader.h"
#include "VertexArray.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace Hazel
{

struct Renderer2DStorage
{
	Ref<VertexArray> QuadVertexArray;
	Ref<Shader> FlatColShader;
};

static Renderer2DStorage* s_Data;

void Renderer2D::Init()
{
	s_Data = new Renderer2DStorage();
	s_Data->QuadVertexArray = VertexArray::Create();

	float squareVertices[5 * 4] = {
		-0.5f, -0.5f, 0.f, 0.5f, -0.5f, 0.f, 0.5f, 0.5f, 0.f, -0.5f, 0.5f, 0.f};

	Ref<VertexBuffer> squareVB;
	squareVB.reset(VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
	squareVB->SetLayout({{ShaderDataType::Float3, "a_Position"}});
	s_Data->QuadVertexArray->AddVertexBuffer(squareVB);

	uint32_t squareIndices[6] = {0, 1, 2, 2, 3, 0};
	Ref<IndexBuffer> squareIB;
	squareIB.reset(IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
	s_Data->QuadVertexArray->SetIndexBuffer(squareIB);

	s_Data->FlatColShader = Shader::Create("assets/shaders/FlatCol.glsl");
}

void Renderer2D::Shutdown() { delete s_Data; }

void Renderer2D::BeginScene(const OrthographicCamera& camera)
{
	std::dynamic_pointer_cast<OpenGLShader>(s_Data->FlatColShader)->Bind();
	std::dynamic_pointer_cast<OpenGLShader>(s_Data->FlatColShader)
		->UploadUniformMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
	std::dynamic_pointer_cast<OpenGLShader>(s_Data->FlatColShader)
		->UploadUniformMat4("u_Transform", glm::mat4(1.f));
}

void Renderer2D::EndScene() {}

void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& colour)
{
	DrawQuad({position.x, position.y, 0.f}, size, colour);
}

void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& colour)
{
	std::dynamic_pointer_cast<OpenGLShader>(s_Data->FlatColShader)->Bind();
	std::dynamic_pointer_cast<Hazel::OpenGLShader>(s_Data->FlatColShader)
		->UploadUniformFloat4("u_Colour", colour);

	s_Data->QuadVertexArray->Bind();
	RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
}

} // namespace Hazel
