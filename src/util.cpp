#include "util.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

std::string read_file_to_string(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        throw std::runtime_error("Util: failed to open file: " + file_path);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::cout << "Util: File read successfully" << std::endl;
    return buffer.str();
}

void load_config(std::shared_ptr<std::string> config, const std::string &config_file) {

    // load config to string
    std::string config_str = read_file_to_string(config_file);

    // clean up config
    ensure_unix_line_endings(config_str);
    remove_comments(config_str);
    remove_empty_lines(config_str);

    // store config in shared pointer
    *config = config_str;
}

//Remove empty lines from string
void remove_empty_lines(std::string& str){
    std::string::size_type pos = 0;
    while ((pos = str.find("\n\n", pos)) != std::string::npos) {
        str.erase(pos, 1);
    }
}

// Ensure \n line endings
void ensure_unix_line_endings(std::string& str) {
    std::string::size_type pos = 0;
    while ((pos = str.find("\r\n", pos)) != std::string::npos) {
        str.replace(pos, 2, "\n");
    }
}

// Remove comments (starting with '%') from string
void remove_comments(std::string& str) {
    std::string::size_type pos = 0;
    while ((pos = str.find('%', pos)) != std::string::npos) {
        str.erase(pos, str.find('\n', pos) - pos);
    }
}
