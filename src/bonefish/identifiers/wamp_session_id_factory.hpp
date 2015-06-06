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

#ifndef BONEFISH_WAMP_SESSION_ID_FACTORY_HPP
#define BONEFISH_WAMP_SESSION_ID_FACTORY_HPP

#include <memory>
#include <string>

namespace bonefish {

class wamp_session_id_generator;

namespace wamp_session_id_factory {

std::shared_ptr<wamp_session_id_generator> create(const std::string& realm);

} // namespace wamp_session_id_factory
} // namespace bonefish

#endif // BONEFISH_WAMP_SESSION_ID_FACTORY_HPP
