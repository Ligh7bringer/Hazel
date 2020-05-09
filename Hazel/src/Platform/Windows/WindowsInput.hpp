#pragma once

#include "Hazel/Core/Input.hpp"

namespace Hazel
{

class WindowsInput : public Input
{
protected:
	virtual bool IsKeyPressedImpl(int keycode) override;

	virtual bool IsMouseBtnPressedImpl(int button) override;
	virtual std::pair<float, float> GetMousePosImpl() override;
	virtual float GetMouseXImpl() override;
	virtual float GetMouseYImpl() override;
};

} // namespace Hazel
