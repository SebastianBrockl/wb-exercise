cmake_minimum_required(VERSION 3.14)
project(homework-wb-mm)

set(CMAKE_CXX_STANDARD 17)

# Find Boost
find_package(Boost REQUIRED COMPONENTS system)

## Include websocketpp
##add_subdirectory(websocketpp)

# Include the source directory
include_directories(src include)

# Add your executable target
add_executable(homework src/main.cpp)

# Link Boost and websocketpp
target_link_libraries(homework Boost::Boost websocketpp)
