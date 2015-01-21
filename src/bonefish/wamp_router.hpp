#ifndef BONEFISH_WAMP_ROUTER_HPP
#define BONEFISH_WAMP_ROUTER_HPP

#include <bonefish/session.hpp>
#include <bonefish/identifier/session_id.hpp>
#include <memory>
#include <unordered_map>

namespace bonefish {

class hello_message;

class wamp_router
{
public:
    wamp_router(const std::string& realm);
    ~wamp_router();

    const std::string& get_realm() const;
    bool has_session(const session_id& id);
    bool attach_session(std::shared_ptr<session>&& s);
    bool detach_session(const session_id& id);

    void process_hello_message(const session_id& id, const hello_message* message);

private:
    const std::string m_realm;
    std::unordered_map<session_id, std::shared_ptr<session>> m_sessions;
};

} // namespace bonefish

#endif // BONEFISH_WAMP_ROUTER_HPP
