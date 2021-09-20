cmake_minimum_required(VERSION 3.21.2)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(main VERSION 1.0)
add_executable(main main.cpp game.cpp player.cpp RPSTrainer.cpp)
target_include_directories(main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
