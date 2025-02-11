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
        m_uart_connection(
            io_context, 
            write_port, 
            write_baudrate, 
            read_port, 
            read_baudrate) 
    {
    // Load configuration file as RadarConfig object
    m_radar_config.loadFromFile(m_config_path);
    std::cout << "RadarConfig loaded" << std::endl;
    // TODO: this should be logged only in debug profile
    std::cout << m_radar_config.toString() << std::endl;
    std::cout << m_radar_config.to_json_pretty() << std::endl;

    // TODO setup config write as an asynch operation
        // callback should print the written config
}

void Controller::run() {
    // Start WebSocket server
    // std::cout << "Starting WebSocket server" << std::endl;
    // m_ws_thread = std::thread([this]() {
    //     m_ws_server.run(m_websocket_port);
    // });

    // // Write configuration to UART
    // m_uart_connection.write_config(m_radar_config);

    // // Start a thread to process the buffer and broadcast to WebSocket clients
    // m_processing_thread = std::thread(&Controller::process_buffer, this);

    // // Read bitstream from UART
    // m_uart_connection.read_bitstream(m_ws_server);

    // m_processing_thread.join();
    // m_ws_thread.join();
}

void Controller::async_write_config(const RadarConfig &config)
{
    //m_uart_connection.async_write_config(config, std::bind(&Controller::write_config_callback, this, std::placeholders::_1, std::placeholders::_2));
}

void Controller::write_config_callback(const boost::system::error_code &error, std::size_t bytes_transferred)
{
    if (!error) {
        std::cout << "Configuration write successful: " << bytes_transferred << " bytes written.\n";
    } else {
        std::cerr << "Configuration write failed: " << error.message() << std::endl;
        // exit(1);
    }
}

// void Controller::process_buffer() {
//     while (true) {
//         std::vector<char> buffer;
//         {
//             std::unique_lock<std::mutex> lock(m_uart_connection.m_mutex);
//             m_uart_connection.m_cond_var.wait(lock, [this] { return !m_uart_connection.m_buffer_queue.empty(); });
//             buffer = std::move(m_uart_connection.m_buffer_queue.front());
//             m_uart_connection.m_buffer_queue.pop();
//         }

//         // Process the buffer and broadcast to WebSocket clients
//         std::string bitstream(buffer.begin(), buffer.end());
//         std::string magic_string = "MAGIC"; // Replace with your actual magic string
//         size_t pos;
//         while ((pos = bitstream.find(magic_string)) != std::string::npos) {
//             std::string message = bitstream.substr(0, pos);
//             bitstream.erase(0, pos + magic_string.length());

//             // Map the message into an object (example object)
//             nlohmann::json message_json;
//             message_json["data"] = message;

//             // Broadcast the message JSON to all connected WebSockets
//             m_ws_server.broadcast(message_json.dump());
//         }
//     }
//}
