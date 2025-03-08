#ifndef FRAME_IDENTIFIER_H
#define FRAME_IDENTIFIER_H

#include "TLV.h"
#include <boost/asio.hpp>
#include <memory>
#include <functional>
#include <iostream>

/**
 * @brief Class to identify the start of a frame in a stream of bytes
 * 
 * Reads UART stream untill start of frame is found, or timeout occurs
 */
class FrameIdentifier : public std::enable_shared_from_this<FrameIdentifier>
{
    using callback_t = std::function<void(const boost::system::error_code &, std::size_t)>;

public:
    FrameIdentifier(
        boost::asio::serial_port &serial_port,
        callback_t callback,
        const std::vector<uint8_t> &delimiter);

    void start();

private:
    void find_frame_start();
    void handle_frame_start(const boost::system::error_code &error, std::size_t bytes_transferred);
    void prepare_header();
    void read_message(size_t remaining_message_lenght);
    void read_callback(const boost::system::error_code &error, std::size_t bytes_transferred);

    void start_timeout();

    // helper function for getting buffer data as a vector of uint8_t for easier handling
    const std::vector<uint8_t> get_buffer_data();

    std::string buffer_as_string();

    boost::asio::serial_port& m_serial_port;
    boost::asio::steady_timer m_timeout;
    boost::asio::streambuf m_read_buffer;
    size_t m_read_bytes;
    callback_t m_callback;
    const std::vector<uint8_t> m_delimiter;

    std::shared_ptr<FrameHeader> m_frame_header;
    std::unique_ptr<std::vector<uint8_t>> m_message_bytes = std::make_unique<std::vector<uint8_t>>();

};

#endif // FRAME_IDENTIFIER_H