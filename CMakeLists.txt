cmake_minimum_required(VERSION 3.23)
project(shelly)

set(CMAKE_CXX_STANDARD 23)

add_executable(shelly main.cpp Request.cpp Request.h Config.cpp Config.h)
target_link_libraries(shelly curl)
target_link_libraries(shelly jsoncpp)
find_package(fmt)
target_link_libraries(shelly fmt::fmt)
