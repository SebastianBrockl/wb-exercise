#include "config_uart.h"

ConfigUART::ConfigUART(
    boost::asio::io_context &io_context, 
    const std::string &port, 
    uint32_t baud_rate)
    : m_serial_port(io_context, port)
{
    m_serial_port.set_option(boost::asio::serial_port_base::baud_rate(baud_rate));
    std::cout << "Config UART initialized" << std::endl;
    std::cout << "Config UART Port: " << port << std::endl;
    std::cout << "Config UART Baud Rate: " << baud_rate << std::endl;
}

void ConfigUART::async_write_config(const std::string& config_data, std::function<void(const boost::system::error_code&, std::size_t)> callback) {
    // asynch write enriched config
    boost::asio::async_write(m_serial_port, boost::asio::buffer(config_data), callback);
}