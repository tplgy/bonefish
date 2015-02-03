#include <bonefish/dealer/wamp_dealer.hpp>
#include <bonefish/dealer/wamp_dealer_registration.hpp>
#include <bonefish/messages/wamp_error_message.hpp>
#include <bonefish/messages/wamp_register_message.hpp>
#include <bonefish/messages/wamp_registered_message.hpp>
#include <bonefish/messages/wamp_unregister_message.hpp>
#include <bonefish/messages/wamp_unregistered_message.hpp>
#include <bonefish/session/wamp_session.hpp>
#include <iostream>
#include <stdexcept>

namespace bonefish {

wamp_dealer::wamp_dealer()
    : m_request_id_generator()
    , m_registration_id_generator()
    , m_sessions()
    , m_session_registrations()
    , m_procedure_registrations()
    , m_registered_procedures()
{
}

wamp_dealer::~wamp_dealer()
{
}

void wamp_dealer::attach_session(const std::shared_ptr<wamp_session>& session)
{
    std::cerr << "attach session: " << session->get_session_id() << std::endl;
    auto result = m_sessions.insert(std::make_pair(session->get_session_id(), session));
    if (!result.second) {
        throw std::logic_error("dealer session already registered");
    }
}

void wamp_dealer::detach_session(const wamp_session_id& session_id)
{
    std::cerr << "detach session:" << session_id << std::endl;

    auto session_itr = m_sessions.find(session_id);
    if (session_itr == m_sessions.end()) {
        throw(std::logic_error("broker session does not exist"));
    }

    auto session_registration_itr = m_session_registrations.find(session_id);
    if (session_registration_itr != m_session_registrations.end()) {
        auto registered_procedures_itr =
                m_registered_procedures.find(session_registration_itr->second);
        if (registered_procedures_itr == m_registered_procedures.end()) {
            throw std::logic_error("dealer registered procedures out of sync");
        }

        auto procedure_registrations_itr =
                m_procedure_registrations.find(registered_procedures_itr->second);
        if (procedure_registrations_itr == m_procedure_registrations.end()) {
            throw std::logic_error("dealer procedure registrations out of sync");
        }

        m_procedure_registrations.erase(procedure_registrations_itr);
        m_registered_procedures.erase(registered_procedures_itr);
        m_session_registrations.erase(session_registration_itr);
    }

    m_sessions.erase(session_itr);
}

void wamp_dealer::process_register_message(const wamp_session_id& session_id,
        const wamp_register_message* register_message)
{
    auto session_itr = m_sessions.find(session_id);
    if (session_itr == m_sessions.end()) {
        throw(std::logic_error("dealer session does not exist"));
    }

    const auto procedure = register_message->get_procedure();
    if (!is_valid_uri(procedure)) {
        return send_error(session_itr->second->get_transport(), register_message->get_type(),
                register_message->get_request_id(), "wamp.error.invalid_uri");
    }

    auto procedure_registrations_itr = m_procedure_registrations.find(procedure);
    if (procedure_registrations_itr != m_procedure_registrations.end()) {
        return send_error(session_itr->second->get_transport(), register_message->get_type(),
                register_message->get_request_id(), "wamp.error.procedure_already_exists");
    }

    const wamp_registration_id registration_id = m_registration_id_generator.generate();
    std::unique_ptr<wamp_dealer_registration> dealer_registration(
            new wamp_dealer_registration(session_itr->second, registration_id));
    m_procedure_registrations[procedure] = std::move(dealer_registration);

    m_session_registrations[session_id] = registration_id;
    m_registered_procedures[registration_id] = procedure;

    std::unique_ptr<wamp_registered_message> registered_message(new wamp_registered_message);
    registered_message->set_request_id(register_message->get_request_id());
    registered_message->set_registration_id(registration_id);
    session_itr->second->get_transport()->send_message(registered_message.get());
}

void wamp_dealer::process_unregister_message(const wamp_session_id& session_id,
        const wamp_unregister_message* unregister_message)
{
    auto session_itr = m_sessions.find(session_id);
    if (session_itr == m_sessions.end()) {
        throw(std::logic_error("dealer session does not exist"));
    }

    auto session_registration_itr = m_session_registrations.find(session_id);
    if (session_registration_itr == m_session_registrations.end()) {
        return send_error(session_itr->second->get_transport(), unregister_message->get_type(),
                unregister_message->get_request_id(), "wamp.error.no_such_registration");
    }

    auto registered_procedures_itr =
            m_registered_procedures.find(session_registration_itr->second);
    if (registered_procedures_itr == m_registered_procedures.end()) {
        std::cerr << "warning: dealer registered procedures out of sync" << std::endl;
    } else {
        auto procedure_registrations_itr =
                m_procedure_registrations.find(registered_procedures_itr->second);
        if (procedure_registrations_itr == m_procedure_registrations.end()) {
            std::cerr << "error: dealer procedure registrations out of sync" << std::endl;
        } else {
            m_procedure_registrations.erase(procedure_registrations_itr);
        }

        m_registered_procedures.erase(registered_procedures_itr);
    }

    m_session_registrations.erase(session_registration_itr);

    std::unique_ptr<wamp_unregistered_message> unregistered_message(new wamp_unregistered_message);
    unregistered_message->set_request_id(unregister_message->get_request_id());
    session_itr->second->get_transport()->send_message(unregistered_message.get());
}

void wamp_dealer::send_error(const std::unique_ptr<wamp_transport>& transport,
            const wamp_message_type request_type, const wamp_request_id& request_id,
            const wamp_uri& error) const
{
    std::unique_ptr<wamp_error_message> error_message(new wamp_error_message);
    error_message->set_request_type(request_type);
    error_message->set_request_id(request_id);
    error_message->set_error(error);

    transport->send_message(error_message.get());
}

} // namespace bonefish
