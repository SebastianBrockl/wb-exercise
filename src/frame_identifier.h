#ifndef FRAME_IDENTIFIER_H
#define FRAME_IDENTIFIER_H

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
        const std::string &delimiter);

    void start();

private:
    void find_frame_start();
    void read_callback(const boost::system::error_code &error, std::size_t bytes_transferred);
    std::size_t match_magic_string(boost::asio::streambuf &readBuffer);
    void start_timeout();

    boost::asio::serial_port& m_serial_port;
    boost::asio::steady_timer m_timeout;
    boost::asio::streambuf m_read_buffer;
    size_t m_read_bytes;
    callback_t m_callback;
    const std::string& m_delimiter;


};

#endif // FRAME_IDENTIFIER_H