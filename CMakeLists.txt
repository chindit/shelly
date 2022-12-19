cmake_minimum_required(VERSION 3.18)
project(shelly)

set(CMAKE_CXX_STANDARD 20)

add_executable(shelly main.cpp Request.cpp Request.h Config.cpp Config.h DeviceFetcher.cpp DeviceFetcher.h Model/DeviceInfo.cpp Model/DeviceInfo.h enums.h Cache.cpp Cache.h Model/CachedDevice.cpp Model/CachedDevice.h)
target_link_libraries(shelly curl)
target_link_libraries(shelly jsoncpp)
target_link_libraries(shelly pqxx)
find_package(fmt)
target_link_libraries(shelly fmt::fmt)
