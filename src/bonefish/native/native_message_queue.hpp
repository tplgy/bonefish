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

#ifndef BONEFISH_NATIVE_MESSAGE_QUEUE_HPP
#define BONEFISH_NATIVE_MESSAGE_QUEUE_HPP

#include <cstddef>
#include <deque>
#include <msgpack.hpp>
#include <mutex>
#include <stdexcept>
#include <vector>

namespace bonefish {

/*!
 * A class that provides a queuing mechanism for native messages. Currently
 * this is just implemented with a simple deque and a mutex. However, for
 * better performance a lockless ring buffer can be used since there is only
 * a single producer and single consumer. This would offer an obvious performance
 * improvement for components with very high messaging rates. It would also then
 * require a flow control mechanism to deal with cases where the ring buffer
 * filled to capacity.
 */
class native_message_queue
{
public:
    native_message_queue();

    native_message_queue(const native_message_queue&) = delete;
    native_message_queue(native_message_queue&&) = delete;

    native_message_queue& operator=(const native_message_queue&) = delete;
    native_message_queue& operator=(native_message_queue&&) = delete;

    /*!
     * Pushes a new message onto the queue.
     */
    void push_message(
            std::vector<msgpack::object>&& fields,
            msgpack::zone&& zone);

    /*!
     * Pops the next message off of the queue.
     */
    void pop_message(
            std::vector<msgpack::object>& fields,
            msgpack::zone& zone);

    /*!
     * Retrieves the size of the underlying message queue.
     *
     * @return The size of the underlying message queue.
     */
    std::size_t size() const;

private:
    /*!
     * Defines a convenience type for a queued message entry.
     */
    using message = std::pair<std::vector<msgpack::object>, msgpack::zone>;

private:
    /*!
     * Protection for access to the queue of messages.
     */
    std::mutex m_mutex;

    /*!
     * The queue of messages.
     */
    std::deque<message> m_messages;
};

inline native_message_queue::native_message_queue()
    : m_mutex()
    , m_messages()
{
}

inline void native_message_queue::push_message(
        std::vector<msgpack::object>&& fields,
        msgpack::zone&& zone)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_messages.push_back(
            message(std::move(fields), std::move(zone)));
}

inline void native_message_queue::pop_message(
        std::vector<msgpack::object>& fields,
        msgpack::zone& zone)
{
    if (m_messages.size() == 0) {
        throw std::underflow_error("message queue underflow");
    }

    std::lock_guard<std::mutex> lock(m_mutex);
    auto& m = m_messages.front();
    fields = std::move(m.first);
    zone = std::move(m.second);
    m_messages.pop_front();
}

inline std::size_t native_message_queue::size() const
{
    return m_messages.size();
}

} // namespace bonefish

#endif // BONEFISH_NATIVE_MESSAGE_QUEUE_HPP
