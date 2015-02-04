#include <bonefish/broker/wamp_broker.hpp>
#include <bonefish/broker/wamp_broker_subscription.hpp>
#include <bonefish/broker/wamp_broker_topic.hpp>
#include <bonefish/messages/wamp_error_message.hpp>
#include <bonefish/messages/wamp_event_message.hpp>
#include <bonefish/messages/wamp_publish_message.hpp>
#include <bonefish/messages/wamp_published_message.hpp>
#include <bonefish/messages/wamp_subscribe_message.hpp>
#include <bonefish/messages/wamp_subscribed_message.hpp>
#include <bonefish/messages/wamp_unsubscribe_message.hpp>
#include <bonefish/messages/wamp_unsubscribed_message.hpp>
#include <bonefish/session/wamp_session.hpp>
#include <bonefish/transport/wamp_transport.hpp>

#include <iostream>

namespace bonefish {

wamp_broker::wamp_broker()
    : m_publication_id_generator()
    , m_subscription_id_generator()
    , m_sessions()
    , m_session_subscriptions()
    , m_topic_subscriptions()
    , m_subscription_topics()
{
}

wamp_broker::~wamp_broker()
{
}

void wamp_broker::attach_session(const std::shared_ptr<wamp_session>& session)
{
    std::cerr << "attach session: " << session->get_session_id() << std::endl;
    auto result = m_sessions.insert(
            std::make_pair(session->get_session_id(), std::move(session)));
    if (!result.second) {
        throw std::logic_error("broker session already registered");
    }
}

void wamp_broker::detach_session(const wamp_session_id& session_id)
{
    std::cerr << "detach session:" << session_id << std::endl;

    auto session_itr = m_sessions.find(session_id);
    if (session_itr == m_sessions.end()) {
        throw std::logic_error("broker session does not exist");
    }

    auto session_subscriptions_itr = m_session_subscriptions.find(session_id);
    if (session_subscriptions_itr != m_session_subscriptions.end()) {
        for (const auto& subscription_id : session_subscriptions_itr->second) {
            auto subscription_topics_itr = m_subscription_topics.find(subscription_id);
            if (subscription_topics_itr == m_subscription_topics.end()) {
                std::cerr << "error: broker subscription topics out of sync" << std::endl;
                continue;
            }

            wamp_uri topic = subscription_topics_itr->second->get_topic();
            subscription_topics_itr->second->remove_session(session_itr->second);
            if (subscription_topics_itr->second->get_sessions().size() == 0) {
                m_subscription_topics.erase(subscription_id);
            }

            auto topic_subscriptions_itr = m_topic_subscriptions.find(topic);
            if (topic_subscriptions_itr == m_topic_subscriptions.end()) {
                std::cerr << "error: broker topic subscriptions out of sync" << std::endl;
                continue;
            }

            topic_subscriptions_itr->second->remove_session(session_itr->second);
            if (topic_subscriptions_itr->second->get_sessions().size() == 0) {
                m_topic_subscriptions.erase(topic);
            }
        }

        m_session_subscriptions.erase(session_subscriptions_itr);
    }

    m_sessions.erase(session_itr);
}

void wamp_broker::process_publish_message(const wamp_session_id& session_id,
        const wamp_publish_message* publish_message)
{
    auto session_itr = m_sessions.find(session_id);
    if (session_itr == m_sessions.end()) {
        throw std::logic_error("broker session does not exist");
    }

    const wamp_uri topic = publish_message->get_topic();
    const wamp_publication_id publication_id = m_publication_id_generator.generate();

    auto topic_subscriptions_itr = m_topic_subscriptions.find(topic);
    if (topic_subscriptions_itr != m_topic_subscriptions.end()) {
        std::unique_ptr<wamp_event_message> event_message(new wamp_event_message);
        event_message->set_subscription_id(topic_subscriptions_itr->second->get_subscription_id());
        event_message->set_publication_id(publication_id);
        event_message->set_arguments(publish_message->get_arguments());
        event_message->set_arguments_kw(publish_message->get_arguments_kw());

        for (const auto& session : topic_subscriptions_itr->second->get_sessions()) {
            // TODO: Improve performance here by offering a transport api that
            //       takes in a pre-serialized buffer. That way we can serialize
            //       the message once and then send it to all of the subscribers.
            session->get_transport()->send_message(event_message.get());
        }
    }

    // TODO: Publish acknowledgements require support for publish options which
    //       we currently do not yet have working.
    //
    //std::unique_ptr<wamp_published_message> published_message(new wamp_published_message);
    //published_message->set_request_id(publish_message->get_request_id());
    //published_message->set_publication_id(publication_id);
    //session_itr->second->get_transport()->send_message(published_message.get());
}

void wamp_broker::process_subscribe_message(const wamp_session_id& session_id,
        const wamp_subscribe_message* subscribe_message)
{
    std::cerr << "processing subscribe message" << std::endl;
    auto session_itr = m_sessions.find(session_id);
    if (session_itr == m_sessions.end()) {
        throw std::logic_error("broker session does not exist");
    }

    wamp_subscription_id subscription_id;
    auto& session = session_itr->second;
    {
        auto result = m_topic_subscriptions.insert(
                std::make_pair(subscribe_message->get_topic(), nullptr));
        if (result.second) {
            subscription_id = m_subscription_id_generator.generate();
            result.first->second.reset(new wamp_broker_subscription(subscription_id));
            result.first->second->add_session(session);
            std::cerr << "created new broker subscription" << std::endl;
        } else {
            subscription_id = result.first->second->get_subscription_id();
            result.first->second->add_session(session);
            std::cerr << "added to existing broker subscription" << std::endl;
        }
    }

    {
        auto result = m_subscription_topics.insert(std::make_pair(subscription_id, nullptr));
        if (result.second) {
            result.first->second.reset(new wamp_broker_topic(subscribe_message->get_topic()));
            result.first->second->add_session(session);
            std::cerr << "created new broker topic" << std::endl;
        } else {
            result.first->second->add_session(session);
            std::cerr << "added to existing broker topic" << std::endl;
        }
    }

    m_session_subscriptions[session_id].insert(subscription_id);

    std::cerr << "sending subscribed message to the subscriber" << std::endl;
    std::unique_ptr<wamp_subscribed_message> subscribed_message(new wamp_subscribed_message);
    subscribed_message->set_request_id(subscribe_message->get_request_id());
    subscribed_message->set_subscription_id(subscription_id);
    session->get_transport()->send_message(subscribed_message.get());
}

void wamp_broker::process_unsubscribe_message(const wamp_session_id& session_id,
        const wamp_unsubscribe_message* unsubscribe_message)
{
    auto session_itr = m_sessions.find(session_id);
    if (session_itr == m_sessions.end()) {
        throw std::logic_error("broker session does not exist");
    }

    auto session_subscriptions_itr = m_session_subscriptions.find(session_id);
    if (session_subscriptions_itr == m_session_subscriptions.end()) {
        return send_error(session_itr->second->get_transport(), unsubscribe_message->get_type(),
                unsubscribe_message->get_request_id(), "wamp.error.no_subscriptions_for_session");
    }

    const wamp_subscription_id& subscription_id = unsubscribe_message->get_subscription_id();
    if (session_subscriptions_itr->second.erase(subscription_id) == 0) {
        return send_error(session_itr->second->get_transport(), unsubscribe_message->get_type(),
                unsubscribe_message->get_request_id(), "wamp.error.no_such_subscription");
    }

    auto subscription_topics_itr = m_subscription_topics.find(subscription_id);
    if (subscription_topics_itr == m_subscription_topics.end()) {
        std::cerr << "error: broker subscription topics out of sync" << std::endl;
    } else {
        wamp_uri topic = subscription_topics_itr->second->get_topic();
        subscription_topics_itr->second->remove_session(session_itr->second);
        if (subscription_topics_itr->second->get_sessions().size() == 0) {
            m_subscription_topics.erase(subscription_id);
        }

        auto topic_subscriptions_itr = m_topic_subscriptions.find(topic);
        if (topic_subscriptions_itr == m_topic_subscriptions.end()) {
            std::cerr << "error: broker topic subscription out of sync" << std::endl;
        } else {
            topic_subscriptions_itr->second->remove_session(session_itr->second);
            if (topic_subscriptions_itr->second->get_sessions().size() == 0) {
                m_topic_subscriptions.erase(topic);
            }
        }
    }

    std::cerr << "sending unsubscribed message to the subscriber" << std::endl;
    std::unique_ptr<wamp_unsubscribed_message> unsubscribed_message(new wamp_unsubscribed_message);
    unsubscribed_message->set_request_id(unsubscribe_message->get_request_id());
    session_itr->second->get_transport()->send_message(unsubscribed_message.get());
}

void wamp_broker::send_error(const std::unique_ptr<wamp_transport>& transport,
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
