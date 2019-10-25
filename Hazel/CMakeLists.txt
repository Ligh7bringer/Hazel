add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/GLFW)

file(GLOB_RECURSE HAZEL_SRCS src/*.h src/*.cpp)
add_library(Hazel STATIC ${HAZEL_SRCS})

set(GLAD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor/Glad)
add_library(glad STATIC ${GLAD_DIR}/include/glad/glad.h ${GLAD_DIR}/src/glad.c)
target_include_directories(glad PUBLIC ${GLAD_DIR}/include)

set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui)
file(GLOB IMGUI_SRCS ${IMGUI_DIR}/*.h ${IMGUI_DIR}/*.cpp)
add_library(imgui STATIC ${IMGUI_SRCS})
target_include_directories(imgui PUBLIC ${IMGUI_DIR})

set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor)
target_include_directories(
  Hazel
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src ${LIB_DIR}/GLFW/include
          ${LIB_DIR}/stb_image
  PUBLIC ${LIB_DIR}/glm ${LIB_DIR}/spdlog/include)

target_compile_features(Hazel PUBLIC cxx_std_17)

if(WIN32)
  set(CURR_PLATFORM HZ_PLATFORM_WINDOWS)
elseif(UNIX)
  set(CURR_PLATFORM HZ_PLATFORM_LINUX)
else()
  set(CURR_PLATFORM UNSUPPORTED)
endif()

target_compile_definitions(
  Hazel PUBLIC ${CURR_PLATFORM} _CRT_SECURE_NO_WARNINGS GLFW_INCLUDE_NONE
  $<$<CONFIG:Debug>:HZ_DEBUG>)
target_precompile_headers(Hazel PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/hzpch.h)

target_link_libraries(Hazel PRIVATE glfw glad imgui)

set_target_properties(glfw glad imgui PROPERTIES FOLDER vendor)