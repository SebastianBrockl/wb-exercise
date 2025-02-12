#ifndef CONFIG_UART_H
#define CONFIG_UART_H

#include "RadarConfig.h"

#include <boost/asio.hpp>
#include <iostream>
#include <functional>
#include <vector>
#include <memory>

class ConfigUART {

public:
    ConfigUART(boost::asio::io_context& io_context, const std::string& port, uint32_t baud_rate);
    void async_write_config(const std::string& config_data, std::function<void(const boost::system::error_code&, std::size_t)> callback);

private:

    boost::asio::serial_port m_serial_port;
    boost::asio::streambuf m_write_buffer;
};

#endif // CONFIG_UART_H