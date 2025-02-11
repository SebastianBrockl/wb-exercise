#include "uart_connection.h"

UARTConnection::UARTConnection(boost::asio::io_context& io_context, const std::string& write_port, const std::string& read_port)
    : m_write_port(io_context, write_port), m_read_port(io_context, read_port) {
    m_write_port.set_option(boost::asio::serial_port_base::baud_rate(9600));
    m_read_port.set_option(boost::asio::serial_port_base::baud_rate(9600));
}

void UARTConnection::write_config(const std::string& config_file) {
    std::ifstream file(config_file, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open config file: " << config_file << std::endl;
        return;
    }

    std::vector<char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    boost::asio::write(m_write_port, boost::asio::buffer(buffer));
}

void UARTConnection::read_bitstream(const std::string& output_file) {
    std::ofstream file(output_file, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open output file: " << output_file << std::endl;
        return;
    }

    char buffer[1024];
    boost::system::error_code ec;
    while (true) {
        size_t len = m_read_port.read_some(boost::asio::buffer(buffer), ec);
        if (ec == boost::asio::error::eof) {
            break; // Connection closed cleanly by peer.
        } else if (ec) {
            std::cerr << "Error reading bitstream: " << ec.message() << std::endl;
            break;
        }
        file.write(buffer, len);
    }
}