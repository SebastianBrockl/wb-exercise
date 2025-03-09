# Target system
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Set cross-compiler
set(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)

# Where the sysroot is (modify if needed)
set(CMAKE_SYSROOT /usr/arm-linux-gnueabihf)

# Specify paths for finding libraries and includes
set(CMAKE_FIND_ROOT_PATH /usr/arm-linux-gnueabihf)

# Tell CMake to search for libraries and includes only in the sysroot
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
