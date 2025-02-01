#include <boost/asio.hpp>
#include <iostream>

class SerialPort {
public:
    SerialPort(const std::string& port, unsigned int baud_rate)
        : io_service(), serial(io_service, port) {

        serial.set_option(boost::asio::serial_port_base::baud_rate(baud_rate));
        serial.set_option(boost::asio::serial_port_base::character_size(8));
        serial.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
        serial.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
    }

    void write(const std::string& data) {
        boost::asio::write(serial, boost::asio::buffer(data));
    }

    std::string read(size_t length) {
        std::string result;
        boost::asio::read(serial, boost::asio::buffer(result, length));
        return result;
    }

private:
    boost::asio::io_service io_service;
    boost::asio::serial_port serial;
};
