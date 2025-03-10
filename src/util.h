#ifndef UTIL_H
#define UTIL_H

#include "TLV.h"
#include <string>
#include <memory>
#include <optional>

namespace util
{
    /**
     * Read a file into a string
     */
    std::string read_file_to_string(const std::string &file_path);

    /**
     * Load a configuration file into a shared pointer
     */
    void load_config(std::shared_ptr<std::string> config, const std::string &config_file);

    /**
     * Remove empty lines from string
     */
    void remove_empty_lines(std::string &str);

    /**
     * Ensure \n line endings
     */
    void ensure_unix_line_endings(std::string &str);

    /**
     * Remove comments from string
     */
    void remove_comments(std::string &str);

    // TLV handling & serialization utilities

    // Serialize a TLV to a string
    std::string to_string(const FrameHeader& header);
    // Serialize a TLV to a string with hex formatting
    std::string to_hex_string(const FrameHeader& header);

    // helpers for serializing data fields to hex strings

    std::string to_hex_string(const std::vector<uint8_t> &data);
    std::string to_hex_string(const uint8_t &data);
    std::string to_hex_string(const uint16_t &data);
    std::string to_hex_string(const uint32_t &data);
    std::string to_hex_string(const uint64_t &data);

    // deserialize header from byte stream
    FrameHeader deserialize_header(const std::vector<uint8_t> &data);
    // deserialize header from byte stream, starting at iterator, iterator is incremented to point to the next byte after the header
    FrameHeader deserialize_header(const std::vector<uint8_t> &data,  std::vector<uint8_t>::const_iterator iterator);

    // deserialize next TLV from byte stream, starting at iterator, iterator is incremented to point to the next byte after the TLV
    std::optional<tlv> deserialize_next_tlv(const std::vector<uint8_t> &data, std::vector<uint8_t>::const_iterator &iterator);

    // deserialize TargetListTLV from byte stream, starting at iterator, iterator is incremented to point to the next byte after the TLV
    std::optional<TargetListTLV> deserialize_target_list_tlv(
        const std::vector<uint8_t> &data, 
        std::vector<uint8_t>::const_iterator &iterator,
        TlvHeader header);

    // deserialize targetIndexTLV Target from byte stream, starting at iterator, iterator is incremented to point to the next byte after the TLV
    Target deserialize_next_target(const std::vector<uint8_t> &data, std::vector<uint8_t>::const_iterator &iterator);

} // namespace util

#endif // UTIL_H