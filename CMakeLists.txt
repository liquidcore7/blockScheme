cmake_minimum_required(VERSION 3.5)
project(blockScheme)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp language/types/blockTypes.h language/types/expressionFolder.h language/types/blockTypes.cpp
        language/types/expressionFolder.cpp language/interpreter/Process.cpp language/interpreter/Process.h)

add_executable(blockScheme ${SOURCE_FILES})
