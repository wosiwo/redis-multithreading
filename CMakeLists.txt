cmake_minimum_required(VERSION 3.12)
project(redis-3.0-annotated C)

set(CMAKE_C_STANDARD 99)

add_executable(redis-3.0-annotated src/redis.c)