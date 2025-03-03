#include "util.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <iomanip>

std::string util::read_file_to_string(const std::string &file_path)
{
    std::ifstream file(file_path);
    if (!file.is_open())
    {
        throw std::runtime_error("Util: failed to open file: " + file_path);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::cout << "Util: File read successfully" << std::endl;
    return buffer.str();
}

void util::load_config(std::shared_ptr<std::string> config, const std::string &config_file)
{

    // load config to string
    std::string config_str = util::read_file_to_string(config_file);

    // clean up config
    util::ensure_unix_line_endings(config_str);
    util::remove_comments(config_str);
    util::remove_empty_lines(config_str);

    // store config in shared pointer
    *config = config_str;
}

// Remove empty lines from string
void util::remove_empty_lines(std::string &str)
{
    std::string::size_type pos = 0;
    while ((pos = str.find("\n\n", pos)) != std::string::npos)
    {
        str.erase(pos, 1);
    }
}

// Ensure \n line endings
void util::ensure_unix_line_endings(std::string &str)
{
    std::string::size_type pos = 0;
    while ((pos = str.find("\r\n", pos)) != std::string::npos)
    {
        str.replace(pos, 2, "\n");
    }
}

// Remove comments (starting with '%') from string
void util::remove_comments(std::string &str)
{
    std::string::size_type pos = 0;
    while ((pos = str.find('%', pos)) != std::string::npos)
    {
        str.erase(pos, str.find('\n', pos) - pos);
    }
}

std::string util::to_string(const FrameHeader &header)
{
    std::stringstream ss;
    ss << "\n"
       << "header magic word: " << util::to_hex_string(header.magic_word) << "\n"
       << "header version : " << util::to_hex_string(header.version) << "\n"
       << "header total packet lenght : " << header.totalPacketLen << "\n"
       << "header platform : " << header.platform << "\n"
       << "header frame number : " << header.frameNumber << "\n"
       << "header time stamp : " << header.timeCpuCycles << "\n"
       << "num detected obj: " << header.numDetectedObj << "\n"
       << "num TLV : " << header.numTLVs << "\n"
       << "subframe num : " << header.subFrameNumber;

    return ss.str();
}

/**
 * transform bytes to hexadecimal string.
 */
std::string util::to_hex_string(const std::vector<uint8_t> &data)
{
    std::stringstream ss;
    ss << "0x ";
    for (const auto &byte : data)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
    }
    return ss.str();
}

std::string util::to_hex_string(const uint8_t &data)
{
    const std::vector<uint8_t> data_vector = {data};
    return util::to_hex_string(data_vector);
}

std::string util::to_hex_string(const uint16_t &data)
{
    const std::vector<uint8_t> data_vector = {
        static_cast<uint8_t>(data >> 8),
        static_cast<uint8_t>(data)};
    return util::to_hex_string(data_vector);
}

std::string util::to_hex_string(const uint32_t &data)
{
    const std::vector<uint8_t> data_vector = {
        static_cast<uint8_t>(data >> 24),
        static_cast<uint8_t>(data >> 16),
        static_cast<uint8_t>(data >> 8),
        static_cast<uint8_t>(data)};
    return util::to_hex_string(data_vector);
}

std::string util::to_hex_string(const uint64_t &data)
{
    const std::vector<uint8_t> data_vector = {
        static_cast<uint8_t>(data >> 56),
        static_cast<uint8_t>(data >> 48),
        static_cast<uint8_t>(data >> 40),
        static_cast<uint8_t>(data >> 32),
        static_cast<uint8_t>(data >> 24),
        static_cast<uint8_t>(data >> 16),
        static_cast<uint8_t>(data >> 8),
        static_cast<uint8_t>(data)};
    return util::to_hex_string(data_vector);
}
