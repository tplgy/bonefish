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

#include <bonefish/rawsocket/rawsocket_transport.hpp>
#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/messages/wamp_message_type.hpp>
#include <bonefish/rawsocket/rawsocket_connection.hpp>
#include <bonefish/serialization/expandable_buffer.hpp>
#include <bonefish/serialization/wamp_serializer.hpp>
#include <bonefish/trace/trace.hpp>

#include <iostream>

namespace bonefish {

rawsocket_transport::rawsocket_transport(
        const std::shared_ptr<wamp_serializer>& serializer,
        const std::shared_ptr<rawsocket_connection>& connection)
    : m_serializer(serializer)
    , m_connection(connection)
{
}

bool rawsocket_transport::send_message(wamp_message&& message)
{
    BONEFISH_TRACE("sending message: %1%", message_type_to_string(message.get_type()));
    expandable_buffer buffer = m_serializer->serialize(message);
    return m_connection->send_message(buffer.data(), buffer.size());
}

} // namespace bonefish
