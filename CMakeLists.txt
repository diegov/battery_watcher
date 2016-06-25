cmake_minimum_required(VERSION 3.5)

project(battery-watch)

include(FindPkgConfig)

pkg_search_module(LIBNOTIFY REQUIRED libnotify)

include_directories(${LIBNOTIFY_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")

add_executable(${PROJECT_NAME} daemon.cpp)

target_link_libraries(${PROJECT_NAME} ${LIBNOTIFY_LIBRARIES})
