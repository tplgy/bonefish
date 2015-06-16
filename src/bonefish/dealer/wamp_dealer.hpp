#ifndef BONEFISH_DEALER_WAMP_DEALER_HPP
#define BONEFISH_DEALER_WAMP_DEALER_HPP

#include <bonefish/identifiers/wamp_registration_id_generator.hpp>
#include <bonefish/identifiers/wamp_request_id_generator.hpp>
#include <bonefish/identifiers/wamp_registration_id.hpp>
#include <bonefish/identifiers/wamp_request_id.hpp>
#include <bonefish/identifiers/wamp_session_id.hpp>
#include <bonefish/messages/wamp_message_type.hpp>
#include <bonefish/utility/wamp_uri.hpp>

#include <boost/asio.hpp>
#include <memory>
#include <unordered_map>
#include <unordered_set>

namespace bonefish {

class wamp_call_message;
class wamp_dealer_invocation;
class wamp_dealer_registration;
class wamp_error_message;
class wamp_register_message;
class wamp_session;
class wamp_transport;
class wamp_unregister_message;
class wamp_yield_message;

class wamp_dealer
{
public:
    wamp_dealer(boost::asio::io_service& io_service);
    ~wamp_dealer();

    void attach_session(const std::shared_ptr<wamp_session>& session);
    void detach_session(const wamp_session_id& id);

    void process_call_message(const wamp_session_id& session_id,
            const wamp_call_message* call_message);
    void process_error_message(const wamp_session_id& session_id,
            const wamp_error_message* error_message);
    void process_register_message(const wamp_session_id& session_id,
            const wamp_register_message* register_message);
    void process_unregister_message(const wamp_session_id& session_id,
            const wamp_unregister_message* unregister_message);
    void process_yield_message(const wamp_session_id& session_id,
            const wamp_yield_message* yield_message);

private:
    void send_error(const std::unique_ptr<wamp_transport>& transport,
            const wamp_message_type request_type, const wamp_request_id& request_id,
            const std::string& error) const;

    void invocation_timeout_handler(const wamp_request_id& request_id,
            const boost::system::error_code& error);

private:
    boost::asio::io_service& m_io_service;

    wamp_request_id_generator m_request_id_generator;
    wamp_registration_id_generator m_registration_id_generator;

    std::unordered_map<wamp_session_id, std::shared_ptr<wamp_session>> m_sessions;
    std::unordered_map<wamp_session_id, std::unordered_set<wamp_registration_id>> m_session_registrations;
    std::unordered_map<std::string, std::unique_ptr<wamp_dealer_registration>> m_procedure_registrations;
    std::unordered_map<wamp_registration_id, std::string> m_registered_procedures;

    std::unordered_map<wamp_session_id, std::unordered_set<wamp_request_id>> m_session_invocations;
    std::unordered_map<wamp_request_id, std::unique_ptr<wamp_dealer_invocation>> m_pending_invocations;
};

} // namespace bonefish

#endif // BONEFISH_DEALER_WAMP_DEALER_HPP
