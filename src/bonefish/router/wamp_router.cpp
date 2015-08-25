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

#include <bonefish/router/wamp_router.hpp>
#include <bonefish/broker/wamp_broker.hpp>
#include <bonefish/dealer/wamp_dealer.hpp>
#include <bonefish/identifiers/wamp_session_id.hpp>
#include <bonefish/identifiers/wamp_session_id_generator.hpp>
#include <bonefish/messages/wamp_abort_message.hpp>
#include <bonefish/messages/wamp_error_message.hpp>
#include <bonefish/messages/wamp_goodbye_message.hpp>
#include <bonefish/messages/wamp_hello_details.hpp>
#include <bonefish/messages/wamp_hello_message.hpp>
#include <bonefish/messages/wamp_publish_message.hpp>
#include <bonefish/messages/wamp_subscribe_message.hpp>
#include <bonefish/messages/wamp_unsubscribe_message.hpp>
#include <bonefish/roles/wamp_role.hpp>
#include <bonefish/router/wamp_router_impl.hpp>
#include <bonefish/session/wamp_session.hpp>

#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>

namespace bonefish {

wamp_router::wamp_router(boost::asio::io_service& io_service, const std::string& realm)
    : m_impl(new wamp_router_impl(io_service, realm))
{
}

wamp_router::~wamp_router()
{
}

const std::string& wamp_router::get_realm() const
{
    return m_impl->get_realm();
}

const std::shared_ptr<wamp_session_id_generator>& wamp_router::get_session_id_generator() const
{
    return m_impl->get_session_id_generator();
}


bool wamp_router::has_session(const wamp_session_id& session_id)
{
    return m_impl->has_session(session_id);
}

bool wamp_router::attach_session(const std::shared_ptr<wamp_session>& session)
{
    return m_impl->attach_session(session);
}

void wamp_router::close_session(const wamp_session_id& session_id, const std::string& reason)
{
    m_impl->close_session(session_id, reason);
}

bool wamp_router::detach_session(const wamp_session_id& session_id)
{
    return m_impl->detach_session(session_id);
}

void wamp_router::process_hello_message(const wamp_session_id& session_id,
        const wamp_hello_message* hello_message)
{
    m_impl->process_hello_message(session_id, hello_message);
}

void wamp_router::process_goodbye_message(const wamp_session_id& session_id,
        const wamp_goodbye_message* goodbye_message)
{
    m_impl->process_goodbye_message(session_id, goodbye_message);
}

void wamp_router::process_call_message(const wamp_session_id& session_id,
        const wamp_call_message* call_message)
{
    m_impl->process_call_message(session_id, call_message);
}

void wamp_router::process_error_message(const wamp_session_id& session_id,
        const wamp_error_message* error_message)
{
    m_impl->process_error_message(session_id, error_message);
}

void wamp_router::process_publish_message(const wamp_session_id& session_id,
        const wamp_publish_message* publish_message)
{
    m_impl->process_publish_message(session_id, publish_message);
}

void wamp_router::process_register_message(const wamp_session_id& session_id,
        const wamp_register_message* register_message)
{
    m_impl->process_register_message(session_id, register_message);
}

void wamp_router::process_subscribe_message(const wamp_session_id& session_id,
        const wamp_subscribe_message* subscribe_message)
{
    m_impl->process_subscribe_message(session_id, subscribe_message);
}

void wamp_router::process_unregister_message(const wamp_session_id& session_id,
        const wamp_unregister_message* unregister_message)
{
    m_impl->process_unregister_message(session_id, unregister_message);
}

void wamp_router::process_unsubscribe_message(const wamp_session_id& session_id,
        const wamp_unsubscribe_message* unsubscribe_message)
{
    m_impl->process_unsubscribe_message(session_id, unsubscribe_message);
}

void wamp_router::process_yield_message(const wamp_session_id& session_id,
        const wamp_yield_message* yield_message)
{
    m_impl->process_yield_message(session_id, yield_message);
}

} // namespace bonefish
