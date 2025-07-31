message(STATUS "Fetching glm ...")

FetchContent_MakeAvailable(glm)

target_include_directories(glm PUBLIC ${glm_SOURCE_DIR})
