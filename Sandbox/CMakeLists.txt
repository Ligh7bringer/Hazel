include(${PROJECT_SOURCE_DIR}/CMake/Utils.cmake)

set(SANDBOX_EXE_NAME "Sandbox")
set(SANDBOX_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Sandbox2D.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SandboxApp.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ParticleSystem.cpp)

hz_register_exe("${SANDBOX_EXE_NAME}" "${SANDBOX_SRCS}")

add_custom_target(
  copy_assets ALL
  COMMAND
    ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/Sandbox/assets
    ${CMAKE_BINARY_DIR}/bin/${SANDBOX_EXE_NAME}/assets)
add_dependencies(${SANDBOX_EXE_NAME} copy_assets)
