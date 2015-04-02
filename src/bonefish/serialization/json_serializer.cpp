#include <bonefish/serialization/json_serializer.hpp>

#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/messages/wamp_message_factory.hpp>
#include <bonefish/serialization/base64.hpp>
#include <bonefish/serialization/json_msgpack_sax.hpp>

#include <iostream>
#include <msgpack.hpp>
#include <rapidjson/rapidjson.h>
#include <rapidjson/error/en.h>
#include <rapidjson/reader.h>
#include <rapidjson/writer.h>
#include <sstream>
#include <stdexcept>

namespace bonefish {

namespace {

// The deserialize() API takes a length argument, so the buffer might not
// be zero-terminated. Use a custom rapidjson stream to supply that limit.
class imemstream
{
public:
    typedef char Ch;
    imemstream(const Ch* buffer, size_t size)
        : m_buffer(buffer)
        , m_current(buffer)
        , m_end(buffer + size)
    {
    }
    imemstream(const imemstream&) = delete;
    imemstream& operator=(const imemstream&) = delete;

    Ch Peek() const
    {
        if (m_current >= m_end) { return '\0'; }
        return *m_current;
    }
    Ch Take()
    {
        if (m_current >= m_end) { return '\0'; }
        return *(m_current++);
    }
    size_t Tell() const { return static_cast<size_t>(m_current - m_buffer); }

    // Write functions for in-situ parsing, not currently implemented.
    Ch* PutBegin() { assert(false); return 0; }
    void Put(Ch) { assert(false); }
    void Flush() { assert(false); }
    size_t PutEnd(Ch*) { assert(false); return 0; }

private:
    const Ch* m_buffer;
    const Ch* m_current;
    const Ch* m_end;
};

// The serialize() API takes a length argument, so the buffer might not
// be zero-terminated. Use a custom rapidjson stream to supply that limit.
class omemstream
{
public:
    typedef char Ch;
    omemstream(Ch* buffer, size_t size)
        : m_buffer(buffer)
        , m_current(buffer)
        , m_end(buffer + size)
        , m_buffer_too_small(false)
    {
    }
    omemstream(const omemstream&) = delete;
    omemstream& operator=(const omemstream&) = delete;

    Ch Peek() const { assert(false); return '\0'; }
    Ch Take() { assert(false); return '\0'; }
    size_t Tell() const { return static_cast<size_t>(m_current - m_buffer); }

    void Put(Ch ch)
    {
        if (m_current >= m_end) {
            m_buffer_too_small = true;
            return;
        }
        *(m_current++) = ch;
    }
    void Flush() { } // nothing to do, Put() writes immediately

    Ch* PutBegin() { assert(false); return 0; }
    size_t PutEnd(Ch*) { assert(false); return 0; }

    bool BufferTooSmall() const { return m_buffer_too_small; }

private:
    Ch* m_buffer;
    Ch* m_current;
    Ch* m_end;
    bool m_buffer_too_small;
};

struct wamp_bin_string_conversion
{
    using convert_from_string = bool;

    static bool should_convert_from_string(const char* str, size_t length)
    {
        return length > 0 && str[0] == '\0';
    }

    static bool to_bool(const convert_from_string& convert) { return convert; }

    // Any type can be returned instead of void, as long as convert() takes the same type.
    static void* to_userdata(const convert_from_string&) { return nullptr; }

    static bool convert(
            msgpack::object& o, msgpack::zone& zone,
            const char* str, size_t length, bool copy,
            void* userdata)
    {
        (void) copy; // unused
        (void) userdata;

        size_t decode_size = base64::decode_size(str + 1, length - 1);
        char* decoded = static_cast<char*>(zone.allocate_align(decode_size));
        if (!decoded) { return false; }

        try {
            base64::decode(decoded, decode_size, str + 1, length - 1);
        } catch (boost::archive::iterators::dataflow_exception&) {
            return false;
        }

        o.type = msgpack::type::BIN;
        o.via.bin.ptr = decoded;
        o.via.bin.size = decode_size;

        return true;
    }

    template <typename Writer>
    static bool from_bin(Writer& writer, const msgpack::object& o)
    {
        size_t encode_size = base64::encode_size(o.via.bin.ptr, o.via.bin.size);
        std::vector<char> encoded(1 + encode_size);
        encoded.at(1) = '\0';

        try {
            base64::encode(encoded.data() + 1, encode_size, o.via.bin.ptr, o.via.bin.size);
        } catch (boost::archive::iterators::dataflow_exception&) {
            return false;
        }

        return writer.String(encoded.data(), encode_size);
    }

    template <typename Writer>
    static bool from_ext(Writer&, const msgpack::object&) { return false; }
};

} // anonymous namespace

wamp_message* json_serializer::deserialize(const char* buffer, size_t length) const
{
    msgpack::zone zone;
    msgpack::object item;

    imemstream bufferstream(buffer, length);
    serialization::msgpack_from_json_handler<wamp_bin_string_conversion> handler(item, zone);
    rapidjson::Reader reader;
    reader.Parse(bufferstream, handler);

    if (reader.HasParseError()) {
        std::ostringstream strstr;
        strstr << "failed to parse message: " << rapidjson::GetParseError_En(reader.GetParseErrorCode());
        throw std::runtime_error(strstr.str());
    }

    std::vector<msgpack::object> fields;
    item.convert(&fields);

    wamp_message_type type = static_cast<wamp_message_type>(fields[0].as<unsigned>());
    std::unique_ptr<wamp_message> message(wamp_message_factory::create_message(type));
    if (message) {
        message->unmarshal(fields);
    } else {
        throw std::runtime_error("no deserializer defined for message");
    }

    return message.release();
}

size_t json_serializer::serialize(const wamp_message* message, char* buffer, size_t length) const
{
    omemstream bufferstream(buffer, length);
    rapidjson::Writer<omemstream> writer(bufferstream);

    const std::vector<msgpack::object>& fields = message->marshal();
    bool write_failed = false;

    do {
        if (!writer.StartArray()) {
            write_failed = true;
            break;
        }

        for (const msgpack::object& field : fields) {
            if (!serialization::write_json<decltype(writer), wamp_bin_string_conversion>(writer, field)) {
                write_failed = true;
                break;
            }
        }
        if (write_failed) {
            break; // escaping outer loop
        }

        if (!writer.EndArray()) {
            write_failed = true;
            break;
        }
    } while (false);

    if (bufferstream.BufferTooSmall()) {
        throw std::overflow_error("serialization buffer too small");
    } else if (write_failed || !writer.IsComplete()) {
        throw std::overflow_error("failed or incomplete serialization");
    }

    return bufferstream.Tell();
}

} // namespace bonefish
