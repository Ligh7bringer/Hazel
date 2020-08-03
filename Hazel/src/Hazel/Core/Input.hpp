#pragma once

#include "Hazel/Core/Core.hpp"

namespace Hazel
{

class HAZEL_API Input
{
public:
	static bool IsKeyPressed(int keycode);
	static bool IsMouseBtnPressed(int button);

	static std::pair<float, float> GetMousePos();
	static float GetMouseX();
	static float GetMouseY();
};

} // namespace Hazel
