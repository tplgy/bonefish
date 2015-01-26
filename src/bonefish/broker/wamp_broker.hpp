#ifndef BONEFISH_BROKER_WAMP_BROKER_HPP
#define BONEFISH_BROKER_WAMP_BROKER_HPP

#include <bonefish/identifiers/wamp_session_id.hpp>
#include <bonefish/identifiers/wamp_subscription_id.hpp>
#include <bonefish/identifiers/wamp_subscription_id_generator.hpp>
#include <bonefish/wamp_uri.hpp>
#include <memory>
#include <unordered_map>
#include <unordered_set>

namespace bonefish {

class wamp_broker_subscription;
class wamp_broker_topic;
class wamp_session;
class wamp_subscribe_message;
class wamp_unsubscribe_message;

class wamp_broker
{
public:
    wamp_broker();
    ~wamp_broker();

    void attach_session(const std::shared_ptr<wamp_session>& session);
    void detach_session(const wamp_session_id& id);

    void process_subscribe_message(const wamp_session_id& session_id,
            const wamp_subscribe_message* subscribe_message);
    void process_unsubscribe_message(const wamp_session_id& session_id,
            const wamp_unsubscribe_message* unsubscribe_message);

private:
    wamp_subscription_id_generator m_subscription_id_generator;
    std::unordered_map<wamp_session_id, std::shared_ptr<wamp_session>> m_sessions;
    std::unordered_map<wamp_session_id, std::unordered_set<wamp_subscription_id>> m_session_subscriptions;
    std::unordered_map<wamp_uri, std::unique_ptr<wamp_broker_subscription>> m_topic_subscriptions;
    std::unordered_map<wamp_subscription_id, std::unique_ptr<wamp_broker_topic>> m_subscription_topics;
};

} // namespace bonefish

#endif // BONEFISH_BROKER_WAMP_BROKER_HPP
