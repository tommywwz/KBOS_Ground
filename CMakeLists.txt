cmake_minimum_required(VERSION 3.16.3)
project(KBOS_Ground C)

set(CMAKE_C_STANDARD 11)

set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)

add_executable(KBOS_Ground Taking_Flight.c Utils.c Utils.h)

target_link_libraries(KBOS_Ground PRIVATE Threads::Threads)
