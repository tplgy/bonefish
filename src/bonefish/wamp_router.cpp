#include <bonefish/wamp_router.hpp>
#include <bonefish/broker/wamp_broker.hpp>
#include <bonefish/messages/wamp_abort_message.hpp>
#include <bonefish/messages/wamp_goodbye_message.hpp>
#include <bonefish/messages/wamp_hello_message.hpp>
#include <bonefish/messages/wamp_publish_message.hpp>
#include <bonefish/messages/wamp_subscribe_message.hpp>
#include <bonefish/messages/wamp_unsubscribe_message.hpp>
#include <bonefish/messages/wamp_welcome_message.hpp>
#include <bonefish/roles/wamp_role.hpp>
#include <bonefish/wamp_dealer.hpp>
#include <bonefish/wamp_session.hpp>
#include <iostream>
#include <stdexcept>

namespace bonefish {

wamp_router::wamp_router(const wamp_uri& realm)
    : m_realm(realm)
    , m_broker()
    , m_dealer()
    , m_sessions()
{
}

wamp_router::~wamp_router()
{
}

const wamp_uri& wamp_router::get_realm() const
{
    return m_realm;
}

bool wamp_router::has_session(const wamp_session_id& session_id)
{
    return m_sessions.find(session_id) != m_sessions.end();
}

bool wamp_router::attach_session(const std::shared_ptr<wamp_session>& session)
{
    std::cerr << "attach session: " << session->get_session_id() << std::endl;
    auto result = m_sessions.insert(
            std::make_pair(session->get_session_id(), session));

    m_dealer->attach_session(session);
    m_broker->attach_session(session);

    return result.second;
}

void wamp_router::close_session(const wamp_session_id& session_id, const wamp_uri& reason)
{
    std::cerr << "close session: " << session_id << std::endl;
    auto session_itr = m_sessions.find(session_id);
    if (session_itr == m_sessions.end()) {
        throw(std::logic_error("session does not exist"));
    }

    auto& session = session_itr->second;
    if (session->get_state() == wamp_session_state::OPEN) {
        std::unique_ptr<wamp_goodbye_message> message(new wamp_goodbye_message);
        message->set_reason(reason);
        session->set_state(wamp_session_state::CLOSING);
        return;
    }
}

bool wamp_router::detach_session(const wamp_session_id& session_id)
{
    std::cerr << "detach session: " << session_id << std::endl;
    m_dealer->detach_session(session_id);
    m_broker->detach_session(session_id);

    return m_sessions.erase(session_id) == 1;
}

void wamp_router::process_hello_message(const wamp_session_id& session_id,
        const wamp_hello_message* hello_message)
{
    auto session_itr = m_sessions.find(session_id);
    if (session_itr == m_sessions.end()) {
        throw(std::logic_error("session does not exist"));
    }

    auto& session = session_itr->second;
    if (session->get_state() != wamp_session_state::NONE) {
        std::unique_ptr<wamp_abort_message> abort_message(new wamp_abort_message);
        abort_message->set_reason("wamp.error.session_already_open");
        session->get_transport()->send_message(abort_message.get());
        return;
    }

    session->set_roles(hello_message->get_roles());
    session->set_state(wamp_session_state::OPEN);

    std::unique_ptr<wamp_welcome_message> welcome_message(new wamp_welcome_message);
    welcome_message->set_session_id(session_id);
    welcome_message->add_role(wamp_role(wamp_role_type::BROKER));
    welcome_message->add_role(wamp_role(wamp_role_type::DEALER));
    session->get_transport()->send_message(welcome_message.get());
}

void wamp_router::process_goodbye_message(const wamp_session_id& session_id,
        const wamp_goodbye_message* goodbye_message)
{
    auto session_itr = m_sessions.find(session_id);
    if (session_itr == m_sessions.end()) {
        throw(std::logic_error("session does not exist"));
    }

    auto& session = session_itr->second;
    if (session->get_state() == wamp_session_state::OPEN) {
        std::unique_ptr<wamp_goodbye_message> message(new wamp_goodbye_message);
        message->set_reason("wamp.error.goodbye_and_out");
        session->set_state(wamp_session_state::CLOSED);
        session->get_transport()->send_message(message.get());
    } else if (session->get_state() == wamp_session_state::CLOSING) {
        session->set_state(wamp_session_state::CLOSED);
    } else {
        throw(std::logic_error("session already closed"));
    }
}

void wamp_router::process_publish_message(const wamp_session_id& session_id,
        const wamp_publish_message* publish_message)
{
    m_broker->process_publish_message(session_id, publish_message);
}

void wamp_router::process_subscribe_message(const wamp_session_id& session_id,
        const wamp_subscribe_message* subscribe_message)
{
    m_broker->process_subscribe_message(session_id, subscribe_message);
}

void wamp_router::process_unsubscribe_message(const wamp_session_id& session_id,
        const wamp_unsubscribe_message* unsubscribe_message)
{
    m_broker->process_unsubscribe_message(session_id, unsubscribe_message);
}

} // namespace bonefish
