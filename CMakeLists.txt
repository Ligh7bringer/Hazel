cmake_minimum_required(VERSION 3.16)

project(Hazel LANGUAGES C CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory(Hazel)
add_subdirectory(Sandbox)
add_subdirectory(Hazelnut)

if(MSVC)
  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT
                                                              Sandbox)
endif()
