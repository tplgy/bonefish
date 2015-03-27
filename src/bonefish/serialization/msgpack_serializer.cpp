#include <bonefish/serialization/msgpack_serializer.hpp>
#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/messages/wamp_message_factory.hpp>

#include <iostream>
#include <msgpack.hpp>
#include <sstream>
#include <stdexcept>

namespace msgpack {
MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS) {

bool reference_func(type::object_type type, uint64_t length, void* user_data) {
    // Never copy any STR, BIN or EXT out of the client buffer on parsing,
    // since we create a deep copy of the unpacked item anyway.
    return false;
}

} // namespace <current msgpack version>
} // namespace msgpack

namespace bonefish {

namespace {

struct raw_buffer_writer
{
    raw_buffer_writer(char* buffer, std::size_t length)
        : m_start(buffer)
        , m_current(buffer)
        , m_end(buffer + length)
        , m_buffer_too_small(false)
    {
    }

    std::size_t bytes_written() const { return m_current - m_start; }
    bool buffer_too_small() const { return m_buffer_too_small; }

    void write(const char* src, std::size_t srclen)
    {
        if (m_current >= m_end) {
            if (srclen) {
                m_buffer_too_small = true;
            }
            return; // EOF
        }

        if (m_current + srclen <= m_end) {
            std::memcpy(m_current, src, srclen);
            m_current += srclen;
        } else {
            std::memcpy(m_current, src, m_end - m_current);
            m_current = m_end;
            m_buffer_too_small = true;
        }
    }

private:
    char* m_start;
    char* m_current;
    char* m_end;
    bool m_buffer_too_small;
};

} // anonymous namespace

wamp_message* msgpack_serializer::deserialize(const char* buffer, size_t length) const
{
    msgpack::unpacked item = msgpack::unpack(buffer, length, msgpack::reference_func);

    std::vector<msgpack::object> fields;
    item.get().convert(&fields);

    wamp_message_type type = static_cast<wamp_message_type>(fields[0].as<unsigned>());
    std::unique_ptr<wamp_message> message(wamp_message_factory::create_message(type));
    if (message) {
        message->unmarshal(fields);
    } else {
        throw std::runtime_error("no deserializer defined for message");
    }

    return message.release();
}

size_t msgpack_serializer::serialize(const wamp_message* message, char* buffer, size_t length) const
{
    raw_buffer_writer writer(buffer, length);
    msgpack::packer<raw_buffer_writer> packer(&writer);
    packer.pack(message->marshal());

    if (writer.buffer_too_small()) {
        throw std::overflow_error("serialization buffer too small");
    }

    return writer.bytes_written();
}

} // namespace bonefish
