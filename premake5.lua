workspace "Hazel"
	architecture "x64"
	
	configurations {
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project	"Hazel"
	location "Hazel"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs {
		"%{prj.name}/vendor/spdlog/include"
	}

	filter "system:linux"
	printf("Generating linux makefiles...")
	cppdialect "C++17"
	staticruntime "On"
	systemversion "latest"

	defines {
		"HZ_PLATFORM_LINUX",
		"HZ_BUILD_DLL"
	}

	filter "system:windows"
		printf("Generating visual studio solution...")
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines {
			"HZ_PLATFORM_WINDOWS",
			"HZ_BUILD_DLL"
		}

	filter "configurations:Debug"
		defines "HZ_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "HZ_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "HZ_DIST"
		optimize "On"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs {
		"Hazel/vendor/spdlog/include",
		"Hazel/src"
	}

	links {
		"Hazel"
	}

	filter "system:linux"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines {
			"HZ_PLATFORM_LINUX"
		}

		postbuildcommands
		{
			("cp ../bin/" .. outputdir .. "/Hazel/libHazel.so" .. " ../bin/" .. outputdir .. "/%{prj.name}")
		}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines {
			"HZ_PLATFORM_WINDOWS"
		}

		postbuildcommands
		{
			("{COPY} ../bin/" .. outputdir .. "/Hazel/Hazel.dll" .. " ../bin/" .. outputdir .. "/%{prj.name}")
		}

	filter "configurations:Debug"
		defines "HZ_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "HZ_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "HZ_DIST"
		optimize "On"