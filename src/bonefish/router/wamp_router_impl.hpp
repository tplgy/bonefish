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

#ifndef BONEFISH_WAMP_ROUTER_IMPL_HPP
#define BONEFISH_WAMP_ROUTER_IMPL_HPP

#include <bonefish/broker/wamp_broker.hpp>
#include <bonefish/dealer/wamp_dealer.hpp>
#include <bonefish/messages/wamp_welcome_details.hpp>

#include <boost/asio.hpp>
#include <memory>
#include <unordered_map>

namespace bonefish {

class wamp_call_message;
class wamp_error_message;
class wamp_goodbye_message;
class wamp_hello_message;
class wamp_publish_message;
class wamp_register_message;
class wamp_session;
class wamp_session_id;
class wamp_session_id_generator;
class wamp_subscribe_message;
class wamp_unregister_message;
class wamp_unsubscribe_message;
class wamp_yield_message;

class wamp_router_impl
{
public:
    wamp_router_impl(boost::asio::io_service& io_service, const std::string& realm);
    ~wamp_router_impl();

    const std::string& get_realm() const;
    const std::shared_ptr<wamp_session_id_generator>& get_session_id_generator() const;

    bool has_session(const wamp_session_id& session_id);
    void close_session(const wamp_session_id& session_id, const std::string& reason);
    bool attach_session(const std::shared_ptr<wamp_session>& session);
    bool detach_session(const wamp_session_id& session_id);

    void process_call_message(const wamp_session_id& session_id,
            const wamp_call_message* call_message);
    void process_error_message(const wamp_session_id& session_id,
            const wamp_error_message* error_message);
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
    std::string m_realm;
    wamp_broker m_broker;
    wamp_dealer m_dealer;
    wamp_welcome_details m_welcome_details;
    std::shared_ptr<wamp_session_id_generator> m_session_id_generator;
    std::unordered_map<wamp_session_id, std::shared_ptr<wamp_session>> m_sessions;
};

} // namespace bonefish

#endif // BONEFISH_WAMP_ROUTER_IMPL_HPP
