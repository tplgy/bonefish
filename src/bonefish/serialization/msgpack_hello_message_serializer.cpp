#include <bonefish/serialization/msgpack_hello_message_serializer.hpp>
#include <bonefish/messages/wamp_message_type.hpp>
#include <bonefish/messages/wamp_hello_message.hpp>
#include <bonefish/roles/wamp_role.hpp>
#include <bonefish/roles/wamp_role_type.hpp>
#include <bonefish/uri.hpp>
#include <memory>
#include <stdexcept>
#include <string>

namespace bonefish {

wamp_hello_message* msgpack_hello_message_serializer::deserialize(
        const std::vector<msgpack::object>& fields)
{
    if (fields.size() != 3) {
        throw(std::invalid_argument("hello message serializer - invalid number of fields"));
    }

    wamp_message_type type = static_cast<wamp_message_type>(fields[0].as<unsigned>());
    if (type != wamp_message_type::HELLO) {
        throw(std::invalid_argument("hello message serializer - invalid message type"));
    }

    std::string realm = fields[1].as<std::string>();
    if (!is_valid_uri(realm)) {
        throw(std::invalid_argument("hello message serializer - invalid realm uri"));
    }

    std::unordered_map<std::string, msgpack::object> details;
    fields[2].convert(&details);

    std::unique_ptr<wamp_hello_message> hello_message(new wamp_hello_message);
    hello_message->set_realm(realm);

    //
    // Agent
    //
    // TODO: Add agent as per advanced profile support

    //
    // Roles
    //
    // TODO: Add features as per advanced profile support
    std::unordered_map<std::string, msgpack::object> roles;
    auto details_itr = details.find("roles");
    if (details_itr == details.end()) {
        throw(std::invalid_argument("hello message serializer - no roles specified"));
    }

    for (const auto& role_itr : roles) {
        hello_message->add_role(wamp_role(role_type_from_string(role_itr.first)));
    }

    return hello_message.release();
}

size_t msgpack_hello_message_serializer::serialize(const wamp_hello_message*, char*, size_t)
{
    throw(std::logic_error("hello message serializer - serialize is not implemented"));
}

} // namespace bonefish
