#include <bonefish/serialization/msgpack_welcome_message_serializer.hpp>
#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/messages/welcome_message.hpp>
#include <bonefish/roles/role.hpp>
#include <bonefish/roles/role_type.hpp>
#include <cstring>
#include <stdexcept>
#include <string>

namespace bonefish {

welcome_message* msgpack_welcome_message_serializer::deserialize(const std::vector<msgpack::object>&)
{
    throw(std::logic_error("welcome message serializer - deserialize is not implemented"));
}

size_t msgpack_welcome_message_serializer::serialize(const welcome_message* message,
        char* buffer, size_t length)
{
    if (message == nullptr) {
        throw(std::invalid_argument("welcome message serializer - null welcome message"));
    }

    msgpack::sbuffer sbuffer;
    msgpack::packer<msgpack::sbuffer> packer(&sbuffer);

    packer.pack_array(3);

    packer.pack(static_cast<unsigned>(message->get_type()));
    packer.pack(message->get_session_id().id());

    //
    // Roles
    //
    // TODO: Add support for agent when advanced profile is added.
    // TODO: Add support for features when advanced profile is added.
    packer.pack_map(1);
    packer.pack(std::string("roles"));

    const std::vector<role>& roles = message->get_roles();
    packer.pack_map(roles.size());
    for (const auto& r : roles) {
        packer.pack(role_type_to_string(r.get_type()));
        packer.pack(r.get_features().get_features());
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
