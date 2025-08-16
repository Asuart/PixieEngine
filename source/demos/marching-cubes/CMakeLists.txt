file(GLOB_RECURSE _source_list "*.c" "*.cpp" "*.h" "*.hpp" )

add_executable(MarchingCubes ${_source_list})

target_link_libraries(MarchingCubes PUBLIC PixieEngineCore)
target_link_libraries(MarchingCubes PUBLIC PixieEngineUI)

target_include_directories(MarchingCubes PUBLIC "${CMAKE_SOURCE_DIR}/source/demos/marching-cubes/")
target_include_directories(MarchingCubes PUBLIC "${CMAKE_SOURCE_DIR}/source/pixie-engine-core/")
target_include_directories(MarchingCubes PUBLIC "${imgui_SOURCE_DIR}/")
target_include_directories(MarchingCubes PUBLIC "${imgui_SOURCE_DIR}/backends/")

foreach(_source IN ITEMS ${_source_list})
    get_filename_component(_source_path "${_source}" PATH)
    string(REPLACE "${CMAKE_SOURCE_DIR}/source/demos/marching-cubes" "" _group_path "${_source_path}")
    string(REPLACE "../../" "" _group_path "${_group_path}")
    string(REPLACE "/" "\\" _group_path "${_group_path}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()

set_target_properties(MarchingCubes PROPERTIES FOLDER Demos)