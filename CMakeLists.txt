cmake_minimum_required(VERSION 3.10)
project(mistral-demo)

# Detect environment variable, otherwise use default
if(DEFINED ENV{LOCAL_DOCKER_REGISTRY})
    set(LOCAL_DOCKER_REGISTRY "$ENV{LOCAL_DOCKER_REGISTRY}")
else()
    ## make sure you are logged in using an personal access token with correct permissions!
    set(LOCAL_DOCKER_REGISTRY "ghcr.io/sebastianbrockl")
endif()

set(CMAKE_CXX_STANDARD 17)

# Set cross-compiler for ARM (only if cross-compiling)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)
    set(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)
endif()

# Enable optimization
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

set(DOCKER_TAG "latest")
set(DOCKER_IMAGE "${LOCAL_DOCKER_REGISTRY}/mistral-demo:${DOCKER_TAG}")

# Set output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Ensure Docker commands run from the correct directory (project root)
get_filename_component(PROJECT_ROOT ${CMAKE_SOURCE_DIR} ABSOLUTE)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS system)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(mistral-demo src/main.cpp src/websocket_server.cpp)
target_compile_options(mistral-demo PRIVATE -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-variable)
target_link_libraries(mistral-demo ${Boost_LIBRARIES} -static-libgcc -static-libstdc++ -static)

# Custom command to build Docker image
add_custom_target(docker-build
    COMMAND docker buildx build --platform linux/arm/v7 -t ${LOCAL_DOCKER_REGISTRY}/mistral-demo --load .
    WORKING_DIRECTORY ${PROJECT_ROOT}
    DEPENDS mistral-demo
)

# Custom command to push the image
add_custom_target(docker-push
    COMMAND docker push ${LOCAL_DOCKER_REGISTRY}/mistral-demo
    DEPENDS docker-build
)
