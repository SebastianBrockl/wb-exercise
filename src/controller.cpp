#include "controller.h"
#include <iostream>
#include <nlohmann/json.hpp>


Controller::Controller(
        boost::asio::io_context& io_context, 
        const std::string& config_path, 
        const std::string& read_port, 
        int read_baudrate, 
        const std::string& write_port, 
        int write_baudrate, 
        uint16_t websocket_port)
    : m_io_context(io_context), 
        m_config_path(config_path), 
        m_read_port(read_port), 
        m_read_baudrate(read_baudrate), 
        m_write_port(write_port), 
        m_write_baudrate(write_baudrate), 
        m_websocket_port(websocket_port), 
        m_ws_server(io_context, websocket_port),
        m_data_uart(io_context, read_port, read_baudrate),
        m_config_uart(io_context, write_port, write_baudrate)
    {
    // Load configuration file as RadarConfig object
    std::cout << "Controller: Loading RadarConfig from file: " << m_config_path << std::endl;
    m_radar_config.loadFromFile(m_config_path);
    std::cout << "Controller: RadarConfig loaded" << std::endl;

    // setup config uart write on asio run
    m_io_context.post([this](){async_write_config(m_radar_config);});
    // setup data uart read on asio run
    m_io_context.post([this](){m_data_uart.start_async_read();});
}

void Controller::run() {
}

void Controller::async_write_config(const RadarConfig &config)
{
    std::ostringstream oss;
    // enrich config with sensor commands
    oss << "sensorStop\n"
        << "flushCfg\n"
        << config.toString() 
        << "sensorStart\n";

    // FIXME: this schould be debug log
    std::cout << "Controler: preparing config asynch write over UART" << std::endl << oss.str() << std::endl;
    std::cout << "Controller: starting config asynch write over UART, config size: " << oss.str().size() << std::endl;
    m_config_uart.async_write_config( oss.str(), std::bind(&Controller::write_config_callback, this, std::placeholders::_1, std::placeholders::_2));
}

void Controller::write_config_callback(const boost::system::error_code &error, std::size_t bytes_transferred)
{
    if (!error) {
        std::cout << "Controller: Configuration write successful: " << bytes_transferred << " bytes written.\n";
    } else {
        std::cerr << "Controller: Configuration write failed: " << error.message() << std::endl;
        // exit(1);
    }
}

void Controller::stop()
{
    m_ws_server.stop();
    m_io_context.stop();
//    m_processing_thread.join();
//    m_ws_thread.join();
}
