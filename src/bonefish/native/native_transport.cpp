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

#include <bonefish/native/native_transport.hpp>
#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/messages/wamp_message_type.hpp>
#include <bonefish/native/native_connection.hpp>
#include <bonefish/trace/trace.hpp>

#include <iostream>

namespace bonefish {

native_transport::native_transport(
        const std::shared_ptr<native_connection>& connection)
    : m_connection(connection)
{
}

bool native_transport::send_message(wamp_message&& message)
{
    BONEFISH_TRACE("sending message: %1%", message_type_to_string(message.get_type()));
    m_connection->send_message(
            std::move(message.marshal()),
            std::move(message.release_zone()));

    return true;
}

} // namespace bonefish
