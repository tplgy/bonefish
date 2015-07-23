/**
 *  Copyright (C) 2015 Topology LP
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include <bonefish/serialization/msgpack_serializer.hpp>
#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/messages/wamp_message_factory.hpp>

#include <iostream>
#include <msgpack.hpp>
#include <sstream>
#include <stdexcept>

namespace msgpack {
MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS) {

bool reference_func(type::object_type type, std::size_t length, void* user_data) {
    // Never copy any STR, BIN or EXT out of the client buffer on parsing,
    // since we create a deep copy of the unpacked item anyway.
    return false;
}

} // namespace <current msgpack version>
} // namespace msgpack

namespace bonefish {

wamp_message* msgpack_serializer::deserialize(const char* buffer, size_t length) const
{
    msgpack::unpacked item = msgpack::unpack(buffer, length, msgpack::reference_func);

    std::vector<msgpack::object> fields;
    item.get().convert(&fields);

    if (fields.size() < 1) {
        throw std::runtime_error("deserialization failed for message");
    }

    wamp_message_type type = static_cast<wamp_message_type>(fields[0].as<unsigned>());
    std::unique_ptr<wamp_message> message(wamp_message_factory::create_message(type));
    if (message) {
        message->unmarshal(fields);
    } else {
        throw std::runtime_error("no deserializer defined for message");
    }

    return message.release();
}

expandable_buffer msgpack_serializer::serialize(const wamp_message* message) const
{
    expandable_buffer buffer(10*1024);
    msgpack::packer<expandable_buffer> packer(buffer);
    packer.pack(message->marshal());

    return buffer;
}

} // namespace bonefish
