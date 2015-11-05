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

#ifndef BONEFISH_MESSAGES_WAMP_RESULT_MESSAGE_HPP
#define BONEFISH_MESSAGES_WAMP_RESULT_MESSAGE_HPP

#include <bonefish/identifiers/wamp_request_id.hpp>
#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/messages/wamp_message_defaults.hpp>
#include <bonefish/messages/wamp_message_type.hpp>

#include <cstddef>
#include <msgpack.hpp>
#include <stdexcept>
#include <ostream>
#include <vector>

namespace bonefish {

//
// [RESULT, CALL.Request|id, Details|dict]
// [RESULT, CALL.Request|id, Details|dict, YIELD.Arguments|list]
// [RESULT, CALL.Request|id, Details|dict, YIELD.Arguments|list, YIELD.ArgumentsKw|dict]
//
class wamp_result_message : public wamp_message
{
public:
    wamp_result_message();
    wamp_result_message(msgpack::zone&& zone);
    virtual ~wamp_result_message() override;

    virtual wamp_message_type get_type() const override;
    virtual std::vector<msgpack::object> marshal() const override;
    virtual void unmarshal(
            const std::vector<msgpack::object>& fields,
            msgpack::zone&& zone) override;

    wamp_request_id get_request_id() const;
    const msgpack::object& get_details() const;
    const msgpack::object& get_arguments() const;
    const msgpack::object& get_arguments_kw() const;

    void set_request_id(const wamp_request_id& request_id);
    void set_details(const msgpack::object& details);
    void set_arguments(const msgpack::object& arguments);
    void set_arguments_kw(const msgpack::object& arguments_kw);

private:
    msgpack::object m_type;
    msgpack::object m_request_id;
    msgpack::object m_details;
    msgpack::object m_arguments;
    msgpack::object m_arguments_kw;

private:
    static const size_t MIN_FIELDS = 3;
    static const size_t MAX_FIELDS = 5;
};

inline wamp_result_message::wamp_result_message()
    : wamp_result_message(msgpack::zone())
{
}

inline wamp_result_message::wamp_result_message(msgpack::zone&& zone)
    : wamp_message(std::move(zone))
    , m_type(wamp_message_type::RESULT)
    , m_request_id()
    , m_details(msgpack_empty_map())
    , m_arguments()
    , m_arguments_kw()
{
}

inline wamp_result_message::~wamp_result_message()
{
}

inline wamp_message_type wamp_result_message::get_type() const
{
    return m_type.as<wamp_message_type>();
}

inline std::vector<msgpack::object> wamp_result_message::marshal() const
{
    std::vector<msgpack::object> fields;

    if (!m_arguments_kw.is_nil()) {
        if (!m_arguments.is_nil()) {
            fields = { m_type, m_request_id, m_details, m_arguments, m_arguments_kw };
        } else {
            fields = { m_type, m_request_id, m_details, msgpack_empty_map(), m_arguments_kw };
        }
    } else if (!m_arguments.is_nil()) {
        fields = { m_type, m_request_id, m_details, m_arguments };
    } else {
        fields = { m_type, m_request_id, m_details };
    }

    return fields;
}

inline void wamp_result_message::unmarshal(
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
    m_request_id = fields[1];
    m_details = fields[2];
    if (fields.size() >= 4) {
        m_arguments = fields[3];
    }
    if (fields.size() == 5) {
        m_arguments_kw = fields[4];
    }
}

inline wamp_request_id wamp_result_message::get_request_id() const
{
    return wamp_request_id(m_request_id.as<uint64_t>());
}

inline const msgpack::object& wamp_result_message::get_details() const
{
    return m_details;
}

inline const msgpack::object& wamp_result_message::get_arguments() const
{
    return m_arguments;
}

inline const msgpack::object& wamp_result_message::get_arguments_kw() const
{
    return m_arguments_kw;
}

inline void wamp_result_message::set_request_id(const wamp_request_id& request_id)
{
    m_request_id = msgpack::object(request_id.id());
}

inline void wamp_result_message::set_details(const msgpack::object& details)
{
    if (details.type != msgpack::type::MAP) {
        m_details = msgpack::object(details, get_zone());
    } else {
        throw std::invalid_argument("invalid details");
    }
}

inline void wamp_result_message::set_arguments(const msgpack::object& arguments)
{
    if (arguments.type == msgpack::type::NIL || arguments.type == msgpack::type::ARRAY) {
        m_arguments = msgpack::object(arguments, get_zone());
    } else {
        throw std::invalid_argument("invalid arguments");
    }
}

inline void wamp_result_message::set_arguments_kw(const msgpack::object& arguments_kw)
{
    if (arguments_kw.type == msgpack::type::NIL || arguments_kw.type == msgpack::type::MAP) {
        m_arguments_kw = msgpack::object(arguments_kw, get_zone());
    } else {
        throw std::invalid_argument("invalid arguments_kw");
    }
}

inline std::ostream& operator<<(std::ostream& os, const wamp_result_message& message)
{
    os << "result [" << message.get_request_id() << ", "
            << message.get_details() << ", "
            << message.get_arguments() << ", "
            << message.get_arguments_kw() << "]";
    return os;
}

} // namespace bonefish

#endif // BONEFISH_MESSAGES_WAMP_RESULT_MESSAGE_HPP
