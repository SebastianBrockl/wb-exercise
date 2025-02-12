#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "websocket_server.h"
#include "RadarConfig.h"
#include "data_uart.h"
#include "config_uart.h"
#include <boost/asio.hpp>
#include <thread>

class Controller
{
public:
    Controller(boost::asio::io_context &io_context,
               const std::string &config_path,
               const std::string &read_port,
               int read_baudrate,
               const std::string &write_port,
               int write_baudrate,
               uint16_t websocket_port);
    void run();
    void async_write_config(const RadarConfig &config);
    void write_config_callback(const boost::system::error_code &error,
                               std::size_t bytes_transferred);
    // best attempt at gracefull shutdown
    void stop();

private:
    boost::asio::io_context &m_io_context;
    std::string m_config_path;
    std::string m_read_port;
    int m_read_baudrate;
    std::string m_write_port;
    int m_write_baudrate;
    int m_websocket_port;

    RadarConfig m_radar_config;
    WebSocketServer m_ws_server;
    DataUART m_data_uart;
    ConfigUART m_config_uart;
    std::thread m_ws_thread;
    std::thread m_processing_thread;
};

#endif // CONTROLLER_H