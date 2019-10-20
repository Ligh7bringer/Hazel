#include "WindowsInput.h"

#include "Hazel/Core/Application.h"
#include <GLFW/glfw3.h>

namespace Hazel
{

Scope<Input> Input::s_Instance = MakeScope<WindowsInput>();

bool Hazel::WindowsInput::IsKeyPressedImpl(int keycode)
{
	auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
	auto state = glfwGetKey(window, keycode);

	return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool WindowsInput::IsMouseBtnPressedImpl(int button)
{
	auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
	auto state = glfwGetMouseButton(window, button);

	return state == GLFW_PRESS;
}

std::pair<float, float> WindowsInput::GetMousePosImpl()
{
	auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
	double xPos, yPos;
	glfwGetCursorPos(window, &xPos, &yPos);

	return {(float)xPos, (float)yPos};
}

float WindowsInput::GetMouseXImpl()
{
	auto [x, y] = GetMousePosImpl();

	return x;
}

float WindowsInput::GetMouseYImpl()
{
	auto [x, y] = GetMousePosImpl();

	return y;
}

} // namespace Hazel
