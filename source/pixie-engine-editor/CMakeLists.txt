
file(GLOB_RECURSE editor_source_list "*.c" "*.cpp" "*.h" "*.hpp" )

add_executable(PixieEngineEditor ${editor_source_list})

target_link_libraries(PixieEngineEditor PUBLIC PixieEngineCore)
target_link_libraries(PixieEngineEditor PUBLIC PixieEngineUI)

target_include_directories(PixieEngineEditor PUBLIC "${CMAKE_SOURCE_DIR}/source/pixie-engine-editor/")
target_include_directories(PixieEngineEditor PUBLIC "${imgui_SOURCE_DIR}/")

foreach(_source IN ITEMS ${editor_source_list})
    get_filename_component(_source_path "${_source}" PATH)
    string(REPLACE "${CMAKE_SOURCE_DIR}/source/pixie-engine-editor" "" _group_path "${_source_path}")
    string(REPLACE "../../" "" _group_path "${_group_path}")
    string(REPLACE "/" "\\" _group_path "${_group_path}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()

if (MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT PixieEngineEditor)
endif()

set(SHADER_SOURCE_DIR "${CMAKE_SOURCE_DIR}/resources/")
set(SHADER_DESTINATION_DIR "$<TARGET_FILE_DIR:PixieEngineEditor>/resources/")

add_custom_command(
    TARGET PixieEngineEditor
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different 
    "${SHADER_SOURCE_DIR}" 
    "${SHADER_DESTINATION_DIR}"
    COMMENT "Copying Resources"
)
