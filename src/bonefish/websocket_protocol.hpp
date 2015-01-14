#ifndef BONEFISH_WEBSOCKET_PROTOCOL_HPP
#define BONEFISH_WEBSOCKET_PROTOCOL_HPP

#include <bonefish/identifier/session_id.hpp>
#include <websocketpp/config/asio_no_tls.hpp>

namespace bonefish {

extern const std::string WAMPV2_JSON_SUBPROTOCOL;
extern const std::string WAMPV2_MSGPACK_SUBPROTOCOL;

} // namespace bonefish

#endif // BONEFISH_WEBSOCKET_PROTOCOL_HPP
