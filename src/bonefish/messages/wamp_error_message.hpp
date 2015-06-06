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

#ifndef BONEFISH_MESSAGES_WAMP_ERROR_MESSAGE_HPP
#define BONEFISH_MESSAGES_WAMP_ERROR_MESSAGE_HPP

#include <bonefish/identifiers/wamp_request_id.hpp>
#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/messages/wamp_message_defaults.hpp>
#include <bonefish/messages/wamp_message_type.hpp>
#include <bonefish/utility/wamp_uri.hpp>

#include <cstddef>
#include <msgpack.hpp>
#include <ostream>
#include <stdexcept>
#include <vector>

namespace bonefish {

//
// [ERROR, REQUEST.Type|int, REQUEST.Request|id, Details|dict, Error|uri]
// [ERROR, REQUEST.Type|int, REQUEST.Request|id, Details|dict, Error|uri, Arguments|list]
// [ERROR, REQUEST.Type|int, REQUEST.Request|id, Details|dict, Error|uri, Arguments|list, ArgumentsKw|dict]
//
class wamp_error_message : public wamp_message
{
public:
    wamp_error_message();
    virtual ~wamp_error_message() override;

    virtual wamp_message_type get_type() const override;
    virtual std::vector<msgpack::object> marshal() const override;
    virtual void unmarshal(const std::vector<msgpack::object>& fields) override;

    wamp_message_type get_request_type() const;
    wamp_request_id get_request_id() const;
    const msgpack::object& get_details() const;
    std::string get_error() const;
    const msgpack::object& get_arguments() const;
    const msgpack::object& get_arguments_kw() const;

    void set_request_type(const wamp_message_type& request_type);
    void set_request_id(const wamp_request_id& request_id);
    void set_details(const msgpack::object& details);
    void set_error(const std::string& error);
    void set_arguments(const msgpack::object& arguments);
    void set_arguments_kw(const msgpack::object& arguments_kw);

private:
    msgpack::zone m_zone;
    msgpack::object m_type;
    msgpack::object m_request_type;
    msgpack::object m_request_id;
    msgpack::object m_details;
    msgpack::object m_error;
    msgpack::object m_arguments;
    msgpack::object m_arguments_kw;

private:
    static const size_t MIN_FIELDS = 5;
    static const size_t MAX_FIELDS = 7;
};

inline wamp_error_message::wamp_error_message()
    : m_zone()
    , m_type(wamp_message_type::ERROR)
    , m_request_type()
    , m_request_id()
    , m_details(msgpack_empty_map())
    , m_error()
    , m_arguments()
    , m_arguments_kw()
{
}

inline wamp_error_message::~wamp_error_message()
{
}

inline wamp_message_type wamp_error_message::get_type() const
{
    return m_type.as<wamp_message_type>();
}

inline std::vector<msgpack::object> wamp_error_message::marshal() const
{
    std::vector<msgpack::object> fields;

    if (!m_arguments_kw.is_nil()) {
        if (!m_arguments.is_nil()) {
            fields = { m_type, m_request_type, m_request_id, m_details,
                    m_error, m_arguments, m_arguments_kw };
        } else {
            fields = { m_type, m_request_type, m_request_id, m_details,
                m_error, msgpack_empty_map(), m_arguments_kw };
        }
    } else if (!m_arguments.is_nil()) {
        fields = { m_type, m_request_type, m_request_id, m_details, m_error, m_arguments };
    } else {
        fields = { m_type, m_request_type, m_request_id, m_details, m_error };
    }

    return fields;
}

inline void wamp_error_message::unmarshal(const std::vector<msgpack::object>& fields)
{
    if (fields.size() < MIN_FIELDS || fields.size() > MAX_FIELDS) {
        throw std::invalid_argument("invalid number of fields");
    }

    if (fields[0].as<wamp_message_type>() != get_type()) {
        throw std::invalid_argument("invalid message type");
    }

    m_request_type = msgpack::object(fields[1]);
    m_request_id = msgpack::object(fields[2]);
    m_details = msgpack::object(fields[3], &m_zone);
    m_error = msgpack::object(fields[4], &m_zone);
    if (fields.size() >= 6) {
        m_arguments = msgpack::object(fields[5], &m_zone);
    }
    if (fields.size() == 7) {
        m_arguments_kw = msgpack::object(fields[6], &m_zone);
    }
}

inline wamp_message_type wamp_error_message::get_request_type() const
{
    return m_request_type.as<wamp_message_type>();
}

inline wamp_request_id wamp_error_message::get_request_id() const
{
    return wamp_request_id(m_request_id.as<uint64_t>());
}

inline const msgpack::object& wamp_error_message::get_details() const
{
    return m_details;
}

inline std::string wamp_error_message::get_error() const
{
    return m_error.as<std::string>();
}

inline const msgpack::object& wamp_error_message::get_arguments() const
{
    return m_arguments;
}

inline const msgpack::object& wamp_error_message::get_arguments_kw() const
{
    return m_arguments_kw;
}

inline void wamp_error_message::set_request_type(const wamp_message_type& request_type)
{
    m_request_type = msgpack::object(request_type, &m_zone);
}

inline void wamp_error_message::set_request_id(const wamp_request_id& request_id)
{
    m_request_id = msgpack::object(request_id.id());
}

inline void wamp_error_message::set_details(const msgpack::object& details)
{
    if (details.type == msgpack::type::MAP) {
        m_details = msgpack::object(details, &m_zone);
    } else {
        throw std::invalid_argument("invalid details");
    }
}

inline void wamp_error_message::set_error(const std::string& error)
{
    m_error = msgpack::object(error, &m_zone);
}

inline void wamp_error_message::set_arguments(const msgpack::object& arguments)
{
    if (arguments.type == msgpack::type::NIL || arguments.type == msgpack::type::ARRAY) {
        m_arguments = msgpack::object(arguments, &m_zone);
    } else {
        throw std::invalid_argument("invalid arguments");
    }
}

inline void wamp_error_message::set_arguments_kw(const msgpack::object& arguments_kw)
{
    if (arguments_kw.type == msgpack::type::NIL || arguments_kw.type == msgpack::type::MAP) {
        m_arguments_kw = msgpack::object(arguments_kw, &m_zone);
    } else {
        throw std::invalid_argument("invalid arguments_kw");
    }
}

inline std::ostream& operator<<(std::ostream& os, const wamp_error_message& message)
{
    os << "error [" << message.get_request_type() << ", "
            << message.get_request_id() << ", "
            << message.get_details() << ", "
            << message.get_error() << ", "
            << message.get_arguments() << ", "
            << message.get_arguments_kw() << "]";
    return os;
}

} // namespace bonefish

#endif // BONEFISH_WAMP_MESSAGES_ERROR_MESSAGE_HPP
