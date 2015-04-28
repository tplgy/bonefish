#include <bonefish/dealer/wamp_dealer.hpp>
#include <bonefish/dealer/wamp_dealer_invocation.hpp>
#include <bonefish/dealer/wamp_dealer_registration.hpp>
#include <bonefish/messages/wamp_call_message.hpp>
#include <bonefish/messages/wamp_error_message.hpp>
#include <bonefish/messages/wamp_invocation_message.hpp>
#include <bonefish/messages/wamp_register_message.hpp>
#include <bonefish/messages/wamp_registered_message.hpp>
#include <bonefish/messages/wamp_result_message.hpp>
#include <bonefish/messages/wamp_unregister_message.hpp>
#include <bonefish/messages/wamp_unregistered_message.hpp>
#include <bonefish/messages/wamp_yield_message.hpp>
#include <bonefish/session/wamp_session.hpp>
#include <bonefish/trace/trace.hpp>

#include <iostream>
#include <stdexcept>

namespace bonefish {

wamp_dealer::wamp_dealer(boost::asio::io_service& io_service)
    : m_request_id_generator()
    , m_registration_id_generator()
    , m_sessions()
    , m_session_registrations()
    , m_procedure_registrations()
    , m_registered_procedures()
    , m_io_service(io_service)
    , m_pending_invocations()
{
}

wamp_dealer::~wamp_dealer()
{
}

void wamp_dealer::attach_session(const std::shared_ptr<wamp_session>& session)
{
    BONEFISH_TRACE("attach session: %1%", *session);
    auto result = m_sessions.insert(std::make_pair(session->get_session_id(), session));
    if (!result.second) {
        throw std::logic_error("dealer session already registered");
    }
}

void wamp_dealer::detach_session(const wamp_session_id& session_id)
{
    auto session_itr = m_sessions.find(session_id);
    if (session_itr == m_sessions.end()) {
        throw std::logic_error("broker session does not exist");
    }

    BONEFISH_TRACE("detach session: %1%", *session_itr->second);
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

void wamp_dealer::process_call_message(const wamp_session_id& session_id,
        const wamp_call_message* call_message)
{
    auto session_itr = m_sessions.find(session_id);
    if (session_itr == m_sessions.end()) {
        throw std::logic_error("dealer session does not exist");
    }

    BONEFISH_TRACE("%1%, %2%", *session_itr->second % *call_message);
    const auto procedure = call_message->get_procedure();
    if (!is_valid_uri(procedure)) {
        return send_error(session_itr->second->get_transport(), call_message->get_type(),
                call_message->get_request_id(), "wamp.error.invalid_uri");
    }

    auto procedure_registrations_itr = m_procedure_registrations.find(procedure);
    if (procedure_registrations_itr == m_procedure_registrations.end()) {
        return send_error(session_itr->second->get_transport(), call_message->get_type(),
                call_message->get_request_id(), "wamp.error.no_such_procedure");
    }

    // TODO: Check for invalid call arguments. If they are invalid then send an
    //       error back to the caller.

    const wamp_request_id request_id = m_request_id_generator.generate();
    std::unique_ptr<wamp_dealer_invocation> dealer_invocation(
            new wamp_dealer_invocation(m_io_service));

    dealer_invocation->set_session(session_itr->second);
    dealer_invocation->set_request_id(call_message->get_request_id());
    dealer_invocation->set_timeout(
            std::bind(&wamp_dealer::invocation_timeout_handler, this, request_id, std::placeholders::_1), 30);

    m_pending_invocations.insert(std::make_pair(request_id, std::move(dealer_invocation)));

    const std::shared_ptr<wamp_session>& session =
            procedure_registrations_itr->second->get_session();

    const wamp_registration_id& registration_id =
            procedure_registrations_itr->second->get_registration_id();

    std::unique_ptr<wamp_invocation_message> invocation_message(new wamp_invocation_message);
    invocation_message->set_request_id(request_id);
    invocation_message->set_registration_id(registration_id);
    invocation_message->set_arguments(call_message->get_arguments());
    invocation_message->set_arguments_kw(call_message->get_arguments_kw());

    BONEFISH_TRACE("%1%, %2%", *session_itr->second % *invocation_message);
    session->get_transport()->send_message(invocation_message.get());
}

void wamp_dealer::process_error_message(const wamp_session_id& session_id,
        const wamp_error_message* error_message)
{
    auto session_itr = m_sessions.find(session_id);
    if (session_itr == m_sessions.end()) {
        throw std::logic_error("dealer session does not exist");
    }

    BONEFISH_TRACE("%1%, %2%", *session_itr->second % *error_message);
    const auto request_id = error_message->get_request_id();
    auto pending_invocations_itr = m_pending_invocations.find(request_id);
    if (pending_invocations_itr == m_pending_invocations.end()) {
        BONEFISH_TRACE("error: unable to find invocation ... ignoring error");
        return;
    }

    const auto& dealer_invocation = pending_invocations_itr->second;
    const auto& session = dealer_invocation->get_session();

    std::unique_ptr<wamp_error_message> caller_error_message(new wamp_error_message);
    caller_error_message->set_request_type(wamp_message_type::CALL);
    caller_error_message->set_request_id(dealer_invocation->get_request_id());
    caller_error_message->set_details(error_message->get_details());
    caller_error_message->set_error(error_message->get_error());
    caller_error_message->set_arguments(error_message->get_arguments());
    caller_error_message->set_arguments_kw(error_message->get_arguments_kw());

    session->get_transport()->send_message(caller_error_message.get());
    m_pending_invocations.erase(pending_invocations_itr);
}

void wamp_dealer::process_register_message(const wamp_session_id& session_id,
        const wamp_register_message* register_message)
{
    auto session_itr = m_sessions.find(session_id);
    if (session_itr == m_sessions.end()) {
        throw std::logic_error("dealer session does not exist");
    }

    BONEFISH_TRACE("%1%, %2%", *session_itr->second % *register_message);
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

    BONEFISH_TRACE("%1%, %2%", *session_itr->second % *registered_message);
    session_itr->second->get_transport()->send_message(registered_message.get());
}

void wamp_dealer::process_unregister_message(const wamp_session_id& session_id,
        const wamp_unregister_message* unregister_message)
{
    auto session_itr = m_sessions.find(session_id);
    if (session_itr == m_sessions.end()) {
        throw std::logic_error("dealer session does not exist");
    }

    BONEFISH_TRACE("%1%, %2%", *session_itr->second % *unregister_message);
    auto session_registration_itr = m_session_registrations.find(session_id);
    if (session_registration_itr == m_session_registrations.end()) {
        return send_error(session_itr->second->get_transport(), unregister_message->get_type(),
                unregister_message->get_request_id(), "wamp.error.no_such_registration");
    }

    auto registered_procedures_itr =
            m_registered_procedures.find(session_registration_itr->second);
    if (registered_procedures_itr == m_registered_procedures.end()) {
        BONEFISH_TRACE("error: dealer registered procedures out of sync");
    } else {
        auto procedure_registrations_itr =
                m_procedure_registrations.find(registered_procedures_itr->second);
        if (procedure_registrations_itr == m_procedure_registrations.end()) {
            BONEFISH_TRACE("error: dealer procedure registrations out of sync");
        } else {
            m_procedure_registrations.erase(procedure_registrations_itr);
        }

        m_registered_procedures.erase(registered_procedures_itr);
    }

    m_session_registrations.erase(session_registration_itr);

    std::unique_ptr<wamp_unregistered_message> unregistered_message(new wamp_unregistered_message);
    unregistered_message->set_request_id(unregister_message->get_request_id());

    BONEFISH_TRACE("%1%, %2%", *session_itr->second % *unregistered_message);
    session_itr->second->get_transport()->send_message(unregistered_message.get());
}

void wamp_dealer::process_yield_message(const wamp_session_id& session_id,
        const wamp_yield_message* yield_message)
{
    auto session_itr = m_sessions.find(session_id);
    if (session_itr == m_sessions.end()) {
        throw std::logic_error("dealer session does not exist");
    }

    BONEFISH_TRACE("%1%, %2%", *session_itr->second % *yield_message);
    const auto request_id = yield_message->get_request_id();
    auto pending_invocations_itr = m_pending_invocations.find(request_id);
    if (pending_invocations_itr == m_pending_invocations.end()) {
        BONEFISH_TRACE("unable to find invocation ... timed out or session closed");
        return;
    }

    const auto& dealer_invocation = pending_invocations_itr->second;
    const auto& session = dealer_invocation->get_session();
    std::unique_ptr<wamp_result_message> result_message(new wamp_result_message);
    result_message->set_request_id(dealer_invocation->get_request_id());
    result_message->set_arguments(yield_message->get_arguments());
    result_message->set_arguments_kw(yield_message->get_arguments_kw());

    BONEFISH_TRACE("%1%, %2%", *session_itr->second % *result_message);
    session->get_transport()->send_message(result_message.get());

    m_pending_invocations.erase(pending_invocations_itr);
}

void wamp_dealer::send_error(const std::unique_ptr<wamp_transport>& transport,
            const wamp_message_type request_type, const wamp_request_id& request_id,
            const std::string& error) const
{
    std::unique_ptr<wamp_error_message> error_message(new wamp_error_message);
    error_message->set_request_type(request_type);
    error_message->set_request_id(request_id);
    error_message->set_error(error);

    BONEFISH_TRACE("%1%", *error_message);
    transport->send_message(error_message.get());
}

void wamp_dealer::invocation_timeout_handler(const wamp_request_id& request_id,
        const boost::system::error_code& error)
{
    if (error == boost::asio::error::operation_aborted) {
        return;
    }

    auto pending_invocations_itr = m_pending_invocations.find(request_id);
    if (pending_invocations_itr == m_pending_invocations.end()) {
        BONEFISH_TRACE("error: unable to find pending invocation");
        return;
    }

    BONEFISH_TRACE("timing out a pending invocation");
    const auto& call_request_id = pending_invocations_itr->second->get_request_id();
    const std::shared_ptr<wamp_session>& session = pending_invocations_itr->second->get_session();
    send_error(session->get_transport(), wamp_message_type::CALL,
            call_request_id, "wamp.error.call_timed_out");

    m_pending_invocations.erase(pending_invocations_itr);
}

} // namespace bonefish
