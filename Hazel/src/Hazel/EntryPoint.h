#pragma once

// windows main
#ifdef HZ_PLATFORM_WINDOWS

extern Hazel::Application* Hazel::CreateApplication();

int main(int argc, char** argv) {
	Hazel::Log::Init();
	HZ_CORE_WARN("Initialised logger!");
	HZ_INFO("Initialised logger!");

	auto app = Hazel::CreateApplication();
	app->Run();
	delete app;
}

#endif

// this is the same as windows just now
// as no platform specific code is used
#ifdef HZ_PLATFORM_LINUX

extern Hazel::Application* Hazel::CreateApplication();

int main(int argc, char** argv) {
	Hazel::Log::Init();
	HZ_CORE_WARN("Initialised logger!");
	HZ_INFO("Initialised logger!");

	auto app = Hazel::CreateApplication();
	app->Run();
	delete app;
}

#endif