#include "websocket_server.h"

WebSocketServer::WebSocketServer(boost::asio::io_context& io_context, uint16_t port) {
    m_server.init_asio(&io_context);
    m_server.set_message_handler(std::bind(&WebSocketServer::on_message, this, std::placeholders::_1, std::placeholders::_2));
    m_server.set_open_handler(std::bind(&WebSocketServer::on_open, this, std::placeholders::_1));
    m_server.set_close_handler(std::bind(&WebSocketServer::on_close, this, std::placeholders::_1));
    m_server.listen(port);
    m_server.start_accept();
    io_context.post([port]() {
        std::cout << "WS Server: WebSocket server running on port " << port << std::endl;
    });
}

void WebSocketServer::on_message(websocketpp::connection_hdl hdl, server_t::message_ptr msg) {
    std::cout << "WS Server: Received incoming message: " << msg->get_payload() << std::endl;
}

/**
 * Stores new websocket connections for broadcasting messages.
 */
void WebSocketServer::on_open(websocketpp::connection_hdl hdl) {
    auto connection = m_server.get_con_from_hdl(hdl);
    std::cout << "WS Server: New connection from " << connection->get_remote_endpoint() << std::endl;
    m_connections.push_back(connection);
    std::cout << "WS Server: Total connections: " << m_connections.size() << std::endl;
}

/**
 * Removes closed connections from the list of connections.
 */
void WebSocketServer::on_close(websocketpp::connection_hdl hdl) {
    erase(hdl);
}

void WebSocketServer::erase(websocketpp::connection_hdl hdl) {
    const auto connection = m_server.get_con_from_hdl(hdl);
    erase(connection);
}

void WebSocketServer::erase(std::shared_ptr<connection_t> connection) {
    std::cout << "WS Server: Connection closed from " << connection->get_remote_endpoint() << std::endl;

    // "erase-remove idiom" to remove the connection from the vector
    m_connections.erase(std::remove_if(m_connections.begin(), m_connections.end(),
        [&connection](const std::shared_ptr<connection_t>& conn) {
            return conn == connection;
        }), m_connections.end());

    std::cout << "WS Server: Total connections: " << m_connections.size() << std::endl;
}

/**
 * Start the server on the specified port.
 */
void WebSocketServer::run(uint16_t port) {
    m_server.listen(port);
    m_server.start_accept();
    // log that server is running on it's own asio thread
    //m_server.run();
    //std::cout << "Server running" << std::endl;
}

void WebSocketServer::broadcast(const std::string& message) {
    for (const auto& connection : m_connections) {
        if (connection->get_state() == websocketpp::session::state::closed) {
            // cleanup for sanity & paranoia, on_close() should have already triggered
            erase(connection);
        } else {
            m_server.send(connection, message, websocketpp::frame::opcode::text);
        }
    }
}

void WebSocketServer::stop()
{
    // close open connections for graceful shutdown
    for (const auto& connection : m_connections) {
        m_server.close(connection, websocketpp::close::status::going_away, "Server shutting down");
    }
}
