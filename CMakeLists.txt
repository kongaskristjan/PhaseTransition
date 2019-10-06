cmake_minimum_required(VERSION 3.6)
set(CMAKE_CXX_STANDARD 17)
project(PhaseTransition)

find_package(OpenCV 3.4 REQUIRED)

include_directories(.)

FILE(GLOB Sources PhaseTransition/*.cpp PhaseTransition/*.h Lib/*.cpp Lib/*.h)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(PhaseTransition ${Sources})
target_link_libraries(PhaseTransition ${OpenCV_LIBS} Threads::Threads)
