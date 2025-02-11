#include "websocket_server.h"
#include "util.h"
#include "RadarConfig.h"
#include "uart_connection.h"
#include <iostream>
#include <getopt.h>
#include <thread>

void print_usage() {
    std::cout << "Usage: mistral-demo [options]\n"
              << "Options:\n"
              << "  --config_path <path>      Path to the default config file (default: /app/default-config.cfg)\n"
              << "  --read_port <port>        Read UART interface (default: /dev/ttyUSB0)\n"
              << "  --write_port <port>       Write UART interface (default: /dev/ttyUSB1)\n"
              << "  --read_baudrate <baud>    Read UART baudrate (default: 9600)\n"
              << "  --write_baudrate <baud>   Write UART baudrate (default: 9600)\n"
              << "  --websocket_port <port>   WebSocket server port (default: 9002)\n"
              << std::endl;
}

void process_buffer(UARTConnection& uart, WebSocketServer& ws_server) {
    while (true) {
        std::vector<char> buffer;
        {
            std::unique_lock<std::mutex> lock(uart.m_mutex);
            uart.m_cond_var.wait(lock, [&uart] { return !uart.m_buffer_queue.empty(); });
            buffer = std::move(uart.m_buffer_queue.front());
            uart.m_buffer_queue.pop();
        }

        // Process the buffer and broadcast to WebSocket clients
        std::string bitstream(buffer.begin(), buffer.end());
        std::string magic_string = "MAGIC"; // Replace with your actual magic string
        size_t pos;
        while ((pos = bitstream.find(magic_string)) != std::string::npos) {
            std::string message = bitstream.substr(0, pos);
            bitstream.erase(0, pos + magic_string.length());

            // Map the message into an object (example object)
            nlohmann::json message_json;
            message_json["data"] = message;

            // Broadcast the message JSON to all connected WebSockets
            ws_server.broadcast(message_json.dump());
        }
    }
}

int main(int argc, char* argv[]) {
    std::string config_path = "/app/default-config.cfg";
    std::string read_port = "/dev/ttyUSB0";
    std::string write_port = "/dev/ttyUSB1";
    int read_baudrate = 9600;
    int write_baudrate = 9600;
    int websocket_port = 9002;

    static struct option long_options[] = {
        {"config_path", required_argument, 0, 'c'},
        {"read_port", required_argument, 0, 'r'},
        {"write_port", required_argument, 0, 'w'},
        {"read_baudrate", required_argument, 0, 'b'},
        {"write_baudrate", required_argument, 0, 'a'},
        {"websocket_port", required_argument, 0, 'p'},
        {0, 0, 0, 0}
    };

    int opt;
    int option_index = 0;
    while ((opt = getopt_long(argc, argv, "c:r:w:b:a:p:", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'c':
                config_path = optarg;
                break;
            case 'r':
                read_port = optarg;
                break;
            case 'w':
                write_port = optarg;
                break;
            case 'b':
                read_baudrate = std::stoi(optarg);
                break;
            case 'a':
                write_baudrate = std::stoi(optarg);
                break;
            case 'p':
                websocket_port = std::stoi(optarg);
                break;
            default:
                print_usage();
                return 1;
        }
    }

    std::cout << "Starting application" << std::endl;
    std::cout << "Config path: " << config_path << std::endl;
    std::cout << "Read port: " << read_port << std::endl;
    std::cout << "Read baudrate: " << read_baudrate << std::endl;
    std::cout << "Write port: " << write_port << std::endl;
    std::cout << "Write baudrate: " << write_baudrate << std::endl;
    std::cout << "WebSocket port: " << websocket_port << std::endl;

    // Load configuration file as RadarConfig object
    RadarConfig radarConfig;
    radarConfig.loadFromFile(config_path);
    std::cout << "RadarConfig loaded" << std::endl;
    // TODO: this should be logged only in debug profile
    std::cout << radarConfig.toString() << std::endl;
    std::cout << radarConfig.to_json_pretty() << std::endl;

    // Setup websocket server
    boost::asio::io_context io_context;
    std::cout << "Starting WebSocket server" << std::endl;
    WebSocketServer ws_server(io_context);
    std::thread ws_thread([&ws_server, websocket_port]() {
        ws_server.run(websocket_port);
    });

    // Setup UART connection
    UARTConnection uart(io_context, write_port, write_baudrate, read_port, read_baudrate);

    // Write configuration to UART
    uart.write_config(radarConfig);

    // Start a thread to process the buffer and broadcast to WebSocket clients
    std::thread processing_thread(process_buffer, std::ref(uart), std::ref(ws_server));

    // Read bitstream from UART
    uart.read_bitstream(ws_server);

    processing_thread.join();
    ws_thread.join();

    return 0;
}