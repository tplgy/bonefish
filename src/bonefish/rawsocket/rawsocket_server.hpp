#ifndef BONEFISH_RAWSOCKET_SERVER_HPP
#define BONEFISH_RAWSOCKET_SERVER_HPP

#include <boost/asio/ip/tcp.hpp>
#include <set>
#include <memory>

namespace bonefish {

class rawsocket_listener;
class rawsocket_server_impl;
class wamp_routers;
class wamp_serializers;

class rawsocket_server
{
public:
    rawsocket_server(
            const std::shared_ptr<wamp_routers>& routers,
            const std::shared_ptr<wamp_serializers>& serializers);
    ~rawsocket_server();

    void attach_listener(const std::shared_ptr<rawsocket_listener>& listener);
    void start();
    void shutdown();

private:
    std::shared_ptr<rawsocket_server_impl> m_impl;
};

} // namespace bonefish

#endif // BONEFISH_RAWSOCKET_SERVER_HPP
