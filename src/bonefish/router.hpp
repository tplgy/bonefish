#ifndef BONEFISH_ROUTER_HPP
#define BONEFISH_ROUTER_HPP

#include <bonefish/session.hpp>
#include <bonefish/identifier/session_id.hpp>
#include <boost/asio/io_service.hpp>
#include <memory>
#include <unordered_map>

namespace bonefish {

class router
{
public:
    router(const std::string& realm);
    ~router();

    void run();
    void shutdown();

    const std::string& get_realm() const;
    bool has_session(const session_id& id);
    bool attach_session(std::shared_ptr<session>&& s);
    bool detach_session(const session_id& id);

private:
    const std::string m_realm;
    boost::asio::io_service m_io_service;
    std::unique_ptr<boost::asio::io_service::work> m_work;
    std::unordered_map<session_id, std::shared_ptr<session>> m_sessions;
};

} // namespace bonefish

#endif // BONEFISH_ROUTER_HPP
