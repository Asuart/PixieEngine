file(GLOB_RECURSE core_source_list "*.c" "*.cpp" "*.h" "*.hpp" "../../dependencies/glad/src/glad.c")

add_library(PixieEngineCore STATIC ${core_source_list})

target_include_directories(PixieEngineCore PUBLIC "${CMAKE_SOURCE_DIR}/source/pixie-engine-core/")
target_include_directories(PixieEngineCore PUBLIC "${SDL2_SOURCE_DIR}/include/")
target_include_directories(PixieEngineCore PUBLIC "${glm_SOURCE_DIR}/")
target_include_directories(PixieEngineCore PUBLIC "${stb_SOURCE_DIR}")
target_include_directories(PixieEngineCore PUBLIC "${CMAKE_SOURCE_DIR}/dependencies/glad/include/")
target_include_directories(PixieEngineCore PUBLIC "${freetype_SOURCE_DIR}/include/")
target_include_directories(PixieEngineCore PUBLIC "${ASSIMP_INCLUDE_DIRS}")

target_link_libraries(PixieEngineCore PUBLIC SDL2::SDL2)
target_link_libraries(PixieEngineCore PUBLIC assimp)
target_link_libraries(PixieEngineCore PUBLIC freetype)

foreach(_source IN ITEMS ${core_source_list})
    get_filename_component(_source_path "${_source}" PATH)
    string(REPLACE "${CMAKE_SOURCE_DIR}/source/pixie-engine-core" "" _group_path "${_source_path}")
    string(REPLACE "../../" "" _group_path "${_group_path}")
    string(REPLACE "/" "\\" _group_path "${_group_path}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()
