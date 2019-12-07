cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 14)
project(PhaseTransition)

#add_compile_options("-pg")
#add_link_options("-pg")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(Threads REQUIRED)
find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS} .)

FILE(GLOB LibSources Lib/*.cpp Lib/*.h)
add_library(library ${LibSources})

FILE(GLOB RunSources PhaseTransition/*.cpp PhaseTransition/*.h)
add_executable(PhaseTransition ${RunSources})
target_link_libraries(PhaseTransition library ${SDL2_LIBRARIES} Threads::Threads)

# Testing

find_package(GTest)

if(GTest_FOUND)
    FILE(GLOB TestSources Tests/*.cpp)
    add_executable(RunTests ${TestSources})
    target_link_libraries(RunTests library ${SDL2_LIBRARIES} Threads::Threads GTest::GTest GTest::Main)

    gtest_discover_tests(RunTests)
    add_test(NAME monolithic COMMAND RunTests)
endif()
