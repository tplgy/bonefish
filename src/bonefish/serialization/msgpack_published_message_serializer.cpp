#include <bonefish/serialization/msgpack_published_message_serializer.hpp>
#include <bonefish/identifiers/wamp_request_id.hpp>
#include <bonefish/identifiers/wamp_publication_id.hpp>
#include <bonefish/messages/wamp_message_type.hpp>
#include <bonefish/messages/wamp_published_message.hpp>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>

namespace bonefish {

wamp_published_message* msgpack_published_message_serializer::deserialize(
        const std::vector<msgpack::object>& fields)
{
    throw(std::logic_error("deserialize is not implemented"));
}

size_t msgpack_published_message_serializer::serialize(const wamp_published_message* published_message,
        char* buffer, size_t length)
{
    if (published_message == nullptr) {
        throw(std::invalid_argument("null message"));
    }

    msgpack::sbuffer sbuffer;
    msgpack::packer<msgpack::sbuffer> packer(&sbuffer);

    packer.pack_array(3);

    packer.pack(static_cast<unsigned>(published_message->get_type()));
    packer.pack(published_message->get_request_id().id());
    packer.pack(published_message->get_publication_id().id());

    if (sbuffer.size() > length) {
        throw(std::overflow_error("serialization buffer too small"));
    }

    // TODO: Fix this ugly copy. It would be nice if we could just
    //       serialize directly into the buffer.
    std::memcpy(buffer, sbuffer.data(), sbuffer.size());
    return sbuffer.size();
}

} // namespace bonefish
