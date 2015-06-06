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

#ifndef BONEFISH_RAWSOCKET_TRANSPORT_HPP
#define BONEFISH_RAWSOCKET_TRANSPORT_HPP

#include <bonefish/transport/wamp_transport.hpp>

#include <iostream>
#include <memory>

namespace bonefish {

class rawsocket_connection;
class wamp_message;
class wamp_serializer;

class rawsocket_transport : public wamp_transport
{
public:
    rawsocket_transport(
            const std::shared_ptr<wamp_serializer>& serializer,
            const std::shared_ptr<rawsocket_connection>& connection);

    virtual bool send_message(const wamp_message* message) override;

private:
    std::shared_ptr<wamp_serializer> m_serializer;
    std::shared_ptr<rawsocket_connection> m_connection;
};

} // namespace bonefish

#endif // BONEFISH_RAWSOCKET_TRANSPORT_HPP
