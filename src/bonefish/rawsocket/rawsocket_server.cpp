#include <bonefish/rawsocket/rawsocket_server.hpp>
#include <bonefish/identifiers/wamp_session_id_generator.hpp>
#include <bonefish/router/wamp_routers.hpp>
#include <bonefish/serialization/wamp_serializers.hpp>
#include <bonefish/rawsocket/rawsocket_server_impl.hpp>

namespace bonefish {

rawsocket_server::rawsocket_server(
        const std::shared_ptr<wamp_routers>& routers,
        const std::shared_ptr<wamp_serializers>& serializers)
    : m_impl(new rawsocket_server_impl(routers, serializers))
{
}

rawsocket_server::~rawsocket_server()
{
}

void rawsocket_server::attach_listener(const std::shared_ptr<rawsocket_listener>& listener)
{
    m_impl->attach_listener(listener);
}

void rawsocket_server::start()
{
    m_impl->start();
}

void rawsocket_server::shutdown()
{
    m_impl->shutdown();
}

} // namespace bonefish
