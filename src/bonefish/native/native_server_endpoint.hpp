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

#ifndef BONEFISH_NATIVE_SERVER_ENDPOINT_HPP
#define BONEFISH_NATIVE_SERVER_ENDPOINT_HPP

#include <bonefish/native/native_endpoint.hpp>
#include <functional>
#include <memory>

namespace bonefish {

/*!
 * A class that provides the concept of an endpoint for communicating natively
 * with a server. Components initiate connections with the server utilizing the
 * connector provided by the server. Components can also use the connector to
 * disconnect from the server. However, all the server gets access to when
 * connections are created is a component's endpoint. As a result if it wants
 * to initiate a disconnect with a component it can only do so by using the
 * component's endpoint. When a server initiates a disconnect it must synchronize
 * with the component to ensure that any shared state can be safely destroyed.
 * As a result, a server endpoint must provide a handler to be invoked by the
 * component when a disconnect issued by the server has been processed.
 */
class native_server_endpoint :
        public native_endpoint,
        public std::enable_shared_from_this<native_server_endpoint>
{
public:
    /*!
     * Defines a handler to be invoked by the component to indicate when a server
     * initiated disconnect is complete.
     */
    using disconnected_handler = std::function<void()>;

public:
    native_server_endpoint();

    const disconnected_handler& get_disconnected_handler() const;
    void set_disconnected_handler(disconnected_handler&& handler);

private:
    disconnected_handler m_disconnected_handler;
};

inline native_server_endpoint::native_server_endpoint()
    : m_disconnected_handler()
{
}

inline const native_server_endpoint::disconnected_handler&
native_server_endpoint::get_disconnected_handler() const
{
    return m_disconnected_handler;
}

inline void native_server_endpoint::set_disconnected_handler(
        disconnected_handler&& handler)
{
    m_disconnected_handler = handler;
}

} // namespace bonefish

#endif // BONEFISH_NATIVE_SERVER_ENDPOINT_HPP
