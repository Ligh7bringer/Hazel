cmake_minimum_required(VERSION 3.16)

project(Hazel LANGUAGES C CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY})

add_subdirectory(Hazel)
add_subdirectory(Sandbox)

if(MSVC)
  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT
                                                              Sandbox)
endif()
