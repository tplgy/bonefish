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

#include <bonefish/identifiers/wamp_session_id_factory.hpp>
#include <bonefish/identifiers/wamp_session_id.hpp>
#include <bonefish/identifiers/wamp_session_id_generator.hpp>

#include <mutex>
#include <unordered_map>

namespace bonefish {
namespace wamp_session_id_factory {

namespace {
std::mutex g_session_id_gererator_mutex;
std::unordered_map<std::string /*realm*/,
        std::shared_ptr<wamp_session_id_generator>> g_session_id_generators;
} // namespace

// Currently we can have only one router per realm and it is single threaded. As a result
// we capitalize on the fact that the underlying session id generator does not need a
// lock. If that ever changes we will need to make sure that we update the session id
// generator code accordingly.
std::shared_ptr<wamp_session_id_generator> create(const std::string& realm)
{
    std::lock_guard<std::mutex> lock(g_session_id_gererator_mutex);
    auto itr = g_session_id_generators.find(realm);
    if (itr == g_session_id_generators.end()) {
        auto generator = std::make_shared<wamp_session_id_generator>();
        g_session_id_generators[realm] = generator;
        return generator;
    }

    return itr->second;
}

} // namespace wamp_session_id_factory
} // namespace bonefish
