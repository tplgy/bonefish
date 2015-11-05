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

#ifndef BONEFISH_NATIVE_ENDPOINT_HPP
#define BONEFISH_NATIVE_ENDPOINT_HPP

// http://stackoverflow.com/questions/22597948/using-boostfuture-with-then-continuations/
#define BOOST_THREAD_PROVIDES_FUTURE
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
#define BOOST_THREAD_PROVIDES_FUTURE_WHEN_ALL_WHEN_ANY

#include <boost/thread/future.hpp>
#include <functional>
#include <msgpack.hpp>
#include <vector>

namespace bonefish {

class native_endpoint;

/*!
 * Defines a convenience type for a future that is fulfilled with a server
 * endpoint.
 */
using native_endpoint_future = boost::future<std::shared_ptr<native_endpoint>>;

/*!
 * Defines a convenience type for a promise that fullfils a server
 * endpoint as its promise.
 */
using native_endpoint_promise = boost::promise<std::shared_ptr<native_endpoint>>;

/*!
 * A class that provides the concept of an endpoint for communicating natively
 * between a component and a server. Communication is message based and conducted
 * in a zero copy manner, as ownership of the message is passed via the endpoint.
 */
class native_endpoint
{
public:
    /*!
     * Defines a handler used by the endpoint for sending messages. The message
     * is represented as a set of fields and the zone used to allocate them.
     * Ownership of the fields is passed off to the recipient of the message.
     */
    using send_message_handler =
            std::function<void(
                    std::vector<msgpack::object>&& fields, msgpack::zone&& zone)>;

public:
    native_endpoint();

    const send_message_handler& get_send_message_handler() const;
    void set_send_message_handler(send_message_handler&& handler);

private:
    send_message_handler m_send_message_handler;
};

inline native_endpoint::native_endpoint()
    : m_send_message_handler()
{
}

inline const native_endpoint::send_message_handler&
native_endpoint::get_send_message_handler() const
{
    return m_send_message_handler;
}

inline void native_endpoint::set_send_message_handler(
        send_message_handler&& handler)
{
    m_send_message_handler = std::move(handler);
}

} // namespace bonefish

#endif // BONEFISH_NATIVE_ENDPOINT_HPP
