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

#ifndef BONEFISH_MESSAGES_WAMP_REGISTER_MESSAGE_HPP
#define BONEFISH_MESSAGES_WAMP_REGISTER_MESSAGE_HPP

#include <bonefish/identifiers/wamp_request_id.hpp>
#include <bonefish/messages/wamp_message.hpp>
#include <bonefish/messages/wamp_message_defaults.hpp>
#include <bonefish/messages/wamp_message_type.hpp>
#include <bonefish/utility/wamp_uri.hpp>

#include <cassert>
#include <cstddef>
#include <msgpack.hpp>
#include <ostream>
#include <stdexcept>
#include <vector>

namespace bonefish {

//
// [REGISTER, Request|id, Options|dict, Procedure|uri]
//
class wamp_register_message : public wamp_message
{
public:
    wamp_register_message();
    virtual ~wamp_register_message() override;

    virtual wamp_message_type get_type() const override;
    virtual std::vector<msgpack::object> marshal() const override;
    virtual void unmarshal(const std::vector<msgpack::object>& fields) override;

    wamp_request_id get_request_id() const;
    const msgpack::object& get_options() const;
    std::string get_procedure() const;

    void set_request_id(const wamp_request_id& request_id);
    void set_options(const msgpack::object& options);
    void set_procedure(const std::string& procedure);

private:
    msgpack::zone m_zone;
    msgpack::object m_type;
    msgpack::object m_request_id;
    msgpack::object m_options;
    msgpack::object m_procedure;

private:
    static const size_t NUM_FIELDS = 4;
};

inline wamp_register_message::wamp_register_message()
    : m_zone()
    , m_type(wamp_message_type::REGISTER)
    , m_request_id()
    , m_options(msgpack_empty_map())
    , m_procedure()
{
}

inline wamp_register_message::~wamp_register_message()
{
}

inline wamp_message_type wamp_register_message::get_type() const
{
    return m_type.as<wamp_message_type>();
}

inline std::vector<msgpack::object> wamp_register_message::marshal() const
{
    std::vector<msgpack::object> fields { m_type, m_request_id, m_options };
    return fields;
}

inline void wamp_register_message::unmarshal(const std::vector<msgpack::object>& fields)
{
    if (fields.size() != NUM_FIELDS) {
        throw std::invalid_argument("invalid number of fields");
    }

    if (fields[0].as<wamp_message_type>() != get_type()) {
        throw std::invalid_argument("invalid message type");
    }

    m_request_id = msgpack::object(fields[1]);
    m_options = msgpack::object(fields[2], &m_zone);
    m_procedure = msgpack::object(fields[3], &m_zone);
}

inline wamp_request_id wamp_register_message::get_request_id() const
{
    return wamp_request_id(m_request_id.as<uint64_t>());
}

inline const msgpack::object& wamp_register_message::get_options() const
{
    return m_options;
}

inline std::string wamp_register_message::get_procedure() const
{
    return m_procedure.as<std::string>();
}

inline void wamp_register_message::set_request_id(const wamp_request_id& request_id)
{
    m_request_id = msgpack::object(request_id.id());
}

inline void wamp_register_message::set_options(const msgpack::object& options)
{
    assert(options.type == msgpack::type::MAP);
    m_options = msgpack::object(options, &m_zone);
}

inline void wamp_register_message::set_procedure(const std::string& procedure)
{
    m_procedure = msgpack::object(procedure, &m_zone);
}

inline std::ostream& operator<<(std::ostream& os, const wamp_register_message& message)
{
    os << "register [" << message.get_request_id() << ", "
            << message.get_options() << ", "
            << message.get_procedure() << "]";
    return os;
}

} // namespace bonefish

#endif // BONEFISH_MESSAGES_WAMP_REGISTER_MESSAGE_HPP
