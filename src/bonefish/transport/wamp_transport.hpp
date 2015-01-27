#ifndef BONEFISH_TRANSPORT_WAMP_TRANSPORT_HPP
#define BONEFISH_TRANSPORT_WAMP_TRANSPORT_HPP

#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/serialization/wamp_serializer.hpp>
#include <memory>
#include <websocketpp/common/connection_hdl.hpp>
#include <websocketpp/server.hpp>

namespace bonefish {

class wamp_message;

class wamp_transport
{
public:
    wamp_transport();
    virtual ~wamp_transport();
    virtual bool send_message(const wamp_message* message) = 0;
};

inline wamp_transport::wamp_transport()
{
}

inline wamp_transport::~wamp_transport()
{
}

} // namespace bonefish

#endif // BONEFISH_TRANSPORT_WAMP_TRANSPORT_HPP
