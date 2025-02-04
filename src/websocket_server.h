#ifndef WEBSOCKET_SERVER_H
#define WEBSOCKET_SERVER_H

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include <functional>
#include <iostream>
#include <vector>

class WebSocketServer {
public:
    WebSocketServer();
    void run(uint16_t port);


private:
    using server_t = websocketpp::server<websocketpp::config::asio>;
    server_t m_server;

    using connection_t = websocketpp::connection<websocketpp::config::asio>;
    std::vector<std::shared_ptr<connection_t>> m_connections;

    // Callback to handle incoming messages
    void on_message(websocketpp::connection_hdl hdl, server_t::message_ptr msg);

    // Callback to handle new connections
    void on_open(websocketpp::connection_hdl hdl);
};

#endif // WEBSOCKET_SERVER_H
