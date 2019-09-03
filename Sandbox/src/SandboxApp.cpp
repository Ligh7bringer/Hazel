#include "Platform/OpenGL/OpenGLShader.h"
#include <Hazel.h>

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class ExampleLayer : public Hazel::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
		, m_Camera(-1.6f, 1.6f, -0.9f, 0.9f)
	{
		m_VertexArray.reset(Hazel::VertexArray::Create());

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

		m_SquareVA.reset(Hazel::VertexArray::Create());

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

		m_Shader.reset(Hazel::Shader::Create("assets/shaders/Basic.glsl"));
		m_FlatColShader.reset(Hazel::Shader::Create("assets/shaders/FlatCol.glsl"));
		m_TextureShader.reset(Hazel::Shader::Create("assets/shaders/Texture.glsl"));

		m_Texture = Hazel::Texture2D::Create("assets/textures/checker.png");
		m_DiamondTexture = Hazel::Texture2D::Create("assets/textures/diamond.png");

		std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_TextureShader)->Bind();
		std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_TextureShader)
			->UploadUniformInt("u_Texture", 0);
	}

	void OnUpdate(Hazel::Timestep dt) override
	{
		float time = dt;

		if(Hazel::Input::IsKeyPressed(HZ_KEY_A))
			m_GUIProps.Position.x -= m_CameraSpeed * dt;
		else if(Hazel::Input::IsKeyPressed(HZ_KEY_D))
			m_GUIProps.Position.x += m_CameraSpeed * dt;

		if(Hazel::Input::IsKeyPressed(HZ_KEY_W))
			m_GUIProps.Position.y += m_CameraSpeed * dt;
		else if(Hazel::Input::IsKeyPressed(HZ_KEY_S))
			m_GUIProps.Position.y -= m_CameraSpeed * dt;

		if(Hazel::Input::IsKeyPressed(HZ_KEY_Y))
			m_GUIProps.Rotation += m_CameraRotationSpeed * dt;
		else if(Hazel::Input::IsKeyPressed(HZ_KEY_T))
			m_GUIProps.Rotation -= m_CameraRotationSpeed * dt;

		Hazel::RenderCommand::SetClearColor(m_GUIProps.ClearColour);
		Hazel::RenderCommand::Clear();

		m_Camera.SetPosition(m_GUIProps.Position);
		m_Camera.SetRotation(m_GUIProps.Rotation);

		Hazel::Renderer::BeginScene(m_Camera);

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

		m_Texture->Bind();
		Hazel::Renderer::Submit(
			m_TextureShader, m_SquareVA, glm::scale(glm::mat4(1.f), glm::vec3(1.5f)));
		m_DiamondTexture->Bind();
		Hazel::Renderer::Submit(
			m_TextureShader, m_SquareVA, glm::scale(glm::mat4(1.f), glm::vec3(1.5f)));

		//Hazel::Renderer::Submit(m_Shader, m_VertexArray);

		Hazel::Renderer::EndScene();
	}

	void OnEvent(Hazel::Event& evt) override {}

	void OnImGuiRender() override
	{
		ImGui::Begin("FPS");
		ImGui::Text("%.3f ms/frame\n%.1f FPS",
					1000.0f / ImGui::GetIO().Framerate,
					ImGui::GetIO().Framerate);
		ImGui::End();

		ImGui::Begin("Settings");
		ImGui::Text("Camera position");
		ImGui::SliderFloat("Position.X", &m_GUIProps.Position.x, -2.f, 2.0f);
		ImGui::SliderFloat("Position.Y", &m_GUIProps.Position.y, -2.f, 2.0f);
		ImGui::SliderFloat("Position.Z", &m_GUIProps.Position.z, -2.f, 2.0f);

		ImGui::Dummy(ImVec2(0.0f, 20.0f));

		ImGui::Text("Camera rotation");
		ImGui::SliderFloat("Rotation.Z", &m_GUIProps.Rotation, 0.0f, 360.f);

		ImGui::Dummy(ImVec2(0.0f, 20.0f));

		static ImVec4 color = ImVec4(0.1f, 0.2f, 0.2f, 1.f);
		ImGui::Text("Clear colour:");
		ImGui::ColorEdit4(
			"glClearColor##2f", (float*)&m_GUIProps.ClearColour, ImGuiColorEditFlags_Float);
		ImGui::ColorEdit4("Square colour", glm::value_ptr(m_SquareColor));
		ImGui::End();
	}

	struct GUIProperties
	{
		glm::vec3 Position{0.f, 0.f, 0.f};
		float Rotation = 0.f;
		glm::vec4 ClearColour{0.16f, 0.7f, 0.3f, 1.0f};
	};

private:
	Hazel::Ref<Hazel::Shader> m_Shader;
	Hazel::Ref<Hazel::VertexArray> m_VertexArray;

	Hazel::Ref<Hazel::Shader> m_FlatColShader, m_TextureShader;
	Hazel::Ref<Hazel::VertexArray> m_SquareVA;

	Hazel::Ref<Hazel::Texture2D> m_Texture, m_DiamondTexture;

	Hazel::OrthographicCamera m_Camera;
	GUIProperties m_GUIProps;
	float m_CameraSpeed = 5.f;
	float m_CameraRotationSpeed = 30.f;

	glm::vec4 m_SquareColor = {.2f, .3f, .8f, 1.f};
};

class Sandbox : public Hazel::Application
{
public:
	Sandbox() { PushLayer(new ExampleLayer()); }

	~Sandbox() {}
};

Hazel::Application* Hazel::CreateApplication() { return new Sandbox(); }
