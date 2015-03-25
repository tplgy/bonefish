#ifndef BONEFISH_TCP_TRANSPORT_HPP
#define BONEFISH_TCP_TRANSPORT_HPP

#include <bonefish/transport/wamp_transport.hpp>

#include <iostream>
#include <memory>

namespace bonefish {

class tcp_connection;
class wamp_message;
class wamp_serializer;

class tcp_transport : public wamp_transport
{
public:
    tcp_transport(
            const std::shared_ptr<wamp_serializer>& serializer,
            const std::shared_ptr<tcp_connection>& connection);

    virtual bool send_message(const wamp_message* message) override;

private:
    std::shared_ptr<wamp_serializer> m_serializer;
    std::shared_ptr<tcp_connection> m_connection;
};

} // namespace bonefish

#endif // BONEFISH_TCP_TRANSPORT_HPP
