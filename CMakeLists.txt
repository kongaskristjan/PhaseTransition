cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 14)
project(PhaseTransition)

find_package(OpenCV 3.4 REQUIRED)

include_directories(.)

FILE(GLOB LibSources Lib/*.cpp Lib/*.h)
add_library(library ${LibSources})

FILE(GLOB RunSources PhaseTransition/*.cpp PhaseTransition/*.h)
add_executable(PhaseTransition ${RunSources})
target_link_libraries(PhaseTransition library ${OpenCV_LIBS})


find_package(GTest)

if(GTest_FOUND)
    FILE(GLOB TestSources Tests/*.cpp)
    add_executable(RunTests ${TestSources})
    target_link_libraries(RunTests GTest::GTest GTest::Main library ${OpenCV_LIBS})

    gtest_discover_tests(RunTests)
    add_test(NAME monolithic COMMAND RunTests)
endif()
