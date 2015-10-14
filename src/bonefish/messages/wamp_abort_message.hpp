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

#ifndef BONEFISH_MESSAGES_WAMP_ABORT_MESSAGE_HPP
#define BONEFISH_MESSAGES_WAMP_ABORT_MESSAGE_HPP

#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/messages/wamp_message_type.hpp>
#include <bonefish/messages/wamp_message_defaults.hpp>
#include <bonefish/utility/wamp_uri.hpp>

#include <cassert>
#include <cstddef>
#include <msgpack.hpp>
#include <ostream>
#include <stdexcept>
#include <vector>

namespace bonefish {

//
// [ABORT, Details|dict, Reason|uri]
//
class wamp_abort_message : public wamp_message
{
public:
    wamp_abort_message();
    virtual ~wamp_abort_message() override;

    virtual wamp_message_type get_type() const override;
    virtual std::vector<msgpack::object> marshal() const override;
    virtual void unmarshal(
            const std::vector<msgpack::object>& fields,
            msgpack::zone&& zone) override;

    const msgpack::object& get_details() const;
    std::string get_reason() const;

    void set_details(const msgpack::object& details);
    void set_reason(const std::string& reason);

private:
    msgpack::object m_type;
    msgpack::object m_details;
    msgpack::object m_reason;
    std::vector<msgpack::object> m_fields;

private:
    static const size_t NUM_FIELDS = 3;
};

inline wamp_abort_message::wamp_abort_message()
    : m_type(wamp_message_type::ABORT)
    , m_details(msgpack_empty_map())
    , m_reason()
{
}

inline wamp_abort_message::~wamp_abort_message()
{
}

inline wamp_message_type wamp_abort_message::get_type() const
{
    return m_type.as<wamp_message_type>();
}

inline std::vector<msgpack::object> wamp_abort_message::marshal() const
{
    std::vector<msgpack::object> fields { m_type, m_details, m_reason };
    return fields;
}

inline void wamp_abort_message::unmarshal(
        const std::vector<msgpack::object>& fields,
        msgpack::zone&& zone)
{
    if (fields.size() != NUM_FIELDS) {
        throw std::invalid_argument("invalid number of fields");
    }

    if (fields[0].as<wamp_message_type>() != get_type()) {
        throw std::invalid_argument("invalid message type");
    }

    acquire_zone(std::move(zone));
    m_details = fields[1];
    m_reason = fields[2];
}

inline const msgpack::object& wamp_abort_message::get_details() const
{
    return m_details;
}

inline std::string wamp_abort_message::get_reason() const
{
    return m_reason.as<std::string>();
}

inline void wamp_abort_message::set_details(const msgpack::object& details)
{
    assert(details.type == msgpack::type::MAP);
    m_details = msgpack::object(details, get_zone());
}

inline void wamp_abort_message::set_reason(const std::string& reason)
{
    m_reason = msgpack::object(reason, get_zone());
}

inline std::ostream& operator<<(std::ostream& os, const wamp_abort_message& message)
{
    os << "abort [" << message.get_details() << ", "
            << message.get_reason() << "]";
    return os;
}

} // namespace bonefish

#endif // BONEFISH_MESSAGES_WAMP_ABORT_MESSAGE_HPP
