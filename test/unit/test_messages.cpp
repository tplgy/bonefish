// Verifies wire-level compatibility: message type codes and field ordering
// for every message type Bonefish marshals, plus msgpack round-trips.

#include "bonefish_test.hpp"

#include <bonefish/messages/wamp_call_message.hpp>
#include <bonefish/messages/wamp_event_message.hpp>
#include <bonefish/messages/wamp_hello_message.hpp>
#include <bonefish/messages/wamp_invocation_message.hpp>
#include <bonefish/messages/wamp_message_factory.hpp>
#include <bonefish/messages/wamp_message_type.hpp>
#include <bonefish/messages/wamp_publish_message.hpp>
#include <bonefish/messages/wamp_register_message.hpp>
#include <bonefish/messages/wamp_result_message.hpp>
#include <bonefish/messages/wamp_subscribe_message.hpp>
#include <bonefish/messages/wamp_subscribed_message.hpp>
#include <bonefish/messages/wamp_welcome_message.hpp>
#include <bonefish/messages/wamp_yield_message.hpp>
#include <bonefish/serialization/expandable_buffer.hpp>
#include <bonefish/serialization/msgpack_serializer.hpp>

#include <msgpack.hpp>

using namespace bonefish;

struct msg_holder {
    msgpack::object_handle handle;
    msgpack::object obj() { return handle.get(); }
};

static msg_holder make_empty_map()
{
    // Raw msgpack encoding of an empty map is the single byte 0x80.
    return msg_holder { msgpack::unpack("\x80", 1) };
}

static void check_root_array(const expandable_buffer& buf, uint64_t code, size_t min_len)
{
    auto root_handle = msgpack::unpack(buf.data(), buf.size());
    msgpack::object root = root_handle.get();
    CHECK(root.type == msgpack::type::ARRAY);
    if (root.type != msgpack::type::ARRAY) {
        return;
    }
    CHECK(root.via.array.size >= min_len);
    if (root.via.array.size < min_len) {
        return;
    }
    CHECK_EQ(root.via.array.ptr[0].as<uint64_t>(), code);
}

