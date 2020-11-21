cmake_minimum_required(VERSION 3.0.0)
project(main VERSION 0.1.0)

file(GLOB src
    src/*.cpp
    lib/thread/lib/thread/Pool.cpp
)

set(THREADS_PREFER_PTHREAD_FLAG ON)
include_directories("lib/spdlog/include/")
include_directories("lib/thread/lib/thread/")
include_directories("lib/Simple-WebSocket-Server/")
include_directories("lib/json/single_include/nlohmann")

find_path(ASIO_PATH asio.hpp)
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

add_compile_definitions(USE_STANDALONE_ASIO)

add_subdirectory("lib/spdlog")

add_executable(main ${src})

target_link_libraries(main PRIVATE OpenSSL::SSL)
target_link_libraries(main PRIVATE Threads::Threads)

if(MSVC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
else()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})