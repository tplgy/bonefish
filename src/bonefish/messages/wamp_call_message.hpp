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

#ifndef BONEFISH_MESSAGES_WAMP_CALL_MESSAGE_HPP
#define BONEFISH_MESSAGES_WAMP_CALL_MESSAGE_HPP

#include <bonefish/identifiers/wamp_request_id.hpp>
#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/messages/wamp_message_type.hpp>
#include <bonefish/messages/wamp_message_defaults.hpp>
#include <bonefish/utility/wamp_uri.hpp>

#include <cstddef>
#include <msgpack.hpp>
#include <ostream>
#include <stdexcept>
#include <vector>

namespace bonefish {

//
// [CALL, Request|id, Options|dict, Procedure|uri]
// [CALL, Request|id, Options|dict, Procedure|uri, Arguments|list]
// [CALL, Request|id, Options|dict, Procedure|uri, Arguments|list, ArgumentsKw|dict]
//
class wamp_call_message : public wamp_message
{
public:
    wamp_call_message();
    wamp_call_message(msgpack::zone&& zone);
    virtual ~wamp_call_message() override;

    virtual wamp_message_type get_type() const override;
    virtual std::vector<msgpack::object> marshal() const override;
    virtual void unmarshal(
            const std::vector<msgpack::object>& fields,
            msgpack::zone&& zone) override;

    wamp_request_id get_request_id() const;
    const msgpack::object& get_options() const;
    std::string get_procedure() const;
    const msgpack::object& get_arguments() const;
    const msgpack::object& get_arguments_kw() const;

    void set_request_id(const wamp_request_id& request_id);
    void set_options(const msgpack::object& options);
    void set_procedure(const std::string& procedure);
    void set_arguments(const msgpack::object& arguments);
    void set_arguments_kw(const msgpack::object& arguments_kw);

private:
    msgpack::object m_type;
    msgpack::object m_request_id;
    msgpack::object m_options;
    msgpack::object m_procedure;
    msgpack::object m_arguments;
    msgpack::object m_arguments_kw;

private:
    static const size_t MIN_FIELDS = 4;
    static const size_t MAX_FIELDS = 6;
};

inline wamp_call_message::wamp_call_message()
    : wamp_call_message(msgpack::zone())
{
}

inline wamp_call_message::wamp_call_message(msgpack::zone&& zone)
    : wamp_message(std::move(zone))
    , m_type(wamp_message_type::CALL)
    , m_request_id()
    , m_options(msgpack_empty_map())
    , m_procedure()
    , m_arguments()
    , m_arguments_kw()
{
}

inline wamp_call_message::~wamp_call_message()
{
}

inline wamp_message_type wamp_call_message::get_type() const
{
    return m_type.as<wamp_message_type>();
}

inline std::vector<msgpack::object> wamp_call_message::marshal() const
{
    std::vector<msgpack::object> fields;

    if (!m_arguments_kw.is_nil()) {
        if (!m_arguments.is_nil()) {
            fields = { m_type, m_request_id, m_options, m_procedure,
                    m_arguments, m_arguments_kw };
        } else {
            fields = { m_type, m_request_id, m_options, m_procedure,
                    msgpack_empty_map(), m_arguments_kw };
        }
    } else if (!m_arguments.is_nil()) {
        fields = { m_type, m_request_id, m_options, m_procedure, m_arguments };
    } else {
        fields = { m_type, m_request_id, m_options, m_procedure };
    }

    return fields;
}

inline void wamp_call_message::unmarshal(
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
    m_options = fields[2];
    m_procedure = fields[3];
    if (fields.size() >= 5) {
        m_arguments = fields[4];
    }
    if (fields.size() == 6) {
        m_arguments_kw = fields[5];
    }
}

inline wamp_request_id wamp_call_message::get_request_id() const
{
    return wamp_request_id(m_request_id.as<uint64_t>());
}

inline const msgpack::object& wamp_call_message::get_options() const
{
    return m_options;
}

inline std::string wamp_call_message::get_procedure() const
{
    return m_procedure.as<std::string>();
}

inline const msgpack::object& wamp_call_message::get_arguments() const
{
    return m_arguments;
}

inline const msgpack::object& wamp_call_message::get_arguments_kw() const
{
    return m_arguments_kw;
}

inline void wamp_call_message::set_request_id(const wamp_request_id& request_id)
{
    m_request_id = msgpack::object(request_id.id());
}

inline void wamp_call_message::set_options(const msgpack::object& options)
{
    if (options.type == msgpack::type::MAP) {
        m_options = msgpack::object(options, get_zone());
    } else {
        throw std::invalid_argument("invalid options");
    }
}

inline void wamp_call_message::set_procedure(const std::string& procedure)
{
    m_procedure = procedure;
}

inline void wamp_call_message::set_arguments(const msgpack::object& arguments)
{
    if (arguments.type == msgpack::type::NIL || arguments.type == msgpack::type::ARRAY) {
        m_arguments = msgpack::object(arguments, get_zone());
    } else {
        throw std::invalid_argument("invalid arguments");
    }
}

inline void wamp_call_message::set_arguments_kw(const msgpack::object& arguments_kw)
{
    if (arguments_kw.type == msgpack::type::NIL || arguments_kw.type == msgpack::type::MAP) {
        m_arguments_kw = msgpack::object(arguments_kw, get_zone());
    } else {
        throw std::invalid_argument("invalid arguments_kw");
    }
}

inline std::ostream& operator<<(std::ostream& os, const wamp_call_message& message)
{
    os << "call [" << message.get_request_id() << ", "
            << message.get_options() << ", "
            << message.get_procedure() << ", "
            << message.get_arguments() << ", "
            << message.get_arguments_kw() << "]";
    return os;
}

} // namespace bonefish

#endif // BONEFISH_MESSAGES_WAMP_CALL_MESSAGE_HPP
