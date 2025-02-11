#include "controller.h"
#include <iostream>
#include <getopt.h>

void handleSignal(boost::asio::io_context &io, const boost::system::error_code & /*error*/, int /*signal_number*/)
{
    std::cout << "\n[INFO] Received SIGINT. Cleaning up..." << std::endl;

    // Perform cleanup (e.g., closing WebSocket connections, saving state)
    // ws_server.closeAllConnections();
    // serial_reader.close();

    io.stop();
}

void print_usage()
{
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

int main(int argc, char *argv[])
{
    std::string config_path = "/app/default-config.cfg";
    std::string read_port = "/dev/ttyUSB0";
    std::string write_port = "/dev/ttyUSB1";
    int read_baudrate = 921600;
    int write_baudrate = 115200;
    uint16_t websocket_port = 9002;

    static struct option long_options[] = {
        {"config_path", required_argument, 0, 'c'},
        {"read_port", required_argument, 0, 'r'},
        {"write_port", required_argument, 0, 'w'},
        {"read_baudrate", required_argument, 0, 'b'},
        {"write_baudrate", required_argument, 0, 'a'},
        {"websocket_port", required_argument, 0, 'p'},
        {0, 0, 0, 0}};

    int opt;
    int option_index = 0;
    while ((opt = getopt_long(argc, argv, "c:r:w:b:a:p:", long_options, &option_index)) != -1)
    {
        switch (opt)
        {
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

    // Setup boost::asio::io_context
    boost::asio::io_context io_context;

    // Capture Ctrl+C (SIGINT) and gracefully stop io_context
    boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
    signals.async_wait([&io_context](const boost::system::error_code &error, int signal_number)
                       { handleSignal(io_context, error, signal_number); });

    // Create and run the controller
    Controller controller(io_context, config_path, read_port, read_baudrate, write_port, write_baudrate, websocket_port);
    // controller.run();
    io_context.run();

    return 0;
}