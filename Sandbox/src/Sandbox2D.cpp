#include "hzpch.h"

#include "Sandbox2D.h"

#include "imgui/imgui.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Platform/OpenGL/OpenGLShader.h"

Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D")
	, m_CameraController(1280.f / 720.f)
{}

void Sandbox2D::OnAttach()
{
	m_SquareVA = Hazel::VertexArray::Create();

	float squareVertices[5 * 4] = {
		-0.5f, -0.5f, 0.f, 0.5f, -0.5f, 0.f, 0.5f, 0.5f, 0.f, -0.5f, 0.5f, 0.f};

	Hazel::Ref<Hazel::VertexBuffer> squareVB;
	squareVB.reset(Hazel::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
	squareVB->SetLayout({{Hazel::ShaderDataType::Float3, "a_Position"}});
	m_SquareVA->AddVertexBuffer(squareVB);

	uint32_t squareIndices[6] = {0, 1, 2, 2, 3, 0};
	Hazel::Ref<Hazel::IndexBuffer> squareIB;
	squareIB.reset(
		Hazel::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
	m_SquareVA->SetIndexBuffer(squareIB);

	m_FlatColShader = Hazel::Shader::Create("assets/shaders/FlatCol.glsl");
}

void Sandbox2D::OnUpdate(Hazel::Timestep dt)
{
	// Update
	m_CameraController.OnUpdate(dt);

	// Render
	Hazel::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.f});
	Hazel::RenderCommand::Clear();

	Hazel::Renderer::BeginScene(m_CameraController.GetCamera());

	static glm::mat4 scale = glm::scale(glm::mat4(1.f), glm::vec3(0.1f));

	std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatColShader)->Bind();
	std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatColShader)
		->UploadUniformFloat4("u_Colour", m_SquareColor);

	Hazel::Renderer::Submit(
		m_FlatColShader, m_SquareVA, glm::scale(glm::mat4(1.f), glm::vec3(1.5f)));

	Hazel::Renderer::EndScene();
}

void Sandbox2D::OnDetach() {}

void Sandbox2D::OnImGuiRender()
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Square colour", glm::value_ptr(m_SquareColor));
	ImGui::End();
}

void Sandbox2D::OnEvent(Hazel::Event& event) { m_CameraController.OnEvent(event); }
