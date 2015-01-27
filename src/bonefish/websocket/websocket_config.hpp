#ifndef BONEFISH_WEBSOCKET_WEBSOCKET_CONFIG_HPP
#define BONEFISH_WEBSOCKET_WEBSOCKET_CONFIG_HPP

#include <bonefish/websocket/websocket_data.hpp>
#include <websocketpp/config/asio_no_tls.hpp>

namespace bonefish {

class websocket_config : public websocketpp::config::asio
{
public:
    // pull default settings from our core config
    typedef websocketpp::config::asio core;

    typedef core::concurrency_type concurrency_type;
    typedef core::request_type request_type;
    typedef core::response_type response_type;
    typedef core::message_type message_type;
    typedef core::con_msg_manager_type con_msg_manager_type;
    typedef core::endpoint_msg_manager_type endpoint_msg_manager_type;
    typedef core::alog_type alog_type;
    typedef core::elog_type elog_type;
    typedef core::rng_type rng_type;
    typedef core::transport_type transport_type;
    typedef core::endpoint_base endpoint_base;

    // Set a custom connection_base class
    typedef websocket_data connection_base;
};

} // namespace bonefish

#endif // BONEFISH_WEBSOCKET_WEBSOCKET_CONFIG_HPP
