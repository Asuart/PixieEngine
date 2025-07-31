
file(GLOB_RECURSE _source_list "*.c" "*.cpp" "*.h" "*.hpp" )

add_executable(MarchingCubesDemo ${_source_list})

target_link_libraries(PixieEngineEditor PUBLIC PixieEngineCore)
target_link_libraries(PixieEngineEditor PUBLIC imgui)

target_include_directories(PixieEngineEditor PUBLIC "${CMAKE_SOURCE_DIR}/source/marching-cubes-demo/")
target_include_directories(PixieEngineEditor PUBLIC "${imgui_SOURCE_DIR}/")
target_include_directories(PixieEngineEditor PUBLIC "${imgui_SOURCE_DIR}/backends/")

foreach(_source IN ITEMS ${_source_list})
    get_filename_component(_source_path "${_source}" PATH)
    string(REPLACE "${CMAKE_SOURCE_DIR}/source/pixie-engine-editor" "" _group_path "${_source_path}")
    string(REPLACE "../../" "" _group_path "${_group_path}")
    string(REPLACE "/" "\\" _group_path "${_group_path}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()