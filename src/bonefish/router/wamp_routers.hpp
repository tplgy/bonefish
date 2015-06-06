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

#ifndef BONEFISH_WAMP_ROUTERS_HPP
#define BONEFISH_WAMP_ROUTERS_HPP

#include <bonefish/router/wamp_router.hpp>

#include <unordered_map>

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
