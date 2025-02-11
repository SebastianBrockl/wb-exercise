#include "websocket_server.h"
#include "util.h"
#include "RadarConfig.h"
#include <iostream>
#include <getopt.h>

void print_usage() {
    std::cout << "Usage: mistral-demo [options]\n"
              << "Options:\n"
              << "  --config_path <path>      Path to the default config file (default: /app/default-config.cfg)\n"
              << "  --read_port <port>        Read UART interface (default: /dev/ttyUSB0)\n"
              << "  --write_port <port>       Write UART interface (default: /dev/ttyUSB1)\n"
              << "  --read_baudrate <baud>    Read UART baudrate (default: 9600)\n"
              << "  --write_baudrate <baud>   Write UART baudrate (default: 9600)\n"
              << std::endl;
}

int main(int argc, char* argv[]) {
    std::string config_path = "/app/default-config.cfg";
    std::string read_port = "/dev/ttyUSB0";
    std::string write_port = "/dev/ttyUSB1";
    int read_baudrate = 9600;
    int write_baudrate = 9600;

    static struct option long_options[] = {
        {"config_path", required_argument, 0, 'c'},
        {"read_port", required_argument, 0, 'r'},
        {"write_port", required_argument, 0, 'w'},
        {"read_baudrate", required_argument, 0, 'b'},
        {"write_baudrate", required_argument, 0, 'a'},
        {0, 0, 0, 0}
    };

    int opt;
    int option_index = 0;
    while ((opt = getopt_long(argc, argv, "c:r:w:b:a:", long_options, &option_index)) != -1) {
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


    // Load configuration file as RadarConfig object
    RadarConfig radarConfig;
    radarConfig.loadFromFile(config_path);
    std::cout << "RadarConfig loaded" << std::endl;
    // TODO: this should be logged only in debug profile
    std::cout << radarConfig.toString() << std::endl;
    std::cout << radarConfig.to_json_pretty() << std::endl;

    // Setup websocket server
    std::cout << "Starting WebSocket server" << std::endl;
    WebSocketServer ws_server;
    ws_server.run(9002); // Run WebSocket server on port 9002

    return 0;
}