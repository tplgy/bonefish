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

#ifndef BONEFISH_MESSAGES_WAMP_MESSAGE_HPP
#define BONEFISH_MESSAGES_WAMP_MESSAGE_HPP

#include <bonefish/messages/wamp_message_type.hpp>

#include <msgpack.hpp>
#include <vector>

namespace bonefish {

class wamp_message
{
public:
    wamp_message();
    wamp_message(msgpack::zone&& zone);
    virtual ~wamp_message() = default;

    wamp_message(const wamp_message&) = delete;
    wamp_message(wamp_message&&) = delete;
    wamp_message& operator=(wamp_message const&) = delete;
    wamp_message& operator=(wamp_message&&) = delete;

    msgpack::zone release_zone();
    msgpack::zone& get_zone();

    virtual wamp_message_type get_type() const = 0;
    virtual std::vector<msgpack::object> marshal() const = 0;
    virtual void unmarshal(
            const std::vector<msgpack::object>& fields,
            msgpack::zone&& zone) = 0;

protected:
    void acquire_zone(msgpack::zone&& zone);

private:
    msgpack::zone m_zone;
};

inline wamp_message::wamp_message()
    : m_zone()
{
}

inline wamp_message::wamp_message(msgpack::zone&& zone)
    : m_zone(std::move(zone))
{
}

inline msgpack::zone wamp_message::release_zone()
{
    return std::move(m_zone);
}

inline msgpack::zone& wamp_message::get_zone()
{
    return m_zone;
}

inline void wamp_message::acquire_zone(msgpack::zone&& zone)
{
    m_zone = std::move(zone);
}

} // namespace bonefish

#endif // BONEFISH_MESSAGES_WAMP_MESSAGE_HPP
