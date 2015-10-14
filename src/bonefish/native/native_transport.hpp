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

#ifndef BONEFISH_NATIVE_TRANSPORT_HPP
#define BONEFISH_NATIVE_TRANSPORT_HPP

#include <bonefish/transport/wamp_transport.hpp>

#include <memory>

namespace bonefish {

class native_connection;
class wamp_message;

/*!
 * A class that provides a transport that can be used to send messages
 * over an underlying connection.
 */
class native_transport : public wamp_transport
{
public:
    native_transport(const std::shared_ptr<native_connection>& connection);

    /*!
     * Sends a message over the transports connection.
     *
     * @param message The WAMP message to send.
     */
    virtual bool send_message(wamp_message&& message) override;

private:
    /*!
     * The underlying connection for sending messages.
     */
    std::shared_ptr<native_connection> m_connection;
};

} // namespace bonefish

#endif // BONEFISH_NATIVE_TRANSPORT_HPP