int main()
{
    // Keeps the empty-map object's msgpack zone alive for the whole test.
    msg_holder empty_map = make_empty_map();

    msgpack_serializer serializer;

    // HELLO [1, realm, details]
    {
        wamp_hello_message msg;
        msg.set_realm("test.realm");
        msg.set_details(empty_map.obj());
        auto buf = serializer.serialize(msg);
        check_root_array(buf, 1, 3);
        auto root_handle = msgpack::unpack(buf.data(), buf.size());
        msgpack::object root = root_handle.get();
        CHECK_EQ(root.via.array.ptr[1].as<std::string>(), std::string("test.realm"));

        std::unique_ptr<wamp_message> decoded(serializer.deserialize(buf.data(), buf.size()));
        CHECK(dynamic_cast<wamp_hello_message*>(decoded.get()) != nullptr);
        CHECK_EQ(static_cast<wamp_hello_message*>(decoded.get())->get_realm(),
                 std::string("test.realm"));
    }

    // WELCOME [2, session, details]
    {
        wamp_welcome_message msg;
        msg.set_session_id(wamp_session_id(42));
        msg.set_details(empty_map.obj());
        auto buf = serializer.serialize(msg);
        check_root_array(buf, 2, 3);
        auto root_handle = msgpack::unpack(buf.data(), buf.size());
        msgpack::object root = root_handle.get();
        CHECK_EQ(root.via.array.ptr[1].as<uint64_t>(), uint64_t(42));

        std::unique_ptr<wamp_message> decoded(serializer.deserialize(buf.data(), buf.size()));
        CHECK_EQ(static_cast<wamp_welcome_message*>(decoded.get())->get_session_id().id(),
                 uint64_t(42));
    }

    // SUBSCRIBE [32, request, options, topic]
    {
        wamp_subscribe_message msg;
        msg.set_request_id(wamp_request_id(100));
        msg.set_options(empty_map.obj());
        msg.set_topic("some.topic");
        auto buf = serializer.serialize(msg);
        check_root_array(buf, 32, 4);
        auto root_handle = msgpack::unpack(buf.data(), buf.size());
        msgpack::object root = root_handle.get();
        CHECK_EQ(root.via.array.ptr[1].as<uint64_t>(), uint64_t(100));
        CHECK_EQ(root.via.array.ptr[3].as<std::string>(), std::string("some.topic"));
    }

    // SUBSCRIBED [33, request, subscription]
    {
        wamp_subscribed_message msg;
        msg.set_request_id(wamp_request_id(100));
        msg.set_subscription_id(wamp_subscription_id(7));
        auto buf = serializer.serialize(msg);
        check_root_array(buf, 33, 3);
        auto root_handle = msgpack::unpack(buf.data(), buf.size());
        msgpack::object root = root_handle.get();
        CHECK_EQ(root.via.array.ptr[2].as<uint64_t>(), uint64_t(7));
    }

    // PUBLISH [16, request, options, topic]
    {
        wamp_publish_message msg;
        msg.set_request_id(wamp_request_id(101));
        msg.set_options(empty_map.obj());
        msg.set_topic("pub.topic");
        auto buf = serializer.serialize(msg);
        check_root_array(buf, 16, 4);
        auto root_handle = msgpack::unpack(buf.data(), buf.size());
        msgpack::object root = root_handle.get();
        CHECK_EQ(root.via.array.ptr[3].as<std::string>(), std::string("pub.topic"));
    }

    // EVENT [36, subscription, publication, details]
    {
        wamp_event_message msg;
        msg.set_subscription_id(wamp_subscription_id(7));
        msg.set_publication_id(wamp_publication_id(9));
        msg.set_details(empty_map.obj());
        auto buf = serializer.serialize(msg);
        check_root_array(buf, 36, 4);
        auto root_handle = msgpack::unpack(buf.data(), buf.size());
        msgpack::object root = root_handle.get();
        CHECK_EQ(root.via.array.ptr[1].as<uint64_t>(), uint64_t(7));
        CHECK_EQ(root.via.array.ptr[2].as<uint64_t>(), uint64_t(9));
    }

    // CALL [48, request, options, procedure]
    {
        wamp_call_message msg;
        msg.set_request_id(wamp_request_id(102));
        msg.set_options(empty_map.obj());
        msg.set_procedure("echo.procedure");
        auto buf = serializer.serialize(msg);
        check_root_array(buf, 48, 4);
        auto root_handle = msgpack::unpack(buf.data(), buf.size());
        msgpack::object root = root_handle.get();
        CHECK_EQ(root.via.array.ptr[3].as<std::string>(), std::string("echo.procedure"));

        std::unique_ptr<wamp_message> decoded(serializer.deserialize(buf.data(), buf.size()));
        CHECK_EQ(static_cast<wamp_call_message*>(decoded.get())->get_procedure(),
                 std::string("echo.procedure"));
        CHECK(static_cast<wamp_call_message*>(decoded.get())->get_request_id().id() == 102);
    }

    // RESULT [50, request, details]
    {
        wamp_result_message msg;
        msg.set_request_id(wamp_request_id(102));
        msg.set_details(empty_map.obj());
        auto buf = serializer.serialize(msg);
        check_root_array(buf, 50, 3);
    }

    // REGISTER [64, request, options, procedure]
    {
        wamp_register_message msg;
        msg.set_request_id(wamp_request_id(103));
        msg.set_options(empty_map.obj());
        msg.set_procedure("reg.procedure");
        auto buf = serializer.serialize(msg);
        check_root_array(buf, 64, 4);
        auto root_handle = msgpack::unpack(buf.data(), buf.size());
        msgpack::object root = root_handle.get();
        CHECK_EQ(root.via.array.ptr[3].as<std::string>(), std::string("reg.procedure"));
    }

    // INVOCATION [68, request, registration, details]
    {
        wamp_invocation_message msg;
        msg.set_request_id(wamp_request_id(104));
        msg.set_registration_id(wamp_registration_id(11));
        msg.set_details(empty_map.obj());
        auto buf = serializer.serialize(msg);
        check_root_array(buf, 68, 4);
        auto root_handle = msgpack::unpack(buf.data(), buf.size());
        msgpack::object root = root_handle.get();
        CHECK_EQ(root.via.array.ptr[2].as<uint64_t>(), uint64_t(11));
    }

    // YIELD [70, request, options]
    {
        wamp_yield_message msg;
        msg.set_request_id(wamp_request_id(104));
        msg.set_options(empty_map.obj());
        auto buf = serializer.serialize(msg);
        check_root_array(buf, 70, 3);
    }

    // Factory + type-code table on the wire.
    {
        const uint64_t codes[] = { 1, 2, 16, 32, 33, 36, 48, 50, 64, 68, 70 };
        for (uint64_t code : codes) {
            std::unique_ptr<wamp_message> msg(
                    wamp_message_factory::create_message(static_cast<wamp_message_type>(code)));
            CHECK(msg != nullptr);
            if (msg) {
                CHECK_EQ(static_cast<uint64_t>(msg->get_type()), code);
            }
        }
        CHECK(wamp_message_factory::create_message(static_cast<wamp_message_type>(999)) == nullptr);
    }

    return test_summary("messages");
}
