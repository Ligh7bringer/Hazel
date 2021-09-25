include(${PROJECT_SOURCE_DIR}/CMake/Utils.cmake)

set(EDITOR_EXE_NAME "Hazelnut")
set(EDITOR_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/EditorLayer.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/src/HazelnutApp.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/src/Panels/SceneHierarchyPanel.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/src/Panels/ContentBrowserPanel.cpp)

hz_register_exe("${EDITOR_EXE_NAME}" "${EDITOR_SRCS}")

add_custom_target(
  copy_assets_editor ALL
  COMMAND
    ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_SOURCE_DIR}/${EDITOR_EXE_NAME}/assets
    ${CMAKE_BINARY_DIR}/bin/${EDITOR_EXE_NAME}/assets)
add_dependencies(${EDITOR_EXE_NAME} copy_assets_editor)
