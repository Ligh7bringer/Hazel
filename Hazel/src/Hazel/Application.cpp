#include "Application.h"

#include "Hazel/Events/KeyEvent.h"
#include "Log.h"

namespace Hazel {

	Application::Application()
	{
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		KeyPressedEvent e(5, 5);
		if(e.isInCategory(EventCategoryKeyboard))
			HZ_TRACE(e);

		while (true);
	}

}