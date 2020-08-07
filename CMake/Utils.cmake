include_guard()

function(hz_register_exe EXE_NAME SOURCES)
  add_executable(${EXE_NAME} ${SOURCES})

  target_include_directories(
    ${EXE_NAME}
    PRIVATE ${PROJECT_SOURCE_DIR}/Hazel/src
            ${PROJECT_SOURCE_DIR}/Hazel/vendor/imgui
            ${PROJECT_SOURCE_DIR}/Hazel/vendor/Glad/include
            ${PROJECT_SOURCE_DIR}/Hazel/vendor/entt/single_include)

  target_link_libraries(${EXE_NAME} PRIVATE Hazel)

  set_target_properties(
    ${EXE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                           ${CMAKE_BINARY_DIR}/bin/${EXE_NAME})

  if(MSVC)
    set_target_properties(
      ${EXE_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY
                             ${CMAKE_BINARY_DIR}/bin/${EXE_NAME}/$<CONFIG>)
  endif()
endfunction()
