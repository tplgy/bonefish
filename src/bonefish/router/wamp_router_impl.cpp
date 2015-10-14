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

#include <bonefish/router/wamp_router_impl.hpp>
#include <bonefish/broker/wamp_broker.hpp>
#include <bonefish/dealer/wamp_dealer.hpp>
#include <bonefish/identifiers/wamp_session_id.hpp>
#include <bonefish/identifiers/wamp_session_id_generator.hpp>
#include <bonefish/identifiers/wamp_session_id_factory.hpp>
#include <bonefish/messages/wamp_abort_message.hpp>
#include <bonefish/messages/wamp_error_message.hpp>
#include <bonefish/messages/wamp_goodbye_message.hpp>
#include <bonefish/messages/wamp_hello_message.hpp>
#include <bonefish/messages/wamp_publish_message.hpp>
#include <bonefish/messages/wamp_subscribe_message.hpp>
#include <bonefish/messages/wamp_unsubscribe_message.hpp>
#include <bonefish/messages/wamp_welcome_details.hpp>
#include <bonefish/messages/wamp_welcome_message.hpp>
#include <bonefish/roles/wamp_role.hpp>
#include <bonefish/roles/wamp_role_type.hpp>
#include <bonefish/session/wamp_session.hpp>
#include <bonefish/trace/trace.hpp>

#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>

