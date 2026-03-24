cmake_minimum_required(VERSION 3.28.3)

project(TerminalAnim)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(TAlib)
target_compile_options(TAlib PRIVATE -Wall)
target_link_libraries(TAlib PRIVATE m)
target_sources(TAlib
  PRIVATE
    src/main.c
    src/escape-sequences.h
    src/platform.h
    src/platform_unix.c
    src/drawer.c
    src/scene.c
    src/scene.h
    src/terminal-handler.c
    src/terminal-anim-internal.h

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

add_executable(TestInitTeardown)
target_sources(TestInitTeardown PRIVATE tests/test_init_teardown.c)
target_link_libraries(TestInitTeardown PRIVATE TAlib)

add_executable(TestMaxRefresh)
target_sources(TestMaxRefresh PRIVATE tests/test_max_refresh.c)
target_link_libraries(TestMaxRefresh PRIVATE TAlib)

add_executable(TestClearScreen)
target_sources(TestClearScreen PRIVATE tests/test_clear.c)
target_link_libraries(TestClearScreen PRIVATE TAlib)

add_executable(TestStyles)
target_sources(TestStyles PRIVATE tests/test_styles.c)
target_link_libraries(TestStyles PRIVATE TAlib)

add_executable(TestShapes)
target_sources(TestShapes PRIVATE tests/test_shapes.c)
target_link_libraries(TestShapes PRIVATE TAlib)

add_executable(TestLines)
target_sources(TestLines PRIVATE tests/test_lines.c)
target_link_libraries(TestLines PRIVATE TAlib)
