cmake_minimum_required(VERSION 3.17)
project(kuhn_poker)

set(CMAKE_CXX_STANDARD 14)

add_executable(kuhn_poker main.cpp kuhn_poker.cpp kuhn_poker.h information_set.cpp information_set.h kuhn_poker_cfr_trainer.cpp kuhn_poker_cfr_trainer.h)