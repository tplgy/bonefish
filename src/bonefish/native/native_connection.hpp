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

#ifndef BONEFISH_NATIVE_CONNECTION_HPP
#define BONEFISH_NATIVE_CONNECTION_HPP

#include <bonefish/common/wamp_connection_base.hpp>
#include <bonefish/native/native_component_endpoint.hpp>
#include <bonefish/native/native_message_queue.hpp>
#include <bonefish/native/native_server_endpoint.hpp>
#include <bonefish/trace/trace.hpp>

#include <boost/asio/io_service.hpp>
#include <functional>
#include <memory>
#include <msgpack.hpp>

namespace bonefish {

/*!
 * A class that represents a connection to the server from a native
 * WAMP component. The connection is message based as it provides
 * direct in process communication with the component. Messages that
 * are received from the component are temporarily queued for processing.
 */
class native_connection :
        public wamp_connection_base,
        public std::enable_shared_from_this<native_connection>
{
public:
    /*!
     * Defines a handler for receiving messages. The handler passes along
     * ownership of the message fields and the zone used to allocate them.
     *
     * @param connection The connection the message was received on.
     * @param fields The message fields.
     * @param zone The zone the message fields were allocated from.
     */
    using receive_message_handler =
            std::function<void(
                    const std::shared_ptr<native_connection>& connection,
                    std::vector<msgpack::object>&& fields,
                    msgpack::zone&& zone)>;

    /*!
     * Defines a handler to be notified when the connection has been safely
     * disconnected. This is used by the server endpoint when it initiates
     * a disconnect with the component and receives a disconnected response
     * from the component. Signifying that the connection can now be safely
     * cleaned up.
     */
    using disconnected_handler = std::function<void()>;

public:
    native_connection(
            boost::asio::io_service& io_service,
            const std::shared_ptr<native_component_endpoint>& component_endpoint);

    virtual ~native_connection() override;

    /*!
     * Sends a message to the component represented by a set of fields and
     * the zone used to allocate the fields. Ownership of the fields and
     * zone are passed to the component creating the effect of zero copy.
     *
     * @param fields The message fields.
     * @param zone The zone used to allocate the message fields.
     */
    void send_message(
            std::vector<msgpack::object>&& fields,
            msgpack::zone&& zone);

    /*!
     * Retrieves the server endpoint to be used by the component to send messages
     * to the server using this connection.
     *
     * @return The server endpoint using to send messages to this connection.
     */
    const std::shared_ptr<native_server_endpoint>& get_server_endpoint();

    /*!
     * Retrieves the component endpoint to be used by the server to send messages
     * to the component using this connection.
     *
     * @return The server endpoint using to send messages to this connection.
     */
    const std::shared_ptr<native_component_endpoint>& get_component_endpoint() const;

    const receive_message_handler& get_receive_message_handler() const;
    void set_receive_message_handler(receive_message_handler&& handler);

    const disconnected_handler& get_disconnected_handler() const;
    void set_disconnected_handler(disconnected_handler&& handler);

private:
    /*!
     * This io service is used to drive events for this connection.
     */
    boost::asio::io_service& m_io_service;

    /*!
     * The endpoint representing the server side of this connection. It is
     * shared with the component and provides a mechanism for the component
     * to send messages to the server using this connection.
     */
    std::shared_ptr<native_server_endpoint> m_server_endpoint;

    /*!
     * The endpoint representing the component side of this connection. It is
     * passed to us by the component upon connecting and provides a mechanism
     * for messages to be sent to the component using this connection.
     */
    std::shared_ptr<native_component_endpoint> m_component_endpoint;

    /*!
     * The message queue used for queuing messages sent to this connection
     * by the component. A queue is used rather than binding functors and
     * posting directly to the io service due to complications with binding
     * the message zone with functors as it ends up creating move-only
     * (non-copyable) functors. In addition, we may also want to flow control
     * messages and using a queue allows for this to be added fairly easily.
     */
    native_message_queue m_receive_message_queue;

    receive_message_handler m_receive_message_handler;
    disconnected_handler m_disconnected_handler;
};

inline native_connection::native_connection(
        boost::asio::io_service& io_service,
        const std::shared_ptr<native_component_endpoint>& component_endpoint)
    : wamp_connection_base()
    , m_io_service(io_service)
    , m_server_endpoint()
    , m_component_endpoint(component_endpoint)
    , m_receive_message_handler()
    , m_disconnected_handler()
{
}

inline native_connection::~native_connection()
{
}

inline void native_connection::send_message(
        std::vector<msgpack::object>&& fields,
        msgpack::zone&& zone)
{
    auto send_message_handler = m_component_endpoint->get_send_message_handler();
    send_message_handler(std::move(fields), std::move(zone));
}

inline const std::shared_ptr<native_component_endpoint>&
native_connection::get_component_endpoint() const
{
    return m_component_endpoint;
}

inline const native_connection::receive_message_handler&
native_connection::get_receive_message_handler() const
{
    return m_receive_message_handler;
}

inline void native_connection::set_receive_message_handler(
        native_connection::receive_message_handler&& handler)
{
    m_receive_message_handler = std::move(handler);
}

inline const native_connection::disconnected_handler&
native_connection::get_disconnected_handler() const
{
    return m_disconnected_handler;
}

inline void native_connection::set_disconnected_handler(
        native_connection::disconnected_handler&& handler)
{
    m_disconnected_handler = std::move(handler);
}

} // namespace bonefish

#endif // BONEFISH_NATIVE_CONNECTION_HPP
