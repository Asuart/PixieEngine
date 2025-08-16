message(STATUS "Fetching freetype ...")

FetchContent_MakeAvailable(freetype)

set_target_properties(freetype PROPERTIES FOLDER freetype)