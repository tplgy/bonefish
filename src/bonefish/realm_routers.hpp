#ifndef BONEFISH_REALM_ROUTERS_HPP
#define BONEFISH_REALM_ROUTERS_HPP

#include <bonefish/router.hpp>

namespace bonefish {

class realm_routers
{
public:
    realm_routers();
    ~realm_routers();

    void add_router(const std::shared_ptr<router>& r);
    std::shared_ptr<router> get_router(const std::string& realm);
    void remove_router(const std::string& realm);

private:
    std::unordered_map<std::string, std::shared_ptr<router>> m_routers;
};

inline realm_routers::realm_routers()
    : m_routers()
{
}

inline realm_routers::~realm_routers()
{
}

inline void realm_routers::add_router(const std::shared_ptr<router>& r)
{
    auto result = m_routers.insert(std::make_pair(r->get_realm(), r));
    assert(result.second);
}

inline std::shared_ptr<router> realm_routers::get_router(const std::string& realm)
{
    auto itr = m_routers.find(realm);
    return itr != m_routers.end() ? itr->second : nullptr;
}

inline void realm_routers::remove_router(const std::string& realm)
{
    m_routers.erase(realm);
}

} // namespace bonefish

#endif // BONEFISH_REALM_ROUTERS_HPP
