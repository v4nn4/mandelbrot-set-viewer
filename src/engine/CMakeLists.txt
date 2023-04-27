add_library(msv-engine SHARED 
  Mandelbrot.cpp
  Coordinates.cpp
)
target_include_directories(msv-engine 
  PUBLIC include
  PRIVATE .
)