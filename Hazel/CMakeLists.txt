option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/GLFW)

file(GLOB_RECURSE HAZEL_SRCS src/*.hpp src/*.cpp)
add_library(Hazel STATIC ${HAZEL_SRCS})

set(GLAD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor/Glad)
add_library(glad STATIC ${GLAD_DIR}/include/glad/glad.h ${GLAD_DIR}/src/glad.c)
target_include_directories(glad PUBLIC ${GLAD_DIR}/include)

set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui)
file(GLOB IMGUI_SRCS ${IMGUI_DIR}/*.h ${IMGUI_DIR}/*.cpp)
add_library(imgui STATIC ${IMGUI_SRCS})
target_include_directories(imgui PUBLIC ${IMGUI_DIR})

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/yaml-cpp)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/nfd)

set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor)
target_include_directories(
  Hazel
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src ${LIB_DIR}/GLFW/include
          ${LIB_DIR}/stb_image
  PUBLIC ${LIB_DIR}/glm ${LIB_DIR}/spdlog/include
         ${LIB_DIR}/entt/single_include ${LIB_DIR}/yaml-cpp/include
         ${LIB_DIR}/nfd/src/include)

target_compile_features(Hazel PUBLIC cxx_std_17)

target_compile_definitions(
  Hazel PUBLIC _CRT_SECURE_NO_WARNINGS GLFW_INCLUDE_NONE SPDLOG_HEADER_ONLY
               $<$<CONFIG:Debug>:HZ_DEBUG> $<$<BOOL:${WIN32}>:NOMINMAX>)
target_precompile_headers(Hazel PUBLIC
                          ${CMAKE_CURRENT_SOURCE_DIR}/src/hzpch.hpp)

target_link_libraries(
  Hazel
  PRIVATE glfw glad imgui
  PUBLIC yaml-cpp::yaml-cpp nfd)

set_target_properties(glfw glad imgui yaml-cpp nfd PROPERTIES FOLDER vendor)
