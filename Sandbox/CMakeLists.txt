add_executable(
  Sandbox
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Sandbox2D.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/SandboxApp.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ParticleSystem.cpp)

target_include_directories(
  Sandbox
  PRIVATE ${PROJECT_SOURCE_DIR}/Hazel/src
          ${PROJECT_SOURCE_DIR}/Hazel/vendor/imgui
          ${PROJECT_SOURCE_DIR}/Hazel/vendor/Glad/include)

target_link_libraries(Sandbox PRIVATE Hazel)

if(MSVC)
  set_target_properties(Sandbox PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY
                                           ${CMAKE_BINARY_DIR}/bin/$<CONFIG>)
endif()

add_custom_target(
  copy_assets ALL
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/assets
          ${CMAKE_BINARY_DIR}/bin/assets)
add_dependencies(Sandbox copy_assets)
