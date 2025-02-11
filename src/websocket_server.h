#ifndef WEBSOCKET_SERVER_H
#define WEBSOCKET_SERVER_H

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <boost/asio.hpp>
#include <vector>
#include <memory>
#include <algorithm>
#include <iostream>

class WebSocketServer {
public:
    using server_t = websocketpp::server<websocketpp::config::asio>;
    using connection_t = server_t::connection_type;

    WebSocketServer(boost::asio::io_context& io_context);

    // callbacks for connection events
    void on_message(websocketpp::connection_hdl hdl, server_t::message_ptr msg);
    void on_open(websocketpp::connection_hdl hdl);
    void on_close(websocketpp::connection_hdl hdl);
    // start server on given port
    void run(uint16_t port);
    // broadcast message to all connected clients
    void broadcast(const std::string& message);

private:
    void erase(websocketpp::connection_hdl hdl);
    void erase(std::shared_ptr<connection_t> connection);

    server_t m_server;
    std::vector<std::shared_ptr<connection_t>> m_connections;
};

#endif // WEBSOCKET_SERVER_H
