#include "Platform/OpenGL/OpenGLShader.hpp"
#include "Sandbox2D.hpp"
#include <Hazel.hpp>
#include <Hazel/Core/EntryPoint.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

class Sandbox : public Hazel::Application
{
public:
	Sandbox() { PushLayer(new Sandbox2D()); }

	~Sandbox() = default;
};

Hazel::Scope<Hazel::Application> Hazel::CreateApplication() { return Hazel::MakeScope<Sandbox>(); }
