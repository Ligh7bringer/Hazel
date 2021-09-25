#pragma once

#include <memory>

// Platform detection using predefined macros
#ifdef _WIN32
/* Windows x64/x86 */
#	ifdef _WIN64
/* Windows x64  */
#		define HZ_PLATFORM_WINDOWS
#	else
/* Windows x86 */
#		error "x86 builds are not supported!"
#	endif
#elif defined(__APPLE__) || defined(__MACH__)
#	error "Platform APPLE is not supported!"
/* We also have to check __ANDROID__ before __linux__
 * since android is based on the linux kernel
 * it has __linux__ defined */
#elif defined(__ANDROID__)
#	define HZ_PLATFORM_ANDROID
#	error "Platform Android is not supported!"
#elif defined(__linux__)
#	define HZ_PLATFORM_LINUX
#else
/* Unknown compiler/platform */
#	error "Unknown platform!"
#endif // End of platform detection

// Provide dynamic library options for Windows and Linux.
// This currently does *not* work as Hazel is linked statically
// by default.
#if defined HZ_PLATFORM_WINDOWS
#	if defined HZ_DYNAMIC_LINK
#		if defined HZ_BUILD_DLL
#			define HAZEL_API __declspec(dllexport)
#		else
#			define HAZEL_API __declspec(dllimport)
#		endif
#	else
#		define HAZEL_API
#	endif
#elif defined HZ_PLATFORM_LINUX
#	if defined HZ_DYNAMIC_LINK
#		if defined HZ_BUILD_DLL
#			define HAZEL_API __attribute__((visibility("default")))
#		else
#			define HAZEL_API
#		endif
#	else
#		define HAZEL_API
#	endif
#else
#	error Unsupported platform!
#endif

// Debug settings
#if defined HZ_DEBUG
#	if defined HZ_PLATFORM_WINDOWS
#		define HZ_DEBUGBREAK() __debugbreak()
#	elif defined HZ_PLATFORM_LINUX
#		include <signal.h>
#		define HZ_DEBUGBREAK() raise(SIGTRAP)
#	endif

#	define HZ_ENABLE_RENDERER_LOG
#	define HZ_ENABLE_ASSERTS
#	define HZ_ENABLE_CORE_LOG
#	define HZ_ENABLE_CLIENT_LOG
#endif // End of Debug settings

// Assert statements
#if defined HZ_ENABLE_ASSERTS
#	define HZ_ASSERT(x, ...)                                                                      \
		{                                                                                          \
			if(!(x))                                                                               \
			{                                                                                      \
				HZ_ERROR("Assertion Failed: {0}", __VA_ARGS__);                                    \
				HZ_DEBUGBREAK();                                                                   \
			}                                                                                      \
		}
#	define HZ_CORE_ASSERT(x, ...)                                                                 \
		{                                                                                          \
			if(!(x))                                                                               \
			{                                                                                      \
				HZ_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__);                               \
				HZ_DEBUGBREAK();                                                                   \
			}                                                                                      \
		}
#else
#	define HZ_ASSERT(x, ...)
#	define HZ_CORE_ASSERT(x, ...)
#endif

// bit shifting macro
#define BIT(x) (1 << x)

#define HZ_BIND_EVENT_FN(fn)                                                                       \
	[this](auto&&... args) -> decltype(auto) {                                                     \
		return this->fn(std::forward<decltype(args)>(args)...);                                    \
	}

namespace Hazel
{

// FIXME: Remove!
#define ASSET_PATH "assets/"
#define SCENE_PATH ASSET_PATH "scenes/"
#define RESOURCE_PATH "resources/"

template <typename T>
using Scope = std::unique_ptr<T>;
template <typename T, typename... Args>
constexpr Scope<T> MakeScope(Args&&... args)
{
	return std::make_unique<T>(std::forward<Args>(args)...);
}

template <typename T>
using Ref = std::shared_ptr<T>;
template <typename T, typename... Args>
constexpr Ref<T> MakeRef(Args&&... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}

} // namespace Hazel
