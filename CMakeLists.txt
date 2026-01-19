cmake_minimum_required(VERSION 3.28.3)

project(TerminalAnim)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(TAlib)
target_compile_options(TAlib PRIVATE -Wall)
target_sources(TAlib
  PRIVATE
    src/main.c
    src/escape-sequences.h
    src/platform.h
    src/platform_unix.c
    src/drawer.c

  PUBLIC
    FILE_SET HEADERS
    BASE_DIRS
      include
    FILES
      include/terminal-anim.h
)

# Note: end goal of project is to be a lib, just for testing for now
add_executable(TerminalExec)
target_sources(TerminalExec PRIVATE src/main.c)
target_link_libraries(TerminalExec PRIVATE TAlib)

add_executable(TestStatic)
target_sources(TestStatic PRIVATE tests/test_static.c)
target_link_libraries(TestStatic PRIVATE TAlib)
