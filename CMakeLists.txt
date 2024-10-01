cmake_minimum_required(VERSION 3.26)
project(find_msg)

set(CMAKE_CXX_STANDARD 17)

add_executable(find_msg main.cpp)
target_link_libraries(find_msg bfd)
