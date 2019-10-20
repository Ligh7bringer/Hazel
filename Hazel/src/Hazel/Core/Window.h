#pragma once

#include "Hazel/Core/Core.h"
#include "Hazel/Events/Event.h"

namespace Hazel
{

// Window properties
struct WindowProps
{
	std::string Title;
	unsigned int Width, Height;

	WindowProps(const std::string& title = "Hazel Engine",
				unsigned int width = 1280,
				unsigned int height = 720)
		: Title(title)
		, Width(width)
		, Height(height)
	{}
};

// window interface
class HAZEL_API Window
{
public:
	using EventCallbackFn = std::function<void(Event&)>;

	virtual ~Window() = default;

	virtual void OnUpdate() = 0;

	virtual unsigned int GetWidth() const = 0;
	virtual unsigned int GetHeight() const = 0;

	// attributes
	virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
	virtual void SetVSync(bool enables) = 0;
	virtual bool IsVSync() const = 0;

	virtual void* GetNativeWindow() const = 0;

	static Window* Create(const WindowProps& props = WindowProps());
};

} // namespace Hazel
