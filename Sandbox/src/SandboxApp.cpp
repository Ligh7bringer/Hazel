#include <Hazel.h>
#include <imgui/imgui.h>

class ExampleLayer : public Hazel::Layer {
public:
	ExampleLayer() : Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f) {
		m_VertexArray.reset(Hazel::VertexArray::Create());

		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.2f, 1.f,
			0.5f, -0.5f, 0.5f, 0.2f, 0.8f, 0.2f, 1.f,
			0.f, 0.5f, 0.0f, 0.2f, 0.2f, 0.8f, 1.f,
		};

		std::shared_ptr<Hazel::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(Hazel::VertexBuffer::Create(vertices, sizeof(vertices)));
		Hazel::BufferLayout layout = {
			{ Hazel::ShaderDataType::Float3, "a_Position" },
			{ Hazel::ShaderDataType::Float4, "a_Color" }
		};
		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		uint32_t indices[3] = { 0, 1, 2 };
		std::shared_ptr<Hazel::IndexBuffer> indexBuffer;
		indexBuffer.reset(Hazel::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		m_SquareVA.reset(Hazel::VertexArray::Create());

		float squareVertices[3 * 4] = {
			-0.75f, -0.75f, 0.f,
			 0.75f, -0.75f, 0.f,
			 0.75f,  0.75f, 0.f,
			-0.75f,  0.75f, 0.f,
		};

		std::shared_ptr<Hazel::VertexBuffer> squareVB;
		squareVB.reset(Hazel::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		squareVB->SetLayout({
			{ Hazel::ShaderDataType::Float3, "a_Position" }
			});
		m_SquareVA->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		std::shared_ptr<Hazel::IndexBuffer> squareIB;
		squareIB.reset(Hazel::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVA->SetIndexBuffer(squareIB);

		std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;
			uniform mat4 u_ViewProjection;
			out vec3 v_Position;
			out vec4 v_Color;
			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);	
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;
			in vec3 v_Position;
			in vec4 v_Color;
			void main()
			{
				color = vec4(v_Position * 0.5 + 0.5, 1.0);
				color = v_Color;
			}
		)";

		m_Shader.reset(Hazel::Shader::Create(vertexSrc, fragmentSrc));

		std::string blueShaderVertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			uniform mat4 u_ViewProjection;
			out vec3 v_Position;
			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);	
			}
		)";

		std::string blueShaderFragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;
			in vec3 v_Position;
			void main()
			{
				color = vec4(0.2, 0.3, 0.8, 1.0);
			}
		)";

		m_ShaderSqr.reset(Hazel::Shader::Create(blueShaderVertexSrc, blueShaderFragmentSrc));
	}

	void OnUpdate(Hazel::Timestep dt) override {
		float time = dt;

		if (Hazel::Input::IsKeyPressed(HZ_KEY_A))
			m_GUIProps.Position.x -= m_CameraSpeed * dt;
		else if (Hazel::Input::IsKeyPressed(HZ_KEY_D))
			m_GUIProps.Position.x += m_CameraSpeed * dt;

		if (Hazel::Input::IsKeyPressed(HZ_KEY_W))
			m_GUIProps.Position.y += m_CameraSpeed * dt;
		else if (Hazel::Input::IsKeyPressed(HZ_KEY_S))
			m_GUIProps.Position.y -= m_CameraSpeed * dt;

		if (Hazel::Input::IsKeyPressed(HZ_KEY_Y))
			m_GUIProps.Rotation += m_CameraRotationSpeed * dt;
		else if (Hazel::Input::IsKeyPressed(HZ_KEY_T))
			m_GUIProps.Rotation -= m_CameraRotationSpeed * dt;

		Hazel::RenderCommand::SetClearColor(m_GUIProps.ClearColour);
		Hazel::RenderCommand::Clear();

		m_Camera.SetPosition(m_GUIProps.Position);
		m_Camera.SetRotation(m_GUIProps.Rotation);

		Hazel::Renderer::BeginScene(m_Camera);

		Hazel::Renderer::Submit(m_ShaderSqr, m_SquareVA);
		Hazel::Renderer::Submit(m_Shader, m_VertexArray);

		Hazel::Renderer::EndScene();
	}

	void OnEvent(Hazel::Event& evt) override {}

	void OnImGuiRender() override {
		ImGui::Begin("FPS");
		ImGui::Text("%.3f ms/frame\n%.1f FPS", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();

		ImGui::BeginGroup();
		ImGui::Text("Camera position");
		ImGui::SliderFloat("Position.X", &m_GUIProps.Position.x, -2.f, 2.0f);
		ImGui::SliderFloat("Position.Y", &m_GUIProps.Position.y, -2.f, 2.0f);
		ImGui::SliderFloat("Position.Z", &m_GUIProps.Position.z, -2.f, 2.0f);
		ImGui::EndGroup();

		ImGui::Dummy(ImVec2(0.0f, 20.0f));

		ImGui::BeginGroup();
		ImGui::Text("Camera rotation");
		ImGui::SliderFloat("Rotation.Z", &m_GUIProps.Rotation, 0.0f, 360.f);
		ImGui::EndGroup();

		ImGui::Dummy(ImVec2(0.0f, 20.0f));

		static ImVec4 color = ImVec4(0.1f, 0.2f, 0.2f, 1.f);
		ImGui::Text("Clear color:");
		ImGui::ColorEdit4("MyColor##2f", (float*)& m_GUIProps.ClearColour, ImGuiColorEditFlags_Float);		
	}

	struct GUIProperties {
		glm::vec3 Position{ 0.f, 0.f, 0.f };
		float Rotation = 0.f;
		glm::vec4 ClearColour{ 0.16f, 0.19f, 0.3f, 1.0f };
	};

private:
	std::shared_ptr<Hazel::Shader> m_Shader;
	std::shared_ptr<Hazel::VertexArray> m_VertexArray;
					
	std::shared_ptr<Hazel::Shader> m_ShaderSqr;
	std::shared_ptr<Hazel::VertexArray> m_SquareVA;

	Hazel::OrthographicCamera m_Camera;
	GUIProperties m_GUIProps;
	float m_CameraSpeed = 1.f;
	float m_CameraRotationSpeed = 30.f;
};

class Sandbox : public Hazel::Application {
public:
	Sandbox() {
		PushLayer(new ExampleLayer());
	}

	~Sandbox() {}
};

Hazel::Application* Hazel::CreateApplication() {
	return new Sandbox();
}