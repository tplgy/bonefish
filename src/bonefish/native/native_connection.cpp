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

#include <bonefish/native/native_connection.hpp>

namespace bonefish {

const std::shared_ptr<native_endpoint>& native_connection::get_server_endpoint()
{
    if (!m_server_endpoint) {
        m_server_endpoint = std::make_shared<native_endpoint>();
        std::weak_ptr<native_connection> weak_this = this->shared_from_this();

        m_server_endpoint->set_send_message_handler(
            [this, weak_this](std::vector<msgpack::object>&& fields,
                    msgpack::zone&& zone) {
                auto shared_this = weak_this.lock();
                if (!shared_this) {
                    // This will get thrown in the context of the component.
                    throw std::runtime_error("connection closed");
                }

                m_receive_message_queue.push_message(
                        std::move(fields), std::move(zone));

                m_io_service.post([this, weak_this]() {
                    auto shared_this = weak_this.lock();
                    if (!shared_this) {
                        // FIXME: This will cause the io service to bail!!
                        throw std::runtime_error("connection closed");
                    }

                    msgpack::zone zone;
                    std::vector<msgpack::object> fields;
                    m_receive_message_queue.pop_message(fields, zone);
                    m_receive_message_handler(
                            shared_this, std::move(fields), std::move(zone));
                });
            }
        );
    }

    return m_server_endpoint;
}

} // namespace bonefish
