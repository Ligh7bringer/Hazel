#pragma once

#include "Hazel/Core/Core.hpp"

namespace Hazel
{

class HAZEL_API Input
{
protected:
	Input() = default;

public:
	virtual ~Input() = default;

	Input(const Input&) = delete;
	Input& operator=(const Input&) = delete;

	static bool IsKeyPressed(int keycode) { return s_Instance->IsKeyPressedImpl(keycode); }

	static bool IsMouseBtnPressed(int button) { return s_Instance->IsMouseBtnPressedImpl(button); }
	static std::pair<float, float> GetMousePos() { return s_Instance->GetMousePosImpl(); }
	static float GetMouseX() { return s_Instance->GetMouseXImpl(); }
	static float GetMouseY() { return s_Instance->GetMouseYImpl(); }

protected:
	virtual bool IsKeyPressedImpl(int keycode) = 0;

	virtual bool IsMouseBtnPressedImpl(int button) = 0;
	virtual std::pair<float, float> GetMousePosImpl() = 0;
	virtual float GetMouseXImpl() = 0;
	virtual float GetMouseYImpl() = 0;

private:
	static Hazel::Scope<Input> s_Instance;
};

} // namespace Hazel
