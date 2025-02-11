#ifndef UART_CONNECTION_H
#define UART_CONNECTION_H

#include <boost/asio.hpp>
#include <iostream>
#include <fstream>
#include <vector>

class UARTConnection {
public:
    UARTConnection(boost::asio::io_context& io_context, const std::string& write_port, const std::string& read_port);
    void write_config(const std::string& config_file);
    void read_bitstream(const std::string& output_file);

private:
    boost::asio::serial_port m_write_port;
    boost::asio::serial_port m_read_port;
};

#endif // UART_CONNECTION_H