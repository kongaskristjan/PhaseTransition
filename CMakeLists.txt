cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 14)
project(PhaseTransition)

#add_compile_options("-pg")
#add_link_options("-pg")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(FFmpeg)

if(FFMPEG_FOUND)
    set(CMAKE_CXX_FLAGS " -D FFMPEG_ENABLED")
endif()

if(NOT EMSCRIPTEN)
    find_package(Threads REQUIRED)
    find_package(SDL2 REQUIRED)
    find_package(SDL2TTF REQUIRED)
endif()

FILE(GLOB LibSources Lib/*.cpp Lib/*.h)
add_library(library ${LibSources})

FILE(GLOB RunSources PhaseTransition/*.cpp PhaseTransition/*.h)
add_executable(PhaseTransition ${RunSources})

include_directories(${SDL2_INCLUDE_DIRS} ${SDL2TTF_INCLUDE_DIR} ${FFMPEG_INCLUDE_DIR} .)

if(EMSCRIPTEN)
    target_link_libraries(PhaseTransition --bind library ${FFMPEG_LIBRARIES})

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_SDL=2 -s USE_SDL_TTF=2 -s ASSERTIONS=1 -s ALLOW_MEMORY_GROWTH=1")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --embed-file ${CMAKE_SOURCE_DIR}/Setups@PhaseTransition")

    configure_file(Tests/PhaseTransition.html PhaseTransition.html COPYONLY)
else()
    target_link_libraries(PhaseTransition library ${SDL2_LIBRARIES} ${SDL2TTF_LIBRARY} ${FFMPEG_LIBRARIES} Threads::Threads)
endif()

# Testing

find_package(GTest)

if(GTest_FOUND)
    FILE(GLOB TestSources Tests/*.cpp)
    add_executable(RunTests ${TestSources})
    target_link_libraries(RunTests library ${SDL2_LIBRARIES} ${SDL2TTF_LIBRARY} Threads::Threads GTest::GTest GTest::Main)

    gtest_discover_tests(RunTests)
    add_test(NAME monolithic COMMAND RunTests)
endif()
