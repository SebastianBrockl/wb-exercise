#include "util.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <iomanip>
#include <boost/endian/conversion.hpp>

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
       << "header platform : " << util::to_hex_string(header.platform) << "\n"
       << "header frame number : " << header.frameNumber << "\n"
       << "header time stamp : " << header.timeCpuCycles << "\n"
       << "num detected obj: " << header.numDetectedObj << "\n"
       << "num TLV : " << header.numTLVs << "\n"
       << "subframe num : " << header.subFrameNumber;

    return ss.str();
}

std::string util::to_hex_string(const FrameHeader &header)
{
    std::stringstream ss;
    ss << "\n"
       << "header magic word: " << util::to_hex_string(header.magic_word) << "\n"
       << "header version : " << util::to_hex_string(header.version) << "\n"
       << "header total packet lenght : " << to_hex_string(header.totalPacketLen) << "\n"
       << "header platform : " << util::to_hex_string(header.platform) << "\n"
       << "header frame number : " << to_hex_string(header.frameNumber) << "\n"
       << "header time stamp : " << to_hex_string(header.timeCpuCycles) << "\n"
       << "num detected obj: " << to_hex_string(header.numDetectedObj) << "\n"
       << "num TLV : " << to_hex_string(header.numTLVs) << "\n"
       << "subframe num : " << to_hex_string(header.subFrameNumber);

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

/**
 * @brief Deserialize the frame header
 *
 * This function deserializes the frame header from the read buffer. Note that serialization is done byte by byte,
 * as this needs to be platform agnostic. Moreover the function assumes that the byte order is little endian.
 * @param data
 * @return FrameHeader
 * @throw std::runtime_error if data size is smaller than header size
 */
FrameHeader util::deserialize_header(const std::vector<uint8_t> &data)
{
    return util::deserialize_header(data, data.begin());
}

/**
 * @brief Deserialize the frame header
 *
 * This function deserializes the frame header from the read buffer. Note that serialization is done byte by byte,
 * as this needs to be platform agnostic. Moreover the function assumes that the byte order is little endian.
 * @param data
 * @param iterator deserialization begins from iterator
 * @return FrameHeader
 * @throw std::runtime_error if data size is smaller than header size
 */
FrameHeader util::deserialize_header(const std::vector<uint8_t> &data,  std::vector<uint8_t>::const_iterator iterator)
{
    FrameHeader header;
    if (data.size() < sizeof(FrameHeader))
    {
        throw std::runtime_error("Util: deserialize_header: insufficient data: data size is smaller than header size");
    }

    // lambda function to copy data from iterator to field
    auto copy_from_iterator = [&iterator](auto &field) {
        std::memcpy(&field, &(*iterator), sizeof(field));
        std::advance(iterator, sizeof(field));
    };

    copy_from_iterator(header.magic_word);
    copy_from_iterator(header.version);
    copy_from_iterator(header.totalPacketLen);
    copy_from_iterator(header.platform);
    copy_from_iterator(header.frameNumber);
    copy_from_iterator(header.timeCpuCycles);
    copy_from_iterator(header.numDetectedObj);
    copy_from_iterator(header.numTLVs);
    copy_from_iterator(header.subFrameNumber);

    // header.magic_word = boost::endian::little_to_native(header.magic_word);
    header.version = boost::endian::little_to_native(header.version);
    header.totalPacketLen = boost::endian::little_to_native(header.totalPacketLen);
    header.platform = boost::endian::little_to_native(header.platform);
    header.frameNumber = boost::endian::little_to_native(header.frameNumber);
    header.timeCpuCycles = boost::endian::little_to_native(header.timeCpuCycles);
    header.numDetectedObj = boost::endian::little_to_native(header.numDetectedObj);
    header.numTLVs = boost::endian::little_to_native(header.numTLVs);
    header.subFrameNumber = boost::endian::little_to_native(header.subFrameNumber);

    return header;
}