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

#ifndef BONEFISH_MESSAGES_WAMP_SUBSCRIBED_MESSAGE_HPP
#define BONEFISH_MESSAGES_WAMP_SUBSCRIBED_MESSAGE_HPP

#include <bonefish/identifiers/wamp_request_id.hpp>
#include <bonefish/identifiers/wamp_subscription_id.hpp>
#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/messages/wamp_message_type.hpp>

#include <cstddef>
#include <msgpack.hpp>
#include <ostream>
#include <stdexcept>
#include <vector>

namespace bonefish {

//
// [SUBSCRIBED, SUBSCRIBE.Request|id, Subscription|id]
//
class wamp_subscribed_message : public wamp_message
{
public:
    wamp_subscribed_message();
    virtual ~wamp_subscribed_message() override;

    virtual wamp_message_type get_type() const override;
    virtual std::vector<msgpack::object> marshal() const override;
    virtual void unmarshal(const std::vector<msgpack::object>& fields) override;

    wamp_request_id get_request_id() const;
    wamp_subscription_id get_subscription_id() const;

    void set_request_id(const wamp_request_id& request_id);
    void set_subscription_id(const wamp_subscription_id& subscription_id);

private:
    msgpack::zone m_zone;
    msgpack::object m_type;
    msgpack::object m_request_id;
    msgpack::object m_subscription_id;

private:
    static const size_t NUM_FIELDS = 3;
};

inline wamp_subscribed_message::wamp_subscribed_message()
    : m_zone()
    , m_type(wamp_message_type::SUBSCRIBED)
    , m_request_id()
    , m_subscription_id()
{
}

inline wamp_subscribed_message::~wamp_subscribed_message()
{
}

inline wamp_message_type wamp_subscribed_message::get_type() const
{
    return m_type.as<wamp_message_type>();
}

inline std::vector<msgpack::object> wamp_subscribed_message::marshal() const
{
    std::vector<msgpack::object> fields { m_type, m_request_id, m_subscription_id };
    return fields;
}

inline void wamp_subscribed_message::unmarshal(const std::vector<msgpack::object>& fields)
{
    if (fields.size() != NUM_FIELDS) {
        throw std::invalid_argument("invalid number of fields");
    }

    if (fields[0].as<wamp_message_type>() != get_type()) {
        throw std::invalid_argument("invalid message type");
    }

    m_request_id = msgpack::object(fields[1]);
    m_subscription_id = msgpack::object(fields[2]);
}

inline wamp_request_id wamp_subscribed_message::get_request_id() const
{
    return wamp_request_id(m_request_id.as<uint64_t>());
}

inline wamp_subscription_id wamp_subscribed_message::get_subscription_id() const
{
    return wamp_subscription_id(m_subscription_id.as<uint64_t>());
}

inline void wamp_subscribed_message::set_request_id(const wamp_request_id& request_id)
{
    m_request_id = msgpack::object(request_id.id());
}

inline void wamp_subscribed_message::set_subscription_id(const wamp_subscription_id& subscription_id)
{
    m_subscription_id = msgpack::object(subscription_id.id());
}

inline std::ostream& operator<<(std::ostream& os, const wamp_subscribed_message& message)
{
    os << "subscribed [" << message.get_request_id() << ", "
            << message.get_subscription_id() << "]";
    return os;
}

} // namespace bonefish

#endif // BONEFISH_MESSAGES_WAMP_SUBSCRIBED_MESSAGE_HPP
