#ifndef BONEFISH_WAMP_ROUTER_HPP
#define BONEFISH_WAMP_ROUTER_HPP

#include <bonefish/identifiers/wamp_session_id.hpp>
#include <bonefish/messages/wamp_welcome_details.hpp>
#include <bonefish/utility/wamp_uri.hpp>
#include <boost/asio.hpp>
#include <memory>
#include <unordered_set>
#include <unordered_map>

namespace bonefish {

class wamp_broker;
class wamp_dealer;
class wamp_call_message;
class wamp_goodbye_message;
class wamp_hello_message;
class wamp_publish_message;
class wamp_register_message;
class wamp_session;
class wamp_subscribe_message;
class wamp_unregister_message;
class wamp_unsubscribe_message;
class wamp_yield_message;

class wamp_router
{
public:
    wamp_router(boost::asio::io_service& io_service, const wamp_uri& realm);
    ~wamp_router();

    const wamp_uri& get_realm() const;
    bool has_session(const wamp_session_id& session_id);
    bool attach_session(const std::shared_ptr<wamp_session>& session);
    void close_session(const wamp_session_id& session_id, const wamp_uri& reason);
    bool detach_session(const wamp_session_id& session_id);

    void process_call_message(const wamp_session_id& session_id,
            const wamp_call_message* call_message);
    void process_hello_message(const wamp_session_id& session_id,
            const wamp_hello_message* hello_message);
    void process_goodbye_message(const wamp_session_id& session_id,
            const wamp_goodbye_message* goodbye_message);
    void process_publish_message(const wamp_session_id& session_id,
            const wamp_publish_message* publish_message);
    void process_register_message(const wamp_session_id& session_id,
            const wamp_register_message* register_message);
    void process_subscribe_message(const wamp_session_id& session_id,
            const wamp_subscribe_message* subscribe_message);
    void process_unregister_message(const wamp_session_id& session_id,
            const wamp_unregister_message* unregister_message);
    void process_unsubscribe_message(const wamp_session_id& session_id,
            const wamp_unsubscribe_message* unsubscribe_message);
    void process_yield_message(const wamp_session_id& session_id,
            const wamp_yield_message* yield_message);

private:
    wamp_uri m_realm;
    wamp_welcome_details m_welcome_details;
    std::unique_ptr<wamp_broker> m_broker;
    std::unique_ptr<wamp_dealer> m_dealer;
    std::unordered_map<wamp_session_id, std::shared_ptr<wamp_session>> m_sessions;
};

} // namespace bonefish

#endif // BONEFISH_WAMP_ROUTER_HPP
