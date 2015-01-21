#include <bonefish/serialization/msgpack_welcome_message_serializer.hpp>
#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/messages/wamp_welcome_message.hpp>
#include <bonefish/roles/wamp_role.hpp>
#include <bonefish/roles/wamp_role_features.hpp>
#include <bonefish/roles/wamp_role_type.hpp>
#include <cstring>
#include <stdexcept>
#include <string>

namespace bonefish {

wamp_welcome_message* msgpack_welcome_message_serializer::deserialize(
        const std::vector<msgpack::object>&)
{
    throw(std::logic_error("welcome message serializer - deserialize is not implemented"));
}

size_t msgpack_welcome_message_serializer::serialize(const wamp_welcome_message* welcome_message,
        char* buffer, size_t length)
{
    if (welcome_message == nullptr) {
        throw(std::invalid_argument("welcome message serializer - null welcome message"));
    }

    msgpack::sbuffer sbuffer;
    msgpack::packer<msgpack::sbuffer> packer(&sbuffer);

    packer.pack_array(3);

    packer.pack(static_cast<unsigned>(welcome_message->get_type()));
    packer.pack(welcome_message->get_session_id().id());

    //
    // Roles
    //
    // TODO: Add support for agent when advanced profile is added.
    // TODO: Add support for features when advanced profile is added.
    packer.pack_map(1);
    packer.pack(std::string("roles"));

    const std::vector<wamp_role>& roles = welcome_message->get_roles();
    packer.pack_map(roles.size());
    for (const auto& role : roles) {
        packer.pack(role_type_to_string(role.get_type()));
        packer.pack(role.get_features().get_options());
    }

    if (sbuffer.size() > length) {
        throw(std::overflow_error("welcome message serializer - serialization buffer too small"));
    }

    // TODO: Fix this ugly copy. It would be nice if we could just
    //       serialize directly into the buffer.
    std::memcpy(buffer, sbuffer.data(), sbuffer.size());
    return sbuffer.size();
}

} // namespace bonefish
