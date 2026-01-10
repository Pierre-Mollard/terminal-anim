cmake_minimum_required(VERSION 3.28.3)

project(TerminalAnim)

# Note: end goal of project is to be a lib, just for testing for now
add_executable(TerminalExec)
target_sources(TerminalExec PRIVATE src/main.c)


add_library(TAlib)
target_sources(TAlib
  PRIVATE
    src/main.c

  PUBLIC
    FILE_SET HEADERS
    BASE_DIRS
      include
    FILES
      include/terminal-anim.h
)
