cmake_minimum_required(VERSION 3.8)
project(glad)

# Have Glad generate as a static library
add_library(glad)
target_sources(glad PRIVATE src/glad.c)
target_include_directories(glad PUBLIC include)
