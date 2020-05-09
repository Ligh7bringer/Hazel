#pragma once

#include "Hazel/Core/Core.hpp"

// windows main
#ifdef HZ_PLATFORM_WINDOWS

extern Hazel::Scope<Hazel::Application> Hazel::CreateApplication();

int main(int argc, char** argv)
{
	Hazel::Log::Init();

	HZ_PROFILE_BEGIN_SESSION("Startup", "HazelProfile-Startup.json");
	auto app = Hazel::CreateApplication();
	HZ_PROFILE_END_SESSION();

	HZ_PROFILE_BEGIN_SESSION("Runtime", "HazelProfile-Runtime.json");
	app->Run();
	HZ_PROFILE_END_SESSION();

	HZ_PROFILE_BEGIN_SESSION("Shutdown", "HazelProfile-Shutdown.json");
	HZ_PROFILE_END_SESSION();
}

#endif

// this is the same as windows just now
// as no platform specific code is used
#ifdef HZ_PLATFORM_LINUX

extern Hazel::Scope<Hazel::Application> Hazel::CreateApplication();

int main(int argc, char** argv)
{
	Hazel::Log::Init();

	HZ_PROFILE_BEGIN_SESSION("Startup", "HazelProfile-Startup.json");
	auto app = Hazel::CreateApplication();
	HZ_PROFILE_END_SESSION();

	HZ_PROFILE_BEGIN_SESSION("Runtime", "HazelProfile-Runtime.json");
	app->Run();
	HZ_PROFILE_END_SESSION();

	HZ_PROFILE_BEGIN_SESSION("Shutdown", "HazelProfile-Shutdown.json");
	HZ_PROFILE_END_SESSION();
}

#endif
