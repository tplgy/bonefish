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
    : m_io_service(io_service)
    , m_request_id_generator()
    , m_registration_id_generator()
    , m_sessions()
    , m_session_registrations()
    , m_procedure_registrations()
    , m_registered_procedures()
    , m_session_invocations()
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
        throw std::logic_error("dealer session does not exist");
    }

    BONEFISH_TRACE("detach session: %1%", *session_itr->second);
    BONEFISH_TRACE("cleaning up session registratons");
    auto session_registrations_itr = m_session_registrations.find(session_id);
    if (session_registrations_itr != m_session_registrations.end()) {
        auto& registration_ids = session_registrations_itr->second;
        for (const auto& registration_id : registration_ids) {
            auto registered_procedures_itr =
                    m_registered_procedures.find(registration_id);
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
        }

        m_session_registrations.erase(session_registrations_itr);
    }

    BONEFISH_TRACE("cleaning up session invocations");
    auto session_invocations_itr = m_session_invocations.find(session_id);
    if (session_invocations_itr != m_session_invocations.end()) {
        const auto& request_ids = session_invocations_itr->second;
        for (const auto& request_id : request_ids) {
            m_pending_invocations.erase(request_id);
        }

        m_session_invocations.erase(session_invocations_itr);
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

    const wamp_request_id request_id = m_request_id_generator.generate();
    std::unique_ptr<wamp_dealer_invocation> dealer_invocation(
            new wamp_dealer_invocation(m_io_service));
    dealer_invocation->set_session(session_itr->second);
    dealer_invocation->set_request_id(call_message->get_request_id());
    dealer_invocation->set_timeout(
            std::bind(&wamp_dealer::invocation_timeout_handler, this, request_id, std::placeholders::_1), 30);

    m_session_invocations[session_id].insert(request_id);
    m_pending_invocations.insert(std::make_pair(request_id, std::move(dealer_invocation)));

    const wamp_registration_id& registration_id =
            procedure_registrations_itr->second->get_registration_id();
    std::shared_ptr<wamp_session> session =
            procedure_registrations_itr->second->get_session();

    std::unique_ptr<wamp_invocation_message> invocation_message(new wamp_invocation_message);
    invocation_message->set_request_id(request_id);
    invocation_message->set_registration_id(registration_id);
    invocation_message->set_arguments(call_message->get_arguments());
    invocation_message->set_arguments_kw(call_message->get_arguments_kw());

    BONEFISH_TRACE("%1%, %2%", *session_itr->second % *invocation_message);
    if (!session->get_transport()->send_message(invocation_message.get())) {
        BONEFISH_TRACE("sending invocation message to callee failed: network failure");
        return send_error(session_itr->second->get_transport(), call_message->get_type(),
                call_message->get_request_id(), "wamp.error.network_failure");
    }
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
        // This is a mormal condition. It means that the caller has ended its
        // session after issuing a call. There is nothing to report to the
        // callee in this case so we just silently drop the message.
        BONEFISH_TRACE("unable to find invocation ... dropping error message");
        return;
    }

    const auto& dealer_invocation = pending_invocations_itr->second;
    std::shared_ptr<wamp_session> session = dealer_invocation->get_session();

    std::unique_ptr<wamp_error_message> caller_error_message(new wamp_error_message);
    caller_error_message->set_request_type(wamp_message_type::CALL);
    caller_error_message->set_request_id(dealer_invocation->get_request_id());
    caller_error_message->set_details(error_message->get_details());
    caller_error_message->set_error(error_message->get_error());
    caller_error_message->set_arguments(error_message->get_arguments());
    caller_error_message->set_arguments_kw(error_message->get_arguments_kw());

    BONEFISH_TRACE("%1%, %2%", *session_itr->second % *caller_error_message);
    if (!session->get_transport()->send_message(caller_error_message.get())) {
        // There is no error message to propogate in this case as this error
        // message was initiated by the callee and sending the callee and error
        // message in response to an error message would not make any sense.
        // Besides, the callers session has ended.
        BONEFISH_TRACE("failed to send error message to caller: network failure");
    }

    // The failure to send a message in the event of a network failure
    // will detach the session. When this happens the pending invocations
    // be cleaned up. So we don't use an iterator here to erase the pending
    // invocation becuase it may have just been invalidated above.
    m_session_invocations[session->get_session_id()].erase(request_id);
    m_pending_invocations.erase(request_id);
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

    m_session_registrations[session_id].insert(registration_id);
    m_registered_procedures[registration_id] = procedure;

    std::unique_ptr<wamp_registered_message> registered_message(new wamp_registered_message);
    registered_message->set_request_id(register_message->get_request_id());
    registered_message->set_registration_id(registration_id);

    // If we fail to send the registered message it is most likely that
    // the underlying network connection has been closed/lost which means
    // that the callee is no longer reachable on this session. So all we
    // do here is trace the fact that this event occured.
    BONEFISH_TRACE("%1%, %2%", *session_itr->second % *registered_message);
    if (!session_itr->second->get_transport()->send_message(registered_message.get())) {
        BONEFISH_TRACE("failed to send registered message to caller: network failure");
    }
}

