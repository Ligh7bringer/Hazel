#include <Hazel.h>
#include <Hazel/Core/EntryPoint.h>

#include "imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Platform/OpenGL/OpenGLShader.h"
#include "Sandbox2D.h"

class ExampleLayer : public Hazel::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
		, m_CameraController(1280.f / 720.f, true)
	{
		m_VertexArray = Hazel::VertexArray::Create();

		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.2f, 1.f,  0.5f, -0.5f, 0.5f, 0.2f,
			0.8f,  0.2f,  1.f,  0.f,  0.5f, 0.0f, 0.2f, 0.2f, 0.8f,  1.f,
		};

		Hazel::Ref<Hazel::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(Hazel::VertexBuffer::Create(vertices, sizeof(vertices)));
		Hazel::BufferLayout layout = {{Hazel::ShaderDataType::Float3, "a_Position"},
									  {Hazel::ShaderDataType::Float4, "a_Color"}};
		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		uint32_t indices[3] = {0, 1, 2};
		Hazel::Ref<Hazel::IndexBuffer> indexBuffer;
		indexBuffer.reset(Hazel::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		m_SquareVA = Hazel::VertexArray::Create();

		float squareVertices[5 * 4] = {-0.5f, -0.5f, 0.f, 0.f, 0.f, 0.5f,  -0.5f, 0.f, 1.f, 0.f,
									   0.5f,  0.5f,  0.f, 1.f, 1.f, -0.5f, 0.5f,  0.f, 0.f, 1.f};

		Hazel::Ref<Hazel::VertexBuffer> squareVB;
		squareVB.reset(Hazel::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		squareVB->SetLayout({{Hazel::ShaderDataType::Float3, "a_Position"},
							 {Hazel::ShaderDataType::Float2, "a_TexCoord"}});
		m_SquareVA->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = {0, 1, 2, 2, 3, 0};
		Hazel::Ref<Hazel::IndexBuffer> squareIB;
		squareIB.reset(
			Hazel::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVA->SetIndexBuffer(squareIB);

		m_TriangleShader = Hazel::Shader::Create("assets/shaders/Basic.glsl");
		m_FlatColShader = Hazel::Shader::Create("assets/shaders/FlatCol.glsl");
		auto textureShader = m_ShaderLibrary.Load("assets/shaders/Texture.glsl");

		m_CheckerTexture = Hazel::Texture2D::Create("assets/textures/checker.png");
		m_DiamondTexture = Hazel::Texture2D::Create("assets/textures/diamond.png");

		std::dynamic_pointer_cast<Hazel::OpenGLShader>(textureShader)->Bind();
		std::dynamic_pointer_cast<Hazel::OpenGLShader>(textureShader)
			->UploadUniformInt("u_Texture", 0);
	}

	void OnUpdate(Hazel::Timestep dt) override
	{
		m_CameraController.OnUpdate(dt);

		Hazel::RenderCommand::SetClearColor(m_ClearColour);
		Hazel::RenderCommand::Clear();

		Hazel::Renderer::BeginScene(m_CameraController.GetCamera());

		static glm::mat4 scale = glm::scale(glm::mat4(1.f), glm::vec3(0.1f));

		std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatColShader)->Bind();
		std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatColShader)
			->UploadUniformFloat4("u_Colour", m_SquareColor);

		for(int y = 0; y < 20; ++y)
		{
			for(int x = 0; x < 20; ++x)
			{
				glm::vec3 position(x * 0.11f, y * 0.11f, 0.f);
				glm::mat4 transform = glm::translate(glm::mat4(1.f), position) * scale;
				Hazel::Renderer::Submit(m_FlatColShader, m_SquareVA, transform);
			}
		}

		auto textureShader = m_ShaderLibrary.Get("Texture");

		m_CheckerTexture->Bind();
		Hazel::Renderer::Submit(
			textureShader, m_SquareVA, glm::scale(glm::mat4(1.f), glm::vec3(1.5f)));
		m_DiamondTexture->Bind();
		Hazel::Renderer::Submit(
			textureShader, m_SquareVA, glm::scale(glm::mat4(1.f), glm::vec3(1.5f)));

		//Hazel::Renderer::Submit(m_Shader, m_VertexArray);

		Hazel::Renderer::EndScene();
	}

	void OnEvent(Hazel::Event& evt) override { m_CameraController.OnEvent(evt); }

	void OnImGuiRender() override
	{
		ImGui::Begin("FPS");
		ImGui::Text("%.3f ms/frame\n%.1f FPS",
					1000.0f / ImGui::GetIO().Framerate,
					ImGui::GetIO().Framerate);
		ImGui::End();

		ImGui::Begin("Settings");

		static ImVec4 color = ImVec4(0.1f, 0.2f, 0.2f, 1.f);
		ImGui::Text("Clear colour:");
		ImGui::ColorEdit4("glClearColor##2f", (float*)&m_ClearColour, ImGuiColorEditFlags_Float);
		ImGui::ColorEdit4("Square colour", glm::value_ptr(m_SquareColor));
		ImGui::End();
	}

private:
	Hazel::ShaderLibrary m_ShaderLibrary;
	Hazel::Ref<Hazel::Shader> m_TriangleShader;
	Hazel::Ref<Hazel::VertexArray> m_VertexArray;

	Hazel::Ref<Hazel::Shader> m_FlatColShader;
	Hazel::Ref<Hazel::VertexArray> m_SquareVA;

	Hazel::Ref<Hazel::Texture2D> m_CheckerTexture, m_DiamondTexture;

	Hazel::OrthographicCameraController m_CameraController;

	glm::vec4 m_SquareColor = {.2f, .3f, .8f, 1.f};
	glm::vec4 m_ClearColour{0.16f, 0.7f, 0.3f, 1.0f};
};

class Sandbox : public Hazel::Application
{
public:
	Sandbox()
	{
		//PushLayer(new ExampleLayer());
		PushLayer(new Sandbox2D());
	}

	~Sandbox() {}
};

Hazel::Application* Hazel::CreateApplication() { return new Sandbox(); }
