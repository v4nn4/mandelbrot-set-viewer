add_subdirectory(engine)
add_executable(main WIN32 main.cpp)
target_link_libraries(main PRIVATE msv-engine)

set_target_properties(main PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(main PUBLIC
    user32
    gdi32
    Xinput
)
find_library(GDIPLUS_LIBRARY NAMES libgdiplus gdiplus)
set(GDIPLUS_LIBRARY gdiplus)