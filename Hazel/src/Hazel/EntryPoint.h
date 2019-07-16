#pragma once

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