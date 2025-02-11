#include "util.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

std::string read_file_to_string(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + file_path);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::cout << "File read successfully" << std::endl;
    return buffer.str();
}

