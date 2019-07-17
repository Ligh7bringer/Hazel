#pragma once

#ifdef HZ_PLATFORM_WINDOWS
	#ifdef HZ_BUILD_DLL
		#define HAZEL_API __declspec(dllexport)
	#else 
		#define HAZEL_API __declspec(dllimport)
	#endif // HZ_BUILD_DLL
#elif HZ_PLATFORM_LINUX
	#ifdef HZ_BUILD_DLL
		#define HAZEL_API __attribute__((visibility("default")))
	#else 
		#define HAZEL_API 
	#endif // HZ_BUILD_DLL
#else 
	// unsupported platform, do nothing
	#define HAZEL_API
#endif

#define BIT(x) (1 << x)