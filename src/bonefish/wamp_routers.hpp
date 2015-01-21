#ifndef BONEFISH_WAMP_ROUTERS_HPP
#define BONEFISH_WAMP_ROUTERS_HPP

#include <bonefish/wamp_router.hpp>

namespace bonefish {

class wamp_routers
{
public:
    wamp_routers();
    ~wamp_routers();

    bool add_router(const std::shared_ptr<wamp_router>& router);
    std::shared_ptr<wamp_router> get_router(const std::string& realm);
    void remove_router(const std::string& realm);

private:
    std::unordered_map<std::string, std::shared_ptr<wamp_router>> m_routers;
};

inline wamp_routers::wamp_routers()
    : m_routers()
{
}

inline wamp_routers::~wamp_routers()
{
}

inline bool wamp_routers::add_router(const std::shared_ptr<wamp_router>& router)
{
    auto result = m_routers.insert(std::make_pair(router->get_realm(), router));
    return result.second;
}

inline std::shared_ptr<wamp_router> wamp_routers::get_router(const std::string& realm)
{
    auto itr = m_routers.find(realm);
    return itr != m_routers.end() ? itr->second : nullptr;
}

inline void wamp_routers::remove_router(const std::string& realm)
{
    m_routers.erase(realm);
}

} // namespace bonefish

#endif // BONEFISH_WAMP_ROUTERS_HPP
