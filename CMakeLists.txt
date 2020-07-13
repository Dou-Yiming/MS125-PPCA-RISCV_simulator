cmake_minimum_required(VERSION 3.10)

project(RISCV_simulator)

set(CMAKE_CXX_STANDARD 14)

add_compile_options(-O2)

add_executable(code RISCV_simulator.cpp)