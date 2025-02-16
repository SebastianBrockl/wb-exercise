#ifndef CONFIG_UART_H
#define CONFIG_UART_H

#include "RadarConfig.h"

#include <boost/asio.hpp>
#include <iostream>
#include <functional>
#include <vector>
#include <memory>
#include <atomic>

class ConfigUART
{

    using callback_t = std::function<void(const boost::system::error_code &, std::size_t)>;

public:
    ConfigUART(boost::asio::io_context &io_context,
               const std::string &port,
               uint32_t baud_rate);
    void async_write_config(const std::string &config_data,
                            callback_t callback);

    bool get_is_transmitting() const { return m_is_transmitting.load(std::memory_order_relaxed); };

private:
    // handler callback for transmission errors
    void transmission_error(const boost::system::error_code &errorCode);
    // handler callback for transmission completion, prints out received lines
    void transmission_complete();
    void ready_transmission(callback_t callback);
    void set_is_transmitting(bool is_transmitting) { m_is_transmitting.store(is_transmitting, std::memory_order_relaxed); };
    // send a single config line, await response
    void async_send_line(const std::string &config_data,
                         callback_t callback);
    // write a single line, includedefault delay
    void write_next_line(const std::string &line);

    // read acks in response to a single written config line
    void read_ack(const boost::system::error_code &errorCode, std::size_t ioBytes);

    boost::asio::serial_port m_serial_port;
    boost::asio::streambuf m_write_buffer;
    boost::asio::io_context &io_context;
    boost::asio::strand<boost::asio::io_context::executor_type> m_strand;
    boost::asio::steady_timer m_30ms_delay;

    // needs a lock

    std::atomic<bool> m_is_transmitting{false};

    // Sate management constructs for handling transmission of config
    boost::asio::streambuf m_buffer;     // buffer for reading
    std::vector<std::string> m_transmit; // lines to transmit
    std::vector<std::string> m_receive;  // total lines received
    std::vector<std::string> m_acks;     // acks received / lines sent
    std::size_t m_sentLines = 0;
    std::size_t m_receivedLines = 0;
    callback_t m_transmission_callback;
};

#endif // CONFIG_UART_H