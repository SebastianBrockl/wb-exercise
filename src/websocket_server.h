#ifndef WEBSOCKET_SERVER_H
#define WEBSOCKET_SERVER_H

#include "i_message_publisher.h"

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <boost/asio.hpp>
#include <vector>
#include <memory>
#include <algorithm>
#include <iostream>

namespace msg
{
    class WebSocketServer : public IMessagePublisher
    {
    public:
        using server_t = websocketpp::server<websocketpp::config::asio>;
        using connection_t = server_t::connection_type;

        WebSocketServer(boost::asio::io_context &io_context, uint16_t port);
        ~WebSocketServer() = default;

        // callbacks for connection events
        void on_message(websocketpp::connection_hdl hdl, server_t::message_ptr msg);
        void on_open(websocketpp::connection_hdl hdl);
        void on_close(websocketpp::connection_hdl hdl);
        // start server on given port
        void run(uint16_t port);
        // broadcast message to all connected clients
        void broadcast(const std::string &message);
        // gracefully stop the server
        void stop();


        // IMessagePublisher interface
        void publish_frame(const Frame &frame) override;

        void publish_pointcloud(const CompressedPointCloudTLV &pointcloud) override;
        void publish_target_list(const TargetListTLV &target_list) override;
        void publish_target_index(const targetIndexTLV &target_index) override;
        void publish_presence_indication(const PresenceIndicationTLV &presence_indication) override;
        void publish_target_height(const TargetHeightTLV &target_height) override;

    private:
        void erase(websocketpp::connection_hdl hdl);
        void erase(std::shared_ptr<connection_t> connection);

        server_t m_server;
        std::vector<std::shared_ptr<connection_t>> m_connections;
    };

}

#endif // WEBSOCKET_SERVER_H
