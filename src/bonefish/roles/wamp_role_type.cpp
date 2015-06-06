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

#include <bonefish/roles/wamp_role_type.hpp>

#include <cassert>
#include <stdexcept>

namespace bonefish {

const char* role_type_to_string(const wamp_role_type& type)
{
    const char* str = nullptr;
    switch(type)
    {
        case wamp_role_type::CALLEE:
            str = "callee";
            break;
        case wamp_role_type::CALLER:
            str = "caller";
            break;
        case wamp_role_type::PUBLISHER:
            str = "publisher";
            break;
        case wamp_role_type::SUBSCRIBER:
            str = "subscriber";
            break;
        case wamp_role_type::DEALER:
            str = "dealer";
            break;
        case wamp_role_type::BROKER:
            str = "broker";
            break;
        default:
            throw std::invalid_argument("unknown role type");
            break;
    }

    return str;
}

wamp_role_type role_type_from_string(const std::string& type)
{
    if (type.compare("callee")) {
        return wamp_role_type::CALLEE;
    }

    if (type.compare("caller")) {
        return wamp_role_type::CALLER;
    }

    if (type.compare("publisher")) {
        return wamp_role_type::PUBLISHER;
    }

    if (type.compare("subscriber")) {
        return wamp_role_type::SUBSCRIBER;
    }

    if (type.compare("dealer")) {
        return wamp_role_type::DEALER;
    }

    if (type.compare("broker")) {
        return wamp_role_type::BROKER;
    }

    throw std::invalid_argument("unknown role type");
}

} // namespace bonefish
