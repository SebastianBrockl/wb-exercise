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
    std::vector<websocketpp::connection_hdl> m_open_connections;

    /**
     * Add a connection to the list of open connections.
     */
    void add_connection(websocketpp::connection_hdl hdl);

    /**
     * Remove a connection from the list of open connections.
     */
    void remove_connection(websocketpp::connection_hdl hdl);

    /**
     * Broadcast a message to all open connections.
     */
    void broadcast_message(std::shared_ptr<std::string> message);

    // Callback to handle incoming messages
    void on_message(websocketpp::connection_hdl hdl, server_t::message_ptr msg);

    // Callback to handle open connections
    void on_open(websocketpp::connection_hdl hdl);

    // Callback to handle close connections
    void on_close(websocketpp::connection_hdl hdl);
};

#endif // WEBSOCKET_SERVER_H
