cmake_minimum_required(VERSION 3.20)

set(PROJECT_NAME "ClientServerApp" CACHE STRING "Project name")
project(${PROJECT_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")

option(BUILD_CLIENT "Build the client program" ON)
option(BUILD_SERVER "Build the server program" ON)

add_subdirectory(src)