void wamp_dealer::process_unregister_message(const wamp_session_id& session_id,
        const wamp_unregister_message* unregister_message)
{
    auto session_itr = m_sessions.find(session_id);
    if (session_itr == m_sessions.end()) {
        throw std::logic_error("dealer session does not exist");
    }

    BONEFISH_TRACE("%1%, %2%", *session_itr->second % *unregister_message);
    auto session_registrations_itr = m_session_registrations.find(session_id);
    if (session_registrations_itr == m_session_registrations.end()) {
        return send_error(session_itr->second->get_transport(), unregister_message->get_type(),
                unregister_message->get_request_id(), "wamp.error.no_such_registration");
    }

    auto& registration_ids = session_registrations_itr->second;
    auto registration_ids_itr = registration_ids.find(
            unregister_message->get_registration_id());
    if (registration_ids_itr == registration_ids.end()) {
        return send_error(session_itr->second->get_transport(), unregister_message->get_type(),
                unregister_message->get_request_id(), "wamp.error.no_such_registration");
    }

    auto registered_procedures_itr =
            m_registered_procedures.find(*registration_ids_itr);
    if (registered_procedures_itr == m_registered_procedures.end()) {
        throw std::logic_error("dealer registered procedures out of sync");
    } else {
        auto procedure_registrations_itr =
                m_procedure_registrations.find(registered_procedures_itr->second);
        if (procedure_registrations_itr == m_procedure_registrations.end()) {
            throw std::logic_error("dealer procedure registrations out of sync");
        } else {
            m_procedure_registrations.erase(procedure_registrations_itr);
        }

        m_registered_procedures.erase(registered_procedures_itr);
    }

    registration_ids.erase(registration_ids_itr);
    if (registration_ids.empty()) {
        m_session_registrations.erase(session_registrations_itr);
    }

    std::unique_ptr<wamp_unregistered_message> unregistered_message(new wamp_unregistered_message);
    unregistered_message->set_request_id(unregister_message->get_request_id());

    // If we fail to send the unregistered message it is most likely that
    // the underlying network connection has been closed/lost which means
    // that the callee is no longer reachable on this session. So all we
    // do here is trace the fact that this event occured.
    BONEFISH_TRACE("%1%, %2%", *session_itr->second % *unregistered_message);
    if (!session_itr->second->get_transport()->send_message(unregistered_message.get())) {
        BONEFISH_TRACE("failed to send unregistered message to caller: network failure");
    }
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
    std::shared_ptr<wamp_session> session = dealer_invocation->get_session();

    std::unique_ptr<wamp_result_message> result_message(new wamp_result_message);
    result_message->set_request_id(dealer_invocation->get_request_id());
    result_message->set_arguments(yield_message->get_arguments());
    result_message->set_arguments_kw(yield_message->get_arguments_kw());

    // If we fail to send the result message it is most likely that the
    // underlying network connection has been closed/lost which means
    // that the callee is no longer reachable on this session. So all
    // we do here is trace the fact that this event occured.
    BONEFISH_TRACE("%1%, %2%", *session_itr->second % *result_message);
    if (!session->get_transport()->send_message(result_message.get())) {
        BONEFISH_TRACE("failed to send result message to caller: network failure");
    }

    // The failure to send a message in the event of a network failure
    // will detach the session. When this happens the pending invocations
    // be cleaned up. So we don't use an iterator here to erase the pending
    // invocation becuase it may have just been invalidated above.
    m_session_invocations[session->get_session_id()].erase(request_id);
    m_pending_invocations.erase(request_id);
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
    if (!transport->send_message(error_message.get())) {
        BONEFISH_TRACE("failed to send error message");
    }
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
    std::shared_ptr<wamp_session> session = pending_invocations_itr->second->get_session();
    send_error(session->get_transport(), wamp_message_type::CALL,
            request_id, "wamp.error.call_timed_out");

    // The failure to send a message in the event of a network failure
    // will detach the session. When this happens the pending invocations
    // be cleaned up. So we don't use an iterator here to erase the pending
    // invocation becuase it may have just been invalidated above.
    m_session_invocations[session->get_session_id()].erase(request_id);
    m_pending_invocations.erase(request_id);
}

} // namespace bonefish
