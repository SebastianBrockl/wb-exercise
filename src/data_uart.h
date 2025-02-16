#ifndef DATA_UART_H
#define DATA_UART_H

#include "frame_identifier.h"

#include <boost/asio.hpp>
#include <iostream>
#include <functional>
#include <vector>
#include <memory>
#include <istream>

// MAGIC STRING SIGNIFIES START OF DATA FRAME
static const uint8_t UART_MAGIC_BYTES[8] = {0x02, 0x01, 0x04, 0x03, 0x06, 0x05, 0x08, 0x07};
static const std::string UART_MAGIC_STRING = "\x02\x01\x04\x03\x06\x05\x08\x07";
class DataUART
{
public:
    DataUART(boost::asio::io_context &io_context,
             const std::string &port,
             uint32_t baud_rate);

    void start_async_read();

    void frame_callback(const boost::system::error_code &error, std::size_t bytes_transferred);

private:

    void handle_read(const boost::system::error_code &error,
                     std::size_t bytes_transferred);
    void handle_frame(std::shared_ptr<std::vector<uint8_t>> frame);

    void find_frame_start();
    std::size_t match_magic_string(boost::asio::streambuf &readBuffer);

    uint32_t extractUint32(const std::vector<uint8_t> &buffer, size_t offset);

    boost::asio::serial_port m_serial_port;
    boost::asio::streambuf m_read_buffer;
    // tasks on strand are guarenteed to run serially
    boost::asio::strand<boost::asio::io_context::executor_type> m_strand;
    std::vector<uint8_t> m_frame_buffer;
    bool m_collecting_frame = false;
    uint32_t m_frame_length = 0;
};

#endif // DATA_UART_H