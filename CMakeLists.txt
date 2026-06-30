cmake_minimum_required(VERSION 3.16)
project(bull)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(bull
    src/main.cpp
    src/bull.cpp
    src/slog.cpp
)

target_include_directories(bull PRIVATE include)

install(TARGETS bull DESTINATION bin)
