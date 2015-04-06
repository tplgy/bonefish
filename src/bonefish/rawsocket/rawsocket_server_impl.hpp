#ifndef BONEFISH_RAWSOCKET_SERVER_IMPL_HPP
#define BONEFISH_RAWSOCKET_SERVER_IMPL_HPP

#include <bonefish/common/wamp_message_processor.hpp>

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <set>
#include <memory>

namespace bonefish {

class rawsocket_connection;
class rawsocket_listener;
class wamp_routers;
class wamp_serializers;

class rawsocket_server_impl : public std::enable_shared_from_this<rawsocket_server_impl>
{
public:
    rawsocket_server_impl(boost::asio::io_service& io_service,
            const std::shared_ptr<wamp_routers>& routers,
            const std::shared_ptr<wamp_serializers>& serializers);
    ~rawsocket_server_impl();

    void attach_listener(const std::shared_ptr<rawsocket_listener>& listener);
    void start();
    void shutdown();

private:
    void on_connect(const std::shared_ptr<rawsocket_connection>& connection);
    void on_close(const std::shared_ptr<rawsocket_connection>& connection);
    void on_fail(const std::shared_ptr<rawsocket_connection>& connection, const char* reason);
    void on_message(const std::shared_ptr<rawsocket_connection>& connection,
            const char* buffer, size_t length);

private:
    boost::asio::io_service& m_io_service;
    std::shared_ptr<wamp_routers> m_routers;
    std::shared_ptr<wamp_serializers> m_serializers;

    std::set<std::shared_ptr<rawsocket_listener>,
            std::owner_less<std::shared_ptr<rawsocket_listener>>> m_listeners;

    std::set<std::shared_ptr<rawsocket_connection>,
            std::owner_less<std::shared_ptr<rawsocket_connection>>> m_connections;

    wamp_message_processor m_message_processor;
};

} // namespace bonefish

#endif // BONEFISH_RAWSOCKET_SERVER_IMPL_HPP
