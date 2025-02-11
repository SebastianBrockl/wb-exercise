#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <memory>

/**
 * Read a file into a string
 */
std::string read_file_to_string(const std::string& file_path);

/**
 * Load a configuration file into a shared pointer
 */
void load_config(std::shared_ptr<std::string> config, const std::string& config_file);

/**
 * Remove empty lines from string
 */
void remove_empty_lines(std::string& str);

/**
 * Ensure \n line endings
 */
void ensure_unix_line_endings(std::string& str);

/**
 * Remove comments from string
 */
void remove_comments(std::string& str);

#endif // UTIL_H