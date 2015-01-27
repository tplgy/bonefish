#ifndef BONEFISH_BROKER_WAMP_BROKER_HPP
#define BONEFISH_BROKER_WAMP_BROKER_HPP

#include <bonefish/identifiers/wamp_publication_id.hpp>
#include <bonefish/identifiers/wamp_publication_id_generator.hpp>
#include <bonefish/identifiers/wamp_request_id.hpp>
#include <bonefish/identifiers/wamp_session_id.hpp>
#include <bonefish/identifiers/wamp_subscription_id.hpp>
#include <bonefish/identifiers/wamp_subscription_id_generator.hpp>
#include <bonefish/messages/wamp_message_type.hpp>
#include <bonefish/utility/wamp_uri.hpp>
#include <memory>
#include <unordered_map>
#include <unordered_set>

namespace bonefish {

class wamp_broker_subscription;
class wamp_broker_topic;
class wamp_publish_message;
class wamp_session;
class wamp_subscribe_message;
class wamp_transport;
class wamp_unsubscribe_message;

class wamp_broker
{
public:
    wamp_broker();
    ~wamp_broker();

    void attach_session(const std::shared_ptr<wamp_session>& session);
    void detach_session(const wamp_session_id& id);

    void process_publish_message(const wamp_session_id& session_id,
            const wamp_publish_message* subscribe_message);
    void process_subscribe_message(const wamp_session_id& session_id,
            const wamp_subscribe_message* subscribe_message);
    void process_unsubscribe_message(const wamp_session_id& session_id,
            const wamp_unsubscribe_message* unsubscribe_message);

private:
    void send_error(const std::unique_ptr<wamp_transport>& transport,
            const wamp_message_type request_type, const wamp_request_id& request_id,
            const wamp_uri& error) const;

private:
    wamp_publication_id_generator m_publication_id_generator;
    wamp_subscription_id_generator m_subscription_id_generator;
    std::unordered_map<wamp_session_id, std::shared_ptr<wamp_session>> m_sessions;
    std::unordered_map<wamp_session_id, std::unordered_set<wamp_subscription_id>> m_session_subscriptions;
    std::unordered_map<wamp_uri, std::unique_ptr<wamp_broker_subscription>> m_topic_subscriptions;
    std::unordered_map<wamp_subscription_id, std::unique_ptr<wamp_broker_topic>> m_subscription_topics;
};

} // namespace bonefish

#endif // BONEFISH_BROKER_WAMP_BROKER_HPP
