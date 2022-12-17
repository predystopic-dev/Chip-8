cmake_minimum_required(VERSION 3.23)
project(chip_8)

set(CMAKE_CXX_STANDARD 14)

add_executable(chip_8 main.cpp chip8.h opcodes.cpp opcodes.h)
