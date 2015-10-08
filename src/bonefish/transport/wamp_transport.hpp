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

#ifndef BONEFISH_TRANSPORT_WAMP_TRANSPORT_HPP
#define BONEFISH_TRANSPORT_WAMP_TRANSPORT_HPP

namespace bonefish {

class wamp_message;

class wamp_transport
{
public:
    virtual ~wamp_transport() = default;
    virtual bool send_message(wamp_message&& message) = 0;
};

} // namespace bonefish

#endif // BONEFISH_TRANSPORT_WAMP_TRANSPORT_HPP
