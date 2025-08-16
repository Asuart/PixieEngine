file(GLOB_RECURSE _source_list "*.c" "*.cpp" "*.h" "*.hpp" )

add_library(PixieEngineUI STATIC ${_source_list})

target_link_libraries(PixieEngineUI PUBLIC imgui)
target_link_libraries(PixieEngineUI PUBLIC SDL2::SDL2)
target_link_libraries(PixieEngineUI PUBLIC PixieEngineCore)
target_link_libraries(PixieEngineUI PUBLIC Vulkan::Vulkan)

target_include_directories(PixieEngineUI PUBLIC "${CMAKE_SOURCE_DIR}/source/pixie-engine-ui/")
target_include_directories(PixieEngineCore PUBLIC "${CMAKE_SOURCE_DIR}/dependencies/glad/include/")
target_include_directories(PixieEngineUI PUBLIC "${imgui_SOURCE_DIR}/")
target_include_directories(PixieEngineUI PUBLIC "${imgui_SOURCE_DIR}/backends/")

foreach(_source IN ITEMS ${_source_list})
    get_filename_component(_source_path "${_source}" PATH)
    string(REPLACE "${CMAKE_SOURCE_DIR}/source/pixie-engine-ui" "" _group_path "${_source_path}")
    string(REPLACE "../../" "" _group_path "${_group_path}")
    string(REPLACE "/" "\\" _group_path "${_group_path}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()