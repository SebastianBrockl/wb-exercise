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
              << "  --data_port <port>        Read UART interface (default: /dev/ttyUSB0)\n"
              << "  --config_port <port>      Write UART interface (default: /dev/ttyUSB1)\n"
              << "  --data_baudrate <baud>    Read UART baudrate (default: 921600)\n"
              << "  --config_baudrate <baud>  Write UART baudrate (default: 115200)\n"
              << "  --websocket_port <port>   WebSocket server port (default: 9002)\n"
              << std::endl;
}

int main(int argc, char *argv[])
{
    std::string config_path = "/app/default-config.cfg";
    std::string data_port = "/dev/ttyUSB0";
    std::string config_port = "/dev/ttyUSB1";
    int data_baudrate = 921600;
    int config_baudrate = 115200;
    uint16_t websocket_port = 9002;

    static struct option long_options[] = {
        {"config_path", required_argument, 0, 'c'},
        {"data_port", required_argument, 0, 'r'},
        {"config_port", required_argument, 0, 'w'},
        {"data_baudrate", required_argument, 0, 'b'},
        {"config_baudrate", required_argument, 0, 'a'},
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
            data_port = optarg;
            break;
        case 'w':
            config_port = optarg;
            break;
        case 'b':
            data_baudrate = std::stoi(optarg);
            break;
        case 'a':
            config_baudrate = std::stoi(optarg);
            break;
        case 'p':
            websocket_port = std::stoi(optarg);
            break;
        default:
            print_usage();
            return 1;
        }
    }

    std::cout << "Main: Starting application" << std::endl;
    std::cout << "Main: Config path: " << config_path << std::endl;
    std::cout << "Main: Read port: " << data_port << std::endl;
    std::cout << "Main: Read baudrate: " << data_baudrate << std::endl;
    std::cout << "Main: Write port: " << config_port << std::endl;
    std::cout << "Main: Write baudrate: " << config_baudrate << std::endl;
    std::cout << "Main: WebSocket port: " << websocket_port << std::endl;

    // Setup boost::asio::io_context
    boost::asio::io_context io_context;

    // Capture Ctrl+C (SIGINT) and gracefully stop io_context
    boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
    signals.async_wait([&io_context](const boost::system::error_code &error, int signal_number)
                       { handleSignal(io_context, error, signal_number); });

    // Create and run the controller
    Controller controller(io_context, config_path, data_port, data_baudrate, config_port, config_baudrate, websocket_port);
    // controller.run();
    io_context.run();

    return 0;
}