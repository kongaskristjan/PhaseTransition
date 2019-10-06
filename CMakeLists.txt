cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 14)
project(PhaseTransition)

find_package(OpenCV 3.4 REQUIRED)

include_directories(.)

FILE(GLOB Sources PhaseTransition/*.cpp PhaseTransition/*.h Lib/*.cpp Lib/*.h)

add_executable(PhaseTransition ${Sources})
target_link_libraries(PhaseTransition ${OpenCV_LIBS})