namespace bonefish {

wamp_router_impl::wamp_router_impl(boost::asio::io_service& io_service, const std::string& realm)
    : m_realm(realm)
    , m_broker(realm)
    , m_dealer(io_service)
    , m_welcome_details()
    , m_session_id_generator(wamp_session_id_factory::create(realm))
    , m_sessions()
{
    // Setup the broker role and supported features
    wamp_role broker_role(wamp_role_type::BROKER);

    m_welcome_details.add_role(std::move(broker_role));

    // Setup the dealer role and supported features
    wamp_role_features dealer_features;
    dealer_features.set_attribute("call_timeout", true);

    wamp_role dealer_role(wamp_role_type::DEALER);
    dealer_role.set_features(std::move(dealer_features));

    m_welcome_details.add_role(std::move(dealer_role));
}

wamp_router_impl::~wamp_router_impl()
{
}

const std::string& wamp_router_impl::get_realm() const
{
    return m_realm;
}

const std::shared_ptr<wamp_session_id_generator>& wamp_router_impl::get_session_id_generator() const
{
    return m_session_id_generator;
}

bool wamp_router_impl::has_session(const wamp_session_id& session_id)
{
    return m_sessions.find(session_id) != m_sessions.end();
}

void wamp_router_impl::close_session(const wamp_session_id& session_id, const std::string& reason)
{
    auto session_itr = m_sessions.find(session_id);
    if (session_itr == m_sessions.end()) {
        throw std::logic_error("session does not exist");
    }

    BONEFISH_TRACE("closing session: %1%", *session_itr->second);
    auto& session = session_itr->second;
    if (session->get_state() == wamp_session_state::OPEN) {
        std::unique_ptr<wamp_goodbye_message> goodbye_message(new wamp_goodbye_message);
        goodbye_message->set_reason(reason);
        session->set_state(wamp_session_state::CLOSING);

        BONEFISH_TRACE("%1%, %2%", *session % *goodbye_message);
        session->get_transport()->send_message(std::move(*goodbye_message));
    }
}

bool wamp_router_impl::attach_session(const std::shared_ptr<wamp_session>& session)
{
    BONEFISH_TRACE("attaching session: %1%", *session);
    auto result = m_sessions.insert(
            std::make_pair(session->get_session_id(), session));

    if (result.second) {
        if (session->get_role(wamp_role_type::CALLER) ||
                session->get_role(wamp_role_type::CALLEE)) {
            m_dealer.attach_session(session);
        }

        if (session->get_role(wamp_role_type::SUBSCRIBER) ||
                session->get_role(wamp_role_type::PUBLISHER)) {
            m_broker.attach_session(session);
        }

        return true;
    }

    BONEFISH_TRACE("failed to attach session: %1%", *session);

    return false;
}

bool wamp_router_impl::detach_session(const wamp_session_id& session_id)
{
    auto session_itr = m_sessions.find(session_id);
    if (session_itr == m_sessions.end()) {
        return false;
    }

    const auto& session = session_itr->second;

    if (session->get_role(wamp_role_type::CALLER) ||
            session->get_role(wamp_role_type::CALLEE)) {
        m_dealer.detach_session(session_id);
    }

    if (session->get_role(wamp_role_type::SUBSCRIBER) ||
            session->get_role(wamp_role_type::PUBLISHER)) {
        m_broker.detach_session(session_id);
    }

    m_sessions.erase(session_itr);

    return true;
}

void wamp_router_impl::process_hello_message(const wamp_session_id& session_id,
        const wamp_hello_message* hello_message)
{
    auto session_itr = m_sessions.find(session_id);
    if (session_itr == m_sessions.end()) {
        throw std::logic_error("session does not exist");
    }

    auto& session = session_itr->second;
    BONEFISH_TRACE("%1%, %2%", *session % *hello_message);
    if (session->get_state() != wamp_session_state::NONE) {
        std::unique_ptr<wamp_abort_message> abort_message(new wamp_abort_message);
        abort_message->set_reason("wamp.error.session_already_open");
        BONEFISH_TRACE("%1%, %2%", *session % *abort_message);
        session->get_transport()->send_message(std::move(*abort_message));
        return;
    }

    const auto& roles = session->get_roles();
    if (roles.empty()) {
        std::unique_ptr<wamp_abort_message> abort_message(new wamp_abort_message);
        abort_message->set_reason("wamp.error.invalid_roles");
        BONEFISH_TRACE("%1%, %2%", *session % *abort_message);
        session->get_transport()->send_message(std::move(*abort_message));
        return;
    }

    session->set_state(wamp_session_state::OPEN);

    std::unique_ptr<wamp_welcome_message> welcome_message(new wamp_welcome_message);
    welcome_message->set_session_id(session_id);
    welcome_message->set_details(m_welcome_details.marshal(welcome_message->get_zone()));

    // If we fail to send the welcome message it is most likely that the
    // underlying network connection has been closed/lost which means
    // that the component is no longer reachable on this session. So all
    // we do here is trace the fact that this event occured.
    BONEFISH_TRACE("%1%, %2%", *session % *welcome_message);
    if (!session->get_transport()->send_message(std::move(*welcome_message))) {
        BONEFISH_TRACE("failed to send the welcome message: network failure");
    }
}

void wamp_router_impl::process_goodbye_message(const wamp_session_id& session_id,
        const wamp_goodbye_message* goodbye_message)
{
    auto session_itr = m_sessions.find(session_id);
    if (session_itr == m_sessions.end()) {
        throw std::logic_error("session does not exist");
    }

    auto& session = session_itr->second;
    if (session->get_state() == wamp_session_state::OPEN) {
        std::unique_ptr<wamp_goodbye_message> message(new wamp_goodbye_message);
        message->set_reason("wamp.error.goodbye_and_out");
        session->set_state(wamp_session_state::CLOSED);

        BONEFISH_TRACE("%1%, %2%", *session % *goodbye_message);
        if (!session->get_transport()->send_message(std::move(*message))) {
            BONEFISH_TRACE("failed to send goodbye message to component: network failure");
        }
    } else if (session->get_state() == wamp_session_state::CLOSING) {
        session->set_state(wamp_session_state::CLOSED);
    } else {
        throw std::logic_error("session already closed");
    }
}

void wamp_router_impl::process_call_message(const wamp_session_id& session_id,
        const wamp_call_message* call_message)
{
    m_dealer.process_call_message(session_id, call_message);
}

void wamp_router_impl::process_error_message(const wamp_session_id& session_id,
        const wamp_error_message* error_message)
{
    const auto request_type = error_message->get_request_type();
    if (request_type == wamp_message_type::INVOCATION) {
        m_dealer.process_error_message(session_id, error_message);
    } else {
        throw std::logic_error("received an unexpected error message");
    }
}

void wamp_router_impl::process_publish_message(const wamp_session_id& session_id,
        const wamp_publish_message* publish_message)
{
    m_broker.process_publish_message(session_id, publish_message);
}

void wamp_router_impl::process_register_message(const wamp_session_id& session_id,
        const wamp_register_message* register_message)
{
    m_dealer.process_register_message(session_id, register_message);
}

void wamp_router_impl::process_subscribe_message(const wamp_session_id& session_id,
        const wamp_subscribe_message* subscribe_message)
{
    m_broker.process_subscribe_message(session_id, subscribe_message);
}

void wamp_router_impl::process_unregister_message(const wamp_session_id& session_id,
        const wamp_unregister_message* unregister_message)
{
    m_dealer.process_unregister_message(session_id, unregister_message);
}

void wamp_router_impl::process_unsubscribe_message(const wamp_session_id& session_id,
        const wamp_unsubscribe_message* unsubscribe_message)
{
    m_broker.process_unsubscribe_message(session_id, unsubscribe_message);
}

void wamp_router_impl::process_yield_message(const wamp_session_id& session_id,
        const wamp_yield_message* yield_message)
{
    m_dealer.process_yield_message(session_id, yield_message);
}

} // namespace bonefish
