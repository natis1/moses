cmake_minimum_required(VERSION 2.6)
project(moses)

add_executable(moses main.cpp)

install(TARGETS moses RUNTIME DESTINATION bin)
