cmake_minimum_required(VERSION 3.14)
project(gemini_cli CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)

# 1. Declare the target first
add_executable(gemini_cli src/main.cpp)

# 2. Configure target directives
target_include_directories(gemini_cli PRIVATE external)
target_include_directories(gemini_cli PRIVATE includes)
target_compile_definitions(gemini_cli PRIVATE CPPHTTPLIB_OPENSSL_SUPPORT)
target_link_libraries(gemini_cli PRIVATE Threads::Threads OpenSSL::SSL OpenSSL::Crypto)
