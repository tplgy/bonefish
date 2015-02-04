#include <bonefish/serialization/msgpack_serializer.hpp>
#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/messages/wamp_message_factory.hpp>

#include <iostream>
#include <msgpack.hpp>
#include <sstream>
#include <stdexcept>

namespace bonefish {

wamp_message* msgpack_serializer::deserialize(const char* buffer, size_t length) const
{
    std::unique_ptr<wamp_message> message;

    // TODO: The extra copy here is lame. Find a more efficient way to
    //       just work on the buffer directly.
    msgpack::unpacked item;
    msgpack::unpacker unpacker;
    unpacker.reserve_buffer(length);
    memcpy(unpacker.buffer(), buffer, length);
    unpacker.buffer_consumed(length);

    std::vector<msgpack::object> fields;
    unpacker.next(&item);
    item.get().convert(&fields);

    wamp_message_type type = static_cast<wamp_message_type>(fields[0].as<unsigned>());
    message.reset(wamp_message_factory::create_message(type));
    if (message) {
        message->unmarshal(fields);
    } else {
        throw std::runtime_error("no deserializer defined for message");
    }

    return message.release();
}

size_t msgpack_serializer::serialize(const wamp_message* message, char* buffer, size_t length) const
{
    msgpack::sbuffer sbuffer;
    msgpack::packer<msgpack::sbuffer> packer(&sbuffer);
    packer.pack(message->marshal());

    if (sbuffer.size() > length) {
        throw std::overflow_error("serialization buffer too small");
    }

    // TODO: Fix this ugly copy. It would be nice if we could just
    //       serialize directly into the buffer.
    std::memcpy(buffer, sbuffer.data(), sbuffer.size());
    return sbuffer.size();
}

} // namespace bonefish
