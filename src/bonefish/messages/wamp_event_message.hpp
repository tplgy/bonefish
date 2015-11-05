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

#ifndef BONEFISH_MESSAGES_WAMP_EVENT_MESSAGE_HPP
#define BONEFISH_MESSAGES_WAMP_EVENT_MESSAGE_HPP

#include <bonefish/identifiers/wamp_subscription_id.hpp>
#include <bonefish/identifiers/wamp_publication_id.hpp>
#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/messages/wamp_message_defaults.hpp>
#include <bonefish/messages/wamp_message_type.hpp>

#include <cstddef>
#include <msgpack.hpp>
#include <ostream>
#include <stdexcept>
#include <vector>

namespace bonefish {

//
// [EVENT, SUBSCRIBED.Subscription|id, PUBLISHED.Publication|id, Details|dict]
// [EVENT, SUBSCRIBED.Subscription|id, PUBLISHED.Publication|id, Details|dict, PUBLISH.Arguments|list]
// [EVENT, SUBSCRIBED.Subscription|id, PUBLISHED.Publication|id, Details|dict, PUBLISH.Arguments|list, PUBLISH.ArgumentsKw|dict]
//
class wamp_event_message : public wamp_message
{
public:
    wamp_event_message();
    wamp_event_message(msgpack::zone&& zone);
    virtual ~wamp_event_message() override;

    virtual wamp_message_type get_type() const override;
    virtual std::vector<msgpack::object> marshal() const override;
    virtual void unmarshal(
            const std::vector<msgpack::object>& fields,
            msgpack::zone&& zone) override;

    wamp_subscription_id get_subscription_id() const;
    wamp_publication_id get_publication_id() const;
    const msgpack::object& get_details() const;
    const msgpack::object& get_arguments() const;
    const msgpack::object& get_arguments_kw() const;

    void set_subscription_id(const wamp_subscription_id& subscription_id);
    void set_publication_id(const wamp_publication_id& publication_id);
    void set_details(const msgpack::object& details);
    void set_arguments(const msgpack::object& arguments);
    void set_arguments_kw(const msgpack::object& arguments_kw);

private:
    msgpack::object m_type;
    msgpack::object m_subscription_id;
    msgpack::object m_publication_id;
    msgpack::object m_details;
    msgpack::object m_arguments;
    msgpack::object m_arguments_kw;

private:
    static const size_t MIN_FIELDS = 4;
    static const size_t MAX_FIELDS = 6;
};

inline wamp_event_message::wamp_event_message()
    : wamp_event_message(msgpack::zone())
{
}

inline wamp_event_message::wamp_event_message(msgpack::zone&& zone)
    : wamp_message(std::move(zone))
    , m_type(wamp_message_type::EVENT)
    , m_subscription_id()
    , m_publication_id()
    , m_details(msgpack_empty_map())
    , m_arguments()
    , m_arguments_kw()
{
}

inline wamp_event_message::~wamp_event_message()
{
}

inline wamp_message_type wamp_event_message::get_type() const
{
    return m_type.as<wamp_message_type>();
}

inline std::vector<msgpack::object> wamp_event_message::marshal() const
{
    std::vector<msgpack::object> fields;

    if (!m_arguments_kw.is_nil()) {
        if (!m_arguments.is_nil()) {
            fields = { m_type, m_subscription_id, m_publication_id, m_details,
                    m_arguments, m_arguments_kw };
        } else {
            fields = { m_type, m_subscription_id, m_publication_id, m_details,
                    msgpack_empty_map(), m_arguments_kw };
        }
    } else if (!m_arguments.is_nil()) {
        fields = { m_type, m_subscription_id, m_publication_id, m_details, m_arguments };
    } else {
        fields = { m_type, m_subscription_id, m_publication_id, m_details };
    }

    return fields;
}

inline void wamp_event_message::unmarshal(
        const std::vector<msgpack::object>& fields,
        msgpack::zone&& zone)
{
    if (fields.size() < MIN_FIELDS || fields.size() > MAX_FIELDS) {
        throw std::invalid_argument("invalid number of fields");
    }

    if (fields[0].as<wamp_message_type>() != get_type()) {
        throw std::invalid_argument("invalid message type");
    }

    acquire_zone(std::move(zone));
    m_subscription_id = fields[1];
    m_publication_id = fields[2];
    m_details = fields[3];
    if (fields.size() >= 5) {
        m_arguments = fields[4];
    }
    if (fields.size() == 6) {
        m_arguments_kw = fields[5];
    }
}

inline wamp_subscription_id wamp_event_message::get_subscription_id() const
{
    return wamp_subscription_id(m_subscription_id.as<uint64_t>());
}

inline wamp_publication_id wamp_event_message::get_publication_id() const
{
    return wamp_publication_id(m_publication_id.as<uint64_t>());
}

inline const msgpack::object& wamp_event_message::get_details() const
{
    return m_details;
}

inline const msgpack::object& wamp_event_message::get_arguments() const
{
    return m_arguments;
}

inline const msgpack::object& wamp_event_message::get_arguments_kw() const
{
    return m_arguments_kw;
}

inline void wamp_event_message::set_subscription_id(const wamp_subscription_id& subscription_id)
{
    m_subscription_id = msgpack::object(subscription_id.id());
}

inline void wamp_event_message::set_publication_id(const wamp_publication_id& publication_id)
{
    m_publication_id = msgpack::object(publication_id.id());
}

inline void wamp_event_message::set_details(const msgpack::object& details)
{
    if (details.type == msgpack::type::MAP) {
        m_details = msgpack::object(details, get_zone());
    } else {
        throw std::invalid_argument("invalid details");
    }
}

inline void wamp_event_message::set_arguments(const msgpack::object& arguments)
{
    if (arguments.type == msgpack::type::NIL || arguments.type == msgpack::type::ARRAY) {
        m_arguments = msgpack::object(arguments, get_zone());
    } else {
        throw std::invalid_argument("invalid arguments");
    }
}

inline void wamp_event_message::set_arguments_kw(const msgpack::object& arguments_kw)
{
    if (arguments_kw.type == msgpack::type::NIL || arguments_kw.type == msgpack::type::MAP) {
        m_arguments_kw = msgpack::object(arguments_kw, get_zone());
    } else {
        throw std::invalid_argument("invalid arguments_kw");
    }
}

inline std::ostream& operator<<(std::ostream& os, const wamp_event_message& message)
{
    os << "event [" << message.get_subscription_id() << ", "
            << message.get_publication_id() << ", "
            << message.get_details() << ", "
            << message.get_arguments() << ", "
            << message.get_arguments_kw() << "]";
    return os;
}

} // namespace bonefish

#endif // BONEFISH_MESSAGES_WAMP_EVENT_MESSAGE_HPP
