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
FrameHeader util::deserialize_header(const std::vector<uint8_t> &data, std::vector<uint8_t>::const_iterator iterator)
{
    FrameHeader header;
    if (data.size() < sizeof(FrameHeader))
    {
        throw std::runtime_error("Util: deserialize_header: insufficient data: data size is smaller than header size");
    }

    // lambda function to copy data from iterator to field
    auto copy_from_iterator = [&iterator](auto &field)
    {
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

/**
 * @brief Deserializes next TLV (type-length-value) objects from the incoming data stream.
 *
 * This function deserializes the next TLV object from the incoming data stream. The function assumes that the byte order is little endian.
 *
 * @param data The incoming data stream
 * @param iterator The iterator to the current position in the data stream
 * @return std::optional<tlv> The deserialized TLV object§
 */
std::optional<tlv> util::deserialize_next_tlv(const std::vector<uint8_t> &data, std::vector<uint8_t>::const_iterator &iterator)
{
    // lambda function to copy data from iterator to field
    auto copy_from_iterator = [&iterator](auto &field)
    {
        std::memcpy(&field, &(*iterator), sizeof(field));
        std::advance(iterator, sizeof(field));
    };

    uint32_t type;
    uint32_t length;
    copy_from_iterator(type);
    copy_from_iterator(length);

    type = boost::endian::little_to_native(type);
    length = boost::endian::little_to_native(length);

    switch (type)
    {
    // case 1020: // pointCloud
    // {

    // }
    case 1010: // targetObjectList
    {
        TlvHeader header{TLVType::TargetObjectList, length};
        return util::deserialize_target_list_tlv(data, iterator, header);
        // return std::nullopt; // FIXME: delete this line
    }
    // case 1011: // targetIndex
    // {

    // }
    // case 1012: // targetHeight
    // {

    // }
    // case 1021: // presenceIndication
    // {

    // }
    default:
    {
        std::cerr << "Util: deserialize_next_tlv: Unknown TLV type: " << type << std::endl;
        std::advance(iterator, length); // advance iterator to skip entire TLV
        return std::nullopt;
    }
    }
}

std::optional<TargetListTLV> deserialize_target_list_tlv(
    const std::vector<uint8_t> &data,
    std::vector<uint8_t>::const_iterator &iterator,
    TlvHeader header)
{
    // lambda function to copy data from iterator to field
    // auto copy_from_iterator = [&iterator](auto &field)
    // {
    //     std::memcpy(&field, &(*iterator), sizeof(field));
    //     std::advance(iterator, sizeof(field));
    // };

    // check that there is sufficient data to contain the (possibly multiple) TLV's
    const auto bytes_in_tlv = static_cast<size_t>(header.length) - sizeof(TlvHeader);
    // naively assume that this cast is safe, should be since the distance should not be negative
    const auto bytes_in_buffer = static_cast<size_t>(std::distance(iterator, data.end()));
    if (bytes_in_buffer < bytes_in_tlv)
    {
        // we don't have enough data to contain even one target list TLV
        std::cerr << "Util: deserialize_target_list_tlv: insufficient data: data size (" << bytes_in_buffer
                  << ") is smaller than TLV size (" << bytes_in_tlv << ")" << std::endl;
        return std::nullopt;
    }
    // we have data for at least one TLV, but there might be multiple
    // in case we have multiple TLV's, ensure we have space and that the data is aligned
    auto byte_misalignment = bytes_in_tlv % sizeof(Target);

    // assemble TargetListTLV
    TargetListTLV target_list;
    target_list.header = header;

    for (size_t i = 0; i < bytes_in_tlv / sizeof(Target); ++i)
    {
        Target target = util::deserialize_next_target(data, iterator);
        target_list.targets.push_back(target);
    }
    if (byte_misalignment != 0)
    {
        std::cerr << "Util: deserialize_target_list_tlv: data is not aligned to target size " << byte_misalignment << "bytes misaligned" << std::endl;
        std::advance(iterator, byte_misalignment);
    }

    return {target_list};
}

Target util::deserialize_next_target(const std::vector<uint8_t> &data, std::vector<uint8_t>::const_iterator &iterator)
{
    Target target;
    // lambda function to copy data from iterator to field
    auto copy_from_iterator = [&iterator](auto &field)
    {
        std::memcpy(&field, &(*iterator), sizeof(field));
        field = boost::endian::little_to_native(field);
        std::advance(iterator, sizeof(field));
    };

    // lambda function to copy data from iterator to field of type float
    // this is required as boost::endian::little_to_native doesn't support floats
    // blindly trusting that the target architecture has 32b floats
    auto copy_float_from_iterator = [&iterator](float &field)
    {
        // first deserialize bytes to uint32_t
        uint32_t pseudo_float;
        std::memcpy(&pseudo_float, &(*iterator), sizeof(pseudo_float));
        pseudo_float = boost::endian::little_to_native(pseudo_float);

        // convert deserialized uint32_t to float
        std::memcpy(&field, &pseudo_float, sizeof(pseudo_float));
        std::advance(iterator, sizeof(pseudo_float));
    };

    // boost::endian::little_to_native doesn't support floats, so w need to work around
    uint32_t my_ec[16];

    copy_from_iterator(target.trackId);
    copy_float_from_iterator(target.posx);
    copy_float_from_iterator(target.posy);
    copy_float_from_iterator(target.posz);
    copy_float_from_iterator(target.velx);
    copy_float_from_iterator(target.vely);
    copy_float_from_iterator(target.velz);
    copy_float_from_iterator(target.accx);
    copy_float_from_iterator(target.accy);
    copy_float_from_iterator(target.accz);
    for (int i = 0; i < 16; i++)
    {
        // copy_from_iterator(my_ec[i]);
        // // convert deserialized uint32_t to float
        // std::memcpy(&target.ec[i], &my_ec[i], sizeof(my_ec[i]));
        copy_float_from_iterator(target.ec[i]);
    }
    copy_float_from_iterator(target.g);
    copy_float_from_iterator(target.confidencelevel);

    return target;
}
